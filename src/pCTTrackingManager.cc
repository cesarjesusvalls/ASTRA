
#include "pCTTrackingManager.hh"
#include "phantom.hh"

#include <TCanvas.h>

#include <TEveManager.h>
#include <TEvePlot3D.h>
#include <TEveTrans.h>
#include <TGLViewer.h>
#include <TH1.h>
#include <TH2.h>
#include <TF2.h>
#include <TH3.h>
#include <TGraph.h>

#include "CMOSPixel.hh"
#include <iostream>
#include <fstream>
#include <string>
#include "TMath.h"
#include "TVirtualFitter.h"
#include "Math/Minimizer.h" 
#include "Math/Factory.h"
#include "Math/Functor.h"

#include <Math/Vector3D.h>
#include <Fit/Fitter.h>
using namespace ROOT::Math;

//************************************************************************************************************
pCTTrackingManager::pCTTrackingManager(pCTEvent* evt, pCTXML* cnf) : fevent(evt), fconfig(cnf) {Init(); return;}
//************************************************************************************************************

//************************************************************************************************************
bool checkHitsCompatibility(CMOSPixel* hit2, CMOSPixel* hit1, double Z){
//************************************************************************************************************
    return TVector3(hit1->GetX()-hit2->GetX(),hit1->GetY()-hit2->GetY(),Z).Theta() <= 1;
}



//************************************************************************************************************
std::vector< pCTTrack* > pCTTrackingManager::DoRTTracking(){
//************************************************************************************************************
    recoTracks.clear();
    isReco.clear();

    bool DEBUG = true;

    // cointainer for the output
    std::vector< pCTTrack* > recoTracks;

    // input info from SciDet
    std::vector< SciDetHit* > listOfSciHits = fevent->GetSciDetHits();
    std::vector< SciDetHit* >::iterator sciHit;

    int nbars(fconfig->GetSciDetNBars());
    int nlayers(fconfig->GetSciDetNLayers());
    int n_xpixels(fconfig->GetPlaneRows());
    int n_ypixels(fconfig->GetPlaneColumns());
    double barWidth(fconfig->GetSciDetBarX());

    double max_dist = 8;
    std::map<int, int> track_to_id;
    for (int s(0); s<fcmosTracks.size(); ++s){

         if(!isCMOSReco[s]) continue;
        // set true by default and make it false if the track building fails.
        isReco[s] = true;
        
        // steps
        // for each seed, compute a prediction in the first SciDet layer.

        // this distance is hardcoded... ideally mode to config XML!
        // last CMOS plane to first layer distance is set to 5mm
        double delta = 5;

        TVector3 seed_pos = GetSpacePoint(fcmosTracks[s][3],3);
        //cout << seed_pos.X() << "," << seed_pos.Y() << "," << seed_pos.Z() << endl;
        TVector3 seed_vec = (seed_pos-GetSpacePoint(fcmosTracks[s][3],3)).Unit();
        TVector3 pred(seed_pos.X()+seed_vec.X()*delta,seed_pos.Y()+seed_vec.Y()*delta,0);

        // compute 3D candidates from 2D hits in the first 2 layers.
        std::vector<TVector3> candidates;
        std::vector<std::vector<int>> candidates_barIDs;
        for(std::vector< SciDetHit* >::iterator hit2d_1=listOfSciHits.begin(); hit2d_1!=listOfSciHits.end(); hit2d_1++){
            if ((*hit2d_1)->GetLayerID() != 0) continue;
            double pos[2] = {0};
            (*hit2d_1)->GetOrientation() ? pos[0] = (-(*hit2d_1)->GetBarID()+nbars/2)*barWidth : pos[1] = (-(*hit2d_1)->GetBarID()+nbars/2)*barWidth;
            for(std::vector< SciDetHit* >::iterator hit2d_2=listOfSciHits.begin(); hit2d_2!=listOfSciHits.end(); hit2d_2++){
                if ((*hit2d_2)->GetLayerID() != 1) continue;
                (*hit2d_2)->GetOrientation() ? pos[0] = (-(*hit2d_2)->GetBarID()+nbars/2)*barWidth : pos[1] = (-(*hit2d_2)->GetBarID()+nbars/2)*barWidth;

                TVector3 new_point(pos[0],pos[1],(*hit2d_2)->GetLayerID()*barWidth);
                //std::cout << "new candidate: " << new_point.X() << "," << new_point.Y() << "," << new_point.Z() << std::endl;
                candidates.push_back(new_point);
                std::vector<int> pair_barIDs;
                pair_barIDs.push_back(((*hit2d_1)->GetLayerID())*nbars+(*hit2d_1)->GetBarID());
                pair_barIDs.push_back(((*hit2d_2)->GetLayerID())*nbars+(*hit2d_2)->GetBarID());
                candidates_barIDs.push_back(pair_barIDs);
            }
        }
        // break the ambiguities comparing seed predictions to the 3D candidates.
        int Naccepted = 0;
        for (std::vector< TVector3>::iterator candidate=candidates.begin(); candidate!=candidates.end(); candidate++){
            double dist = (pred-(*candidate)).Mag();
            if ((pred-(*candidate)).Mag() < max_dist){
                ++Naccepted;
            }
        }

        std::vector<TVector3> listOf3Dhits;
        std::vector<int> listOfBarIDs;

        cout << "track: " << s << " | Naccepted: " << Naccepted << endl;

        //if the seed candidates are too close or if there is not accepted seed candidate, the track is not reconstructed.
        if (Naccepted != 1){
            //std::cout << "breaking!!" << std::endl;
            isReco[s] = false;
            continue;
        }
        else{
            int counter = 0;
            for (std::vector< TVector3>::iterator candidate=candidates.begin(); candidate!=candidates.end(); candidate++){
                double dist = (pred-(*candidate)).Mag();

                //std::cout << "dist: " << dist << " to candidate: " << (*candidate).X() << "," << (*candidate).Y() << "," << (*candidate).Z() << std::endl;
                if ((pred-(*candidate)).Mag() < max_dist){
                    ++Naccepted;
                    listOf3Dhits.push_back((*candidate));
                    listOfBarIDs.push_back(candidates_barIDs[counter][0]);
                    listOfBarIDs.push_back(candidates_barIDs[counter][1]);
                    track_to_id[candidates_barIDs[counter][0]]=s;
                    track_to_id[candidates_barIDs[counter][1]]=s;
                    cout << candidates_barIDs[counter][0] << "," << candidates_barIDs[counter][1] << "," << s << endl;
                }
                counter++;
            }
        }

        int trueTrackID = 999;

        for (int layerNum(2); layerNum<nlayers; layerNum++){
            int min_dist = 1000;
            double costh    = 1000;
            std::vector<TVector3> layer_candidates;
            std::vector<int> layer_barIDs;
            for(std::vector< SciDetHit* >::iterator hit2d=listOfSciHits.begin(); hit2d!=listOfSciHits.end(); hit2d++){
                if ((*hit2d)->GetLayerID() != layerNum) continue;
                double pos[2] = {0};
                (*hit2d)->GetOrientation() ? pos[0] = (-(*hit2d)->GetBarID()+nbars/2)*barWidth : pos[1] = (-(*hit2d)->GetBarID()+nbars/2)*barWidth;
                (*hit2d)->GetOrientation() ? pos[1] = listOf3Dhits.back().Y() : pos[0] = listOf3Dhits.back().X();
                TVector3 step_candidate(pos[0],pos[1],(*hit2d)->GetLayerID()*barWidth);

                double dist = (listOf3Dhits.back()-step_candidate).Mag();
                //std::cout << "dist: " << dist << std::endl;
                if(dist < max_dist and dist < min_dist){
                    costh = abs((listOf3Dhits.back()-step_candidate).CosTheta());
                    min_dist = dist;
                    layer_candidates.push_back(step_candidate);
                    layer_barIDs.push_back(((*hit2d)->GetLayerID())*nbars+(*hit2d)->GetBarID());
                    if((*hit2d)->GetTrackID() < trueTrackID) trueTrackID = (*hit2d)->GetTrackID();
                }
            }

            if (layer_candidates.size()){
                listOf3Dhits.push_back(layer_candidates.back());
                if(track_to_id.count(layer_barIDs.back())){
                    isReco[track_to_id[layer_barIDs.back()]] = false;
                    isReco[s] = false;
                }
                else track_to_id[layer_barIDs.back()]=s;
                listOfBarIDs.push_back(layer_barIDs.back());
            }


            // else if(layer_candidates.size() >1){
            //     std::cout << "there is more than 1 candidate." << layer_barIDs.back() << std::endl;
            //     // if there is more than one candidate the info can not be separated and the track is not reconstructed.
            //     isReco[s] = false;
            //     break;
            // }


            else if(!layer_candidates.size()){
                // either the track is successfully finished (and there is another track with longer range)
                // or the track has a hole (situation we need to address).
                break;
            }
        }

        if(!isReco[s]) continue;
        auto tmp_trk = new pCTTrack();
        tmp_trk->Set3DHits(listOf3Dhits);
        tmp_trk->SetBarIDs(listOfBarIDs);
        if (trueTrackID<=(int) fevent->GetGunEnergyMap().size())
            tmp_trk->SetTrueEnergy(fevent->GetGunEnergyMap()[trueTrackID]);
        else
            tmp_trk->SetTrueEnergy(-1);

        recoTracks.push_back(tmp_trk);
        cout << "Reco: " << recoTracks.size() << endl;
    }

    return recoTracks;
}

//************************************************************************************************************
double pCTTrackingManager::TrackOptimality(vector <CMOSPixel*> inputPixels, pCTXML*  config){
//************************************************************************************************************
    // function Object to be minimized
    struct SumDistance2 {
        const vector <CMOSPixel*> fPixels;
        pCTXML* fconfig;
        SumDistance2(const vector <CMOSPixel*> g, pCTXML*  config) : fPixels(g), fconfig(config) {}
        // calculate distance line-point
        double distance2(double x,double y,double z, const double *p) {
            // distance line point is D= | (xp-x0) cross  ux |
            // where ux is direction of line and x0 is a point in the line (like t = 0)
            XYZVector xp(x,y,z);
            XYZVector x0(p[0], p[1], p[2]);
            XYZVector x1(p[0] + p[3], p[1] + p[4], p[2]+ p[5]);
            XYZVector u = (x1-x0).Unit();
            double d2 = ((xp-x0).Cross(u)).Mag2();
            return d2;
        }
        // implementation of the function to be minimized
        double operator() (const double *par) {
            assert(fPixels.size() != 0);
            double sum = 0;
            int p_it = 0;
            for (auto v:fPixels) {
                sum += distance2(v->GetX(),v->GetY(), fconfig->GetPosZi(p_it),par);
                p_it++;
            }
            return sum;
        }
    };

    ROOT::Fit::Fitter  fitter;
    // make the functor objet
    SumDistance2 sdist(inputPixels, config);
    ROOT::Math::Functor fcn(sdist,6);
    // set the function and the initial parameter values
    double pStart[6] = {1,1,1,1,1,1};
    fitter.SetFCN(fcn,pStart);
    // set step sizes different than default ones (0.3 times parameter values)
    for (int i = 0; i < 6; ++i) fitter.Config().ParSettings(i).SetStepSize(0.01);
    std::vector <double> p_result;
    bool ok = fitter.FitFCN();
    if (!ok) return 1E6;

    const ROOT::Fit::FitResult & result = fitter.Result();
    return result.MinFcnValue();
}

//************************************************************************************************************
void pCTTrackingManager::DoCMOSTracking(){
//************************************************************************************************************

    isCMOSReco.clear();
    fcmosTracks.clear();

    int Nplanes    = 4;
    int maxTracks  = 10;
    int Ntracks    = 1000;
    double zPos[4] = {fconfig->GetPosZ0(),fconfig->GetPosZ1(),fconfig->GetPosZ2(),fconfig->GetPosZ3()};
    std::map<G4int, std::vector< CMOSPixel* > > planeToHits = fevent->GetPixelHitsMap();
    std::map<G4int, std::vector< CMOSPixel*> >::iterator plane;
    std::vector< CMOSPixel*>::iterator cmosHit;

    for(plane=planeToHits.begin(); plane!=planeToHits.end(); plane++) 
        if ((*plane).second.size() < Ntracks) Ntracks = (*plane).second.size(); 

    if(Ntracks>maxTracks) {return;}

    std::map<int, std::vector< std::vector<CMOSPixel*>>> cmos_tracks;
    // start creating 1 cmos_track per hit in the first plane
    for(uint i(0); i<planeToHits[0].size(); i++){
        isCMOSReco[i] = true;
        std::vector<std::vector<CMOSPixel*>> new_set_of_tracks;
        std::vector<CMOSPixel*> new_track;
        new_track.push_back(planeToHits[0][i]);
        new_set_of_tracks.push_back(new_track);
        cmos_tracks[i] = new_set_of_tracks;
        int trks_size = 1;
        while(isCMOSReco[i] and trks_size<Nplanes){
            std::vector<std::vector<CMOSPixel*>> tmp_tracks;
            int tracks_to_rm = (int) cmos_tracks[i].size();
            for(int trk(0); trk<(int)cmos_tracks[i].size(); trk++){
                int p_it = (int)cmos_tracks[i][trk].size();
                int compat_count = 0;
                for(uint j(0); j<planeToHits[p_it].size(); j++){
                    if(checkHitsCompatibility(cmos_tracks[i][trk].back(),planeToHits[p_it][j],zPos[p_it]-zPos[p_it-1])){
                        std::vector<CMOSPixel*> new_tmp_track;
                        for(auto tmpTrkHit:cmos_tracks[i][trk]) new_tmp_track.push_back(tmpTrkHit);
                        new_tmp_track.push_back(planeToHits[p_it][j]);
                        tmp_tracks.push_back(new_tmp_track);
                        compat_count++;
                    }
                }
                if(compat_count==1) for (auto tmpTrk:tmp_tracks) cmos_tracks[i].push_back((tmpTrk));
                else isCMOSReco[i] = false;
                cmos_tracks[i].erase(cmos_tracks[i].begin(), cmos_tracks[i].begin() + tracks_to_rm);
            }
            trks_size++;
        }
    }

    std::vector<std::vector<CMOSPixel*>> result;
    for(uint i(0); i<planeToHits[0].size(); i++){
        if(isCMOSReco[i]){
            result.push_back(cmos_tracks[i][0]);
        }
    }

    fcmosTracks = result;
}

//************************************************************************************************************
TVector3 pCTTrackingManager::GetSpacePoint (CMOSPixel* pixel, int plane){
//************************************************************************************************************
    double zPos[4] = {fconfig->GetPosZ0(),fconfig->GetPosZ1(),fconfig->GetPosZ2(),fconfig->GetPosZ3()};
    double xPos =  fconfig->GetPixelX()*0.001*(pixel->GetX()-fconfig->GetPlaneColumns()*0.5);
    double yPos =  fconfig->GetPixelY()*0.001*(pixel->GetY()-fconfig->GetPlaneRows()*0.5);
    return TVector3(xPos,yPos,zPos[plane]);
}

//************************************************************************************************************
void pCTTrackingManager::DoCMOSChi2Tracking(){
//************************************************************************************************************
    isCMOSReco.clear();
    fcmosTracks.clear();

    // check how many tracks do we have and return if there are too many.
    int maxTracks  = 10;
    int Ntracks    = 1000;
    std::map<G4int, std::vector< CMOSPixel* > > planeToHits = fevent->GetPixelHitsMap();
    std::map<G4int, std::vector< CMOSPixel*> >::iterator plane;
    for(plane=planeToHits.begin(); plane!=planeToHits.end(); plane++) 
        if ((*plane).second.size() < Ntracks) Ntracks = (*plane).second.size(); 
    if(Ntracks>maxTracks) {return;}

    // form all possible track combinations and sort them by fitness (chi2).
    int tot_size = 0;
    for (int p_it(0); p_it<4; p_it++) tot_size += planeToHits[p_it].size();
    std::vector<std::pair<double,std::vector<CMOSPixel*>>> tracks_fitness;

    for(int i(0); i<(int)planeToHits[0].size(); i++){
        for(int j(0); j<(int)planeToHits[1].size(); j++){
            for(int k(0); k<(int)planeToHits[2].size(); k++){
                for(int l(0); l<(int)planeToHits[3].size(); l++){
                    std::vector< CMOSPixel* > track;
                    track.push_back(planeToHits[0][i]);
                    track.push_back(planeToHits[1][j]);
                    track.push_back(planeToHits[2][k]);
                    track.push_back(planeToHits[3][l]);
                    tracks_fitness.push_back(make_pair(TrackOptimality(track,fconfig),track));
                }
            }
        }
    }
    std::sort(tracks_fitness.begin(), tracks_fitness.end()); 

    // select tracks not sharing CMOSPixel*.
    std::vector<std::vector<CMOSPixel*>> result;
    std::vector<std::pair<double,std::vector<CMOSPixel*>>>::iterator trk;
    std::vector<CMOSPixel*> selPixels;
    int trk_cnt = 0;

    for(trk=tracks_fitness.begin(); trk!=tracks_fitness.end(); trk++){
        bool found = false;
        for(auto cmoshit:trk->second){
            std::vector<CMOSPixel*>::iterator it = std::find(selPixels.begin(), selPixels.end(), cmoshit);
            if(it != selPixels.end()){
                found = true;
                break;
            }
        }
        if (!found){
            isCMOSReco[trk_cnt] = true;

            for(auto cmoshit:trk->second) selPixels.push_back(cmoshit);
            result.push_back((trk->second));
            trk_cnt++;
        }
    }

    fcmosTracks = result;
}




//************************************************************************************************************
//std::pair<TH2F*,TH2F*> pCTTrackingManager::PosResolution(){
std::vector<std::pair<std::pair<double,double>,std::pair<double,double>>> pCTTrackingManager::phantomPositions(){
//************************************************************************************************************
TH1F* resolution = new TH1F("resolution","",100,0,10);
TH1F* resolutionX = new TH1F("resolutionX","",100,0,10);
TH1F* resolutionY = new TH1F("resolutionY","",100,0,10);
//TH2F* realPos = new TH2F("realPos","",500,0,100,500,0,100);
//TH2F* projPos = new TH2F("projPos","",500,0,100,500,0,100);
std::vector<std::pair<double,double>>  realPos ;
std::vector<std::pair<double,double>>  projPos ;
std::vector<std::pair<std::pair<double,double>,std::pair<double,double>>> res;
 for (int s(0); s<fcmosTracks.size(); ++s){

            if(!isCMOSReco[s]) continue;

          //back  std::cout << "track: " << s << std::endl;

            // set true by default and make it false if the track building fails.
            isReco[s] = true;
            TVector3 phantomHit = fevent->GetPhantomMidPos();
            std::cout<< "phantom mid pos = " << phantomHit.X() << ", " << phantomHit.Y() << ", " << phantomHit.Z() << std::endl;
            float phantomPosZ = (fconfig->GetPosZ2()+fconfig->GetPosZ1())*0.5;//phantomHit.Z();//
            // steps
            // for each seed, compute a prediction in the first SciDet layer.
           // TVector3 seed_pos = GetSpacePoint(fcmosTracks[s][3],3);
            TVector3 front_vec = (GetSpacePoint(fcmosTracks[s][1],1)-GetSpacePoint(fcmosTracks[s][0],0)).Unit();
            TVector3 back_vec = (GetSpacePoint(fcmosTracks[s][3],3)-GetSpacePoint(fcmosTracks[s][2],2)).Unit();

            double_t frontProjX= front_vec.X()*(phantomPosZ-GetSpacePoint(fcmosTracks[s][1],1).Z())/front_vec.Z()+GetSpacePoint(fcmosTracks[s][1],1).X();
            double_t frontProjY= front_vec.Y()*(phantomPosZ-GetSpacePoint(fcmosTracks[s][1],1).Z())/front_vec.Z()+GetSpacePoint(fcmosTracks[s][1],1).Y();

            float backProjX= back_vec.X()*(phantomPosZ-GetSpacePoint(fcmosTracks[s][2],2).Z())/back_vec.Z()+GetSpacePoint(fcmosTracks[s][2],2).X();
            float backProjY= back_vec.Y()*(phantomPosZ-GetSpacePoint(fcmosTracks[s][2],2).Z())/back_vec.Z()+GetSpacePoint(fcmosTracks[s][2],2).Y();

            TVector3 frontProj(frontProjX, frontProjY,phantomPosZ);
            TVector3 backProj(backProjX,backProjY,phantomPosZ);
            

            float frontD2 = pow(pow(phantomHit.X() - frontProj.X(),2) + pow(phantomHit.Y() - frontProj.Y(),2),0.5); //+ pow(phantomHit.Z() - frontProj.Z(),2),0.5);
            float backD2 = pow(pow(phantomHit.X() - backProj.X(),2) + pow(phantomHit.Y() - backProj.Y(),2),0.5); 
            /*
            std::cout << "World Position in 1st Tracker (computed): " << GetSpacePoint(fcmosTracks[s][0],0).X() << " " << GetSpacePoint(fcmosTracks[s][0],0).Y() << " " << GetSpacePoint(fcmosTracks[s][0],0).Z() <<std::endl;
            std::cout << "World Position in 2nd Tracker (computed): " << GetSpacePoint(fcmosTracks[s][1],1).X() << " " << GetSpacePoint(fcmosTracks[s][1],1).Y() << " " << GetSpacePoint(fcmosTracks[s][1],1).Z() <<std::endl;
            std::cout << "World Position in 3rd Tracker (computed): " << GetSpacePoint(fcmosTracks[s][2],2).X() << " " << GetSpacePoint(fcmosTracks[s][2],2).Y() << " " << GetSpacePoint(fcmosTracks[s][2],2).Z() <<std::endl;
            std::cout << "World Position in 4th Tracker (computed): " << GetSpacePoint(fcmosTracks[s][3],3).X() << " " << GetSpacePoint(fcmosTracks[s][3],3).Y() << " " << GetSpacePoint(fcmosTracks[s][3],3).Z() <<std::endl;
            */
           // std::cout << "Resolution:  " << frontD2 << std::endl;
            
            //realPos->Fill(phantomHit.X(),phantomHit.Y());
            //projPos->Fill(frontProjX,frontProjY);
            //realPos=make_pair(phantomHit.X(),phantomHit.Y());
            //projPos=make_pair(frontProjX,frontProjY);
            res.push_back(make_pair(make_pair(phantomHit.X(),phantomHit.Y()),make_pair(frontProjX,frontProjY)));

            //resolution->Fill(backD2);
            //resolutionX->Fill(phantomHit.X() - frontProj.X());
            //resolutionY->Fill(phantomHit.Y() - frontProj.Y());

    }
    //std::pair<TH1F*,TH1F*> res(resolutionX,resolutionY);
    //std::pair<TH1F*,TH1F*> res = make_pair(resolutionX,resolutionY);
    //std::pair<TH2F*,TH2F*> res = make_pair(realPos,projPos);
    //std::vector<std::pair<std::pair<double,double>>,std::pair<double,double>>> res = make_pair(realPos,projPos);
    //std::pair<TH1F*,TH1F*> res(resolutionX,resolutionY);

return res ;

}
