
#include "pCTTrackingManager.hh"
#include <TCanvas.h>

#include <TEveManager.h>
#include <TEvePlot3D.h>
#include <TEveTrans.h>
#include <TGLViewer.h>
#include <TF2.h>
#include <TH3.h>


#include "CMOSPixel.hh"
#include <iostream>
#include <fstream>
#include <string>
#include "TMath.h"
#include "TVirtualFitter.h"
#include "Math/Minimizer.h" 
#include "Math/Factory.h"
#include "Math/Functor.h"




pCTTrackingManager::pCTTrackingManager(pCTEvent* evt, pCTXML* cnf)
	: fevent(evt), fconfig(cnf) {Init(); return;}

pCTTrackingManager::pCTTrackingManager(pCTEvent* evt, pCTXML* cnf, std::vector<TVector3> pnts, std::vector<TVector3> vecs)
	: fevent(evt), fconfig(cnf), seedPoints(pnts), seedVecs(vecs) {Init(); return;}

// void pCTTrackingManager::DrawRecoEvent(){
//     std::map<int, std::vector<TVector3> >::iterator it;
//     for (it=recoTracks.begin(); it<NCMOSTracks; it++){
//     	// If the current trackID belongs to a track not reconstructed, jump to the next one
//     	if(!IsReconstructed[(*it).first]) continue;
//     	// Fill the 3D info in a ROOT drawable object
//         TNtuple *event3D = new TNtuple("3D_reco_event", "3D_reco_event", "x:y:z:c");
//     	std::vector<TVector3> points3D = (*it).second;
//         for (auto p:points3D) event3D->Fill(p.X(),p.Y(),z.X(),(*it).first);        
//         // Create a canvas and draw the info stored in the TNtuple
//         TCanvas *canv = new TCanvas("aux_canv", "aux_canv", 800, 600, 800, 600);
//         canv->cd(1);
//         event3D->Draw("x:y:z:c","","box2");
//         // Access to the TNtuple axis limits via its hiden TH3F 
//         TH3F *htemp = (TH3F*)gPad->GetPrimitive("htemp");
//         htemp->GetXaxis()->SetLimits(-100,2300);
//         htemp->GetYaxis()->SetLimits(-100,2300);
//         htemp->GetZaxis()->SetLimits(-5,20);
//         htemp->SetTitle("");
//         // Draw and Wait
//         canv->Update();
//         canv->WaitPrimitive();
//         // Remove the temporal objects created for the display
//         delete htemp;
//         delete canv;
//         delete event3D;
//     }
// }

void pCTTrackingManager::DrawGeometry(){

        // TCanvas *canv = new TCanvas("aux_canv", "aux_canv", 800, 600, 800, 600);
        // canv->cd(1);

        // canv->Update();
        // canv->WaitPrimitive();

        // delete canv;
/*
   TEveManager::Create();
   gEve->GetDefaultGLViewer()->SetCurrentCamera(TGLViewer::kCameraPerspXOY);
   TF2 *f2 = new TF2("f2","x**2 + y**2 - x**3 -8*x*y**4", -1., 1.2, -1.5, 1.5);
   f2->SetFillColor(45);
   auto x = new TEvePlot3D("EvePlot - TF2");
   x->SetLogZ(kTRUE);
   x->SetPlot(f2,"glsurf4");
   x->RefMainTrans().MoveLF(2, 1);
   gEve->AddElement(x);
   TH3F *h31 = new TH3F("h31", "h31", 10, -1, 1, 10, -1, 1, 10, -1, 1);
   h31->FillRandom("gaus");
   h31->SetFillColor(2);
   x = new TEvePlot3D("EvePlot - TH3F");
   x->SetPlot(h31, "glbox");
   x->RefMainTrans().MoveLF(2, -1);
   gEve->AddElement(x);
   gEve->Redraw3D(kTRUE);
*/
}


std::vector< pCTTrack* > pCTTrackingManager::DoTracking(){

    bool DEBUG = true;

    // cointainer for the output
    std::vector< pCTTrack* > recoTracks;

    // input info from SciDet
    std::vector< SciDetHit* > listOfSciHits = fevent->GetSciDetHits();
    std::vector< SciDetHit* >::iterator sciHit;
    if (DEBUG) cout << "# of listOfSciHits: " << listOfSciHits.size() << endl;

    int nbars(fconfig->GetSciDetNBars());
    int nlayers(fconfig->GetSciDetNLayers());
    int n_xpixels(fconfig->GetPlaneRows());
    int n_ypixels(fconfig->GetPlaneColumns());
    double barWidth(fconfig->GetSciDetBarX());

    double pitchX = 0.040; // 40 microns in mm
    double pitchY = 0.036; // 36 microns in mm

    double max_dist = 8;
    std::map<int, int> track_to_id;
    for (int s(0); s<seedPoints.size(); ++s){

        // set true by default and make it false if the track building fails.
        isReco[s] = true;
        
        // steps
        // for each seed, compute a prediction in the first SciDet layer.
        double delta = 0; // in the future compute distance CMOS to first layer using config...

        TVector3 seed_pos = seedPoints[s];
        TVector3 seed_vec = seedVecs[s];

        TVector3 pred((seed_pos.X()-n_xpixels/2)*pitchX+seed_vec.X()*delta, (seed_pos.Y()-n_ypixels/2)*pitchY+seed_vec.Y()*delta, 0);
        //std::cout << "seed: " << seed_pos.X() << "," << seed_pos.Y() << "," << seed_pos.Z() << std::endl;
        //std::cout << "pred: " << pred.X() << "," << pred.Y() << "," << pred.Z() << std::endl;

        // compute 3D candidates from 2D hits in the first 2 layers.
        std::vector<TVector3> candidates;
        std::vector<std::vector<int>> candidates_barIDs;
        for(std::vector< SciDetHit* >::iterator hit2d_1=listOfSciHits.begin(); hit2d_1!=listOfSciHits.end(); hit2d_1++){
            if ((*hit2d_1)->GetLayerID() != 0) continue;
            double pos[2] = {0};
            std::vector<int> pair_barIDs;
            (*hit2d_1)->GetOrientation() ? pos[0] = (-(*hit2d_1)->GetBarID()+nbars/2)*barWidth : pos[1] = (-(*hit2d_1)->GetBarID()+nbars/2)*barWidth;
            pair_barIDs.push_back(((*hit2d_1)->GetLayerID())*nbars+(*hit2d_1)->GetBarID());
            for(std::vector< SciDetHit* >::iterator hit2d_2=listOfSciHits.begin(); hit2d_2!=listOfSciHits.end(); hit2d_2++){
                if ((*hit2d_2)->GetLayerID() != 1) continue;
                (*hit2d_2)->GetOrientation() ? pos[0] = (-(*hit2d_2)->GetBarID()+nbars/2)*barWidth : pos[1] = (-(*hit2d_2)->GetBarID()+nbars/2)*barWidth;

                TVector3 new_point(pos[0],pos[1],(*hit2d_2)->GetLayerID()*barWidth);
                //std::cout << "new candidate: " << new_point.X() << "," << new_point.Y() << "," << new_point.Z() << std::endl;
                candidates.push_back(new_point);
                pair_barIDs.push_back(((*hit2d_2)->GetLayerID())*nbars+(*hit2d_2)->GetBarID());
                candidates_barIDs.push_back(pair_barIDs);
            }
        }
        // break the ambiguities comparing seed predictions to the 3D candidates.
        int Naccepted = 0;
        std::vector<TVector3> listOf3Dhits;
        std::vector<int> listOfBarIDs;
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
                //std::cout << "ini bar ids: " << candidates_barIDs[counter][0] << ", " << candidates_barIDs[counter][1] << std::endl;
            }
            counter++;
        }

        //if the seed candidates are too close or if there is not accepted seed candidate, the track is not reconstructed.
        //std::cout << "Naccepted: " << Naccepted << std::endl;
        if (Naccepted != 1){
            //std::cout << "breaking!!" << std::endl;
            isReco[s] = false;
            continue;
        }

        // process iteratively layer after layer.
        // for each layer generate a list of candidates and evaluate how many are
        // closer than 'max_dist' to the previous layer 3D point for the track being built.
        // for (int layerNum(2); layerNum<nlayers; layerNum++){
        //     std::vector<TVector3> layer_candidates;
        //     std::vector<int> layer_barIDs;
        //     for(std::vector< SciDetHit* >::iterator hit2d=listOfSciHits.begin(); hit2d!=listOfSciHits.end(); hit2d++){
        //         if ((*hit2d)->GetLayerID() != layerNum) continue;
        //         double pos[2] = {0};
        //         (*hit2d)->GetOrientation() ? pos[0] = (-(*hit2d)->GetBarID()+nbars/2)*barWidth : pos[1] = (-(*hit2d)->GetBarID()+nbars/2)*barWidth;
        //         (*hit2d)->GetOrientation() ? pos[1] = listOf3Dhits.back().Y() : pos[0] = listOf3Dhits.back().X();
        //         TVector3 step_candidate(pos[0],pos[1],(*hit2d)->GetLayerID()*barWidth);

        //         double dist = (listOf3Dhits.back()-step_candidate).Mag();
        //         std::cout << "dist: " << dist << std::endl;
        //         if(dist < max_dist){
        //             layer_candidates.push_back(step_candidate);
        //             layer_barIDs.push_back(((*hit2d)->GetLayerID())*nbars+(*hit2d)->GetBarID());
        //         }
        //     }
        //     if     (layer_candidates.size()==1){
        //         listOf3Dhits.push_back(layer_candidates.back());
        //         std::cout << "adding bar ids: " << layer_barIDs.back() << std::endl;
        //         listOfBarIDs.push_back(layer_barIDs.back());
        //     }
        //     else if(layer_candidates.size() >1){
        //         std::cout << "there is more than 1 candidate." << layer_barIDs.back() << std::endl;
        //         // if there is more than one candidate the info can not be separated and the track is not reconstructed.
        //         isReco[s] = false;
        //         break;
        //     }
        //     else if(!layer_candidates.size()){
        //         // either the track is successfully finished (and there is another track with longer range)
        //         // or the track has a hole (situation we need to address).
        //         break;
        //     }
        // }

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
                }
            }
            if (layer_candidates.size()){
                //std::cout << "costh: " << costh << std::endl;
                listOf3Dhits.push_back(layer_candidates.back());
                //std::cout << "adding bar ids: " << layer_barIDs.back() << std::endl;
                if(track_to_id.count(layer_barIDs.back())){
                    isReco[track_to_id[layer_barIDs.back()]] = false;
                }
                track_to_id[layer_barIDs.back()]=s;
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

        // now, if is Reco the listOf3Dhits constitute the reconstructed track
        // std::cout << "Track " << s << ", is reconstructed: " << isReco[s] << std::endl;
        // std::cout << "seed: " << pred.X() << "," << pred.Y() << "," << pred.Z() << std::endl;
        // for (std::vector< TVector3>::iterator point3d=listOf3Dhits.begin(); point3d!=listOf3Dhits.end(); point3d++){
        //     std::cout << "point3d: " << (*point3d).X() << "," << (*point3d).Y() << "," << (*point3d).Z() << std::endl;
        // }
        // for(std::vector<int>::iterator iter=listOfBarIDs.begin(); iter != listOfBarIDs.end(); iter++)
        //     std::cout << "barID: " << (*iter) << std::endl;
        // std:cout << "-------------" << std::endl;

        auto tmp_trk = new pCTTrack();
        tmp_trk->Set3DHits(listOf3Dhits);
        tmp_trk->SetBarIDs(listOfBarIDs);
        recoTracks.push_back(tmp_trk);

    }

    return recoTracks;
}



const int nPlanes= 4;
int x[nPlanes] = {0};
int y[nPlanes] = {0};
int trackID[nPlanes] = {0};
unsigned short int nTracks = 0;

double chi2(const double *par)
//double chi2(const double *par, Double_t zPos[4])
{

//Double_t z[4]= {fconfig->GetPosZ0(),fconfig->GetPosZ1(),fconfig->GetPosZ2(),fconfig->GetPosZ3()};
Double_t z[4]={0,5,10,15};
Double_t chi2=0;

for (int i=0; i<nPlanes;i++)
{
double XY[2]={par[2]*z[i]+par[0],par[3]*z[i]+par[1]};
//std::cout << "This is X and Y values at each point = " << x[i] << " " << y[i] << std::endl;
chi2 += pow((x[i]-XY[0]),2)+pow((y[i]-XY[1]),2);

}
return chi2;
}


std::vector<TVector3> pCTTrackingManager::SpacePoint (std::vector<std::pair <int ,std::pair <int,int>>> pixelPoint)
{   double zPos[nPlanes] = {fconfig->GetPosZ0(),fconfig->GetPosZ1(),fconfig->GetPosZ2(),fconfig->GetPosZ3()};
    double pitchY = 0.04;
    double pitchX = 0.036;
    int rows = fconfig->GetPlaneRows();
    int cols = fconfig->GetPlaneColumns();
    std::vector<TVector3> XYZ;
    for (int i=0; i<int(pixelPoint.size());i++)
    {
    double xPos = pitchX*(pixelPoint[i].second.first-cols*0.5);
    double yPos =  pitchY*(pixelPoint[i].second.second-rows*0.5);
    TVector3 xyz(xPos,yPos,zPos[i]);
     XYZ.push_back(xyz);
    }
return XYZ;
}


TVector3 pCTTrackingManager::Vd(TVector3 V1,TVector3 V2)
{
double m = (V2-V1).Mag();
TVector3 V3((V2-V1).x()/m,(V2-V1).y()/m,(V2-V1).z()/m);
return V3;
}




std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>> pCTTrackingManager::SortTracksByChi2(
//std::vector<std::pair<Double_t,std::vector<std::pair<int,int>>>> SortTracksByChi2(
std::vector< CMOSPixel*> Det1, 
std::vector< CMOSPixel*> Det2, 
std::vector< CMOSPixel*> Det3, 
std::vector< CMOSPixel*> Det4)


{   
    std::vector< CMOSPixel*>::iterator it1, it2, it3, it4;
    //std::vector<std::vector< CMOSPixel*>::iterator> its[4] = {it1,it2,it3,it4};

    //std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>> chi2Points;
    std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>> chi2Points;

            std::vector<std::pair <int,int>> points;
            std::vector<std::pair <int,std::pair <int,int>>> pointsTrackID;

            for (it1=Det1.begin(); it1!=Det1.end(); it1++)
            { 
                for (it2=Det2.begin(); it2!=Det2.end(); it2++)
                {   
                    for (it3=Det3.begin(); it3!=Det3.end(); it3++)
                    {   
                     for (it4=Det4.begin(); it4!=Det4.end(); it4++)
                        { 
                            /*
                                for( uint i(0); i<4; i++)
                                if ((*its[i])->GetTrackID() != -999)
                                {
                                    x[i]=(*its[i])->GetX();
                                    y[i]=(*its[i])->GetY();
                                    trackID[i]=(*its[i])->GetTrackID();
                                }

                                */

                            if ((*it1)->GetTrackID()!=-999)
                                {
                                    x[0]=(*it1)->GetX();
                                    y[0]=(*it1)->GetY();
                                    trackID[0]=(*it1)->GetTrackID();
                                }

                            if ((*it2)->GetTrackID()!=-999)
                                {
                                    x[1]=(*it2)->GetX();
                                    y[1]=(*it2)->GetY();
                                    trackID[1]=(*it2)->GetTrackID();
                                }

                            if ((*it3)->GetTrackID()!=-999)
                                {
                                    x[2]=(*it3)->GetX();
                                    y[2]=(*it3)->GetY();
                                    trackID[2]=(*it3)->GetTrackID();
                                }

                            if ((*it4)->GetTrackID()!=-999)
                                {
                                    x[3]=(*it4)->GetX();
                                    y[3]=(*it4)->GetY();
                                    trackID[3]=(*it4)->GetTrackID();
                                }
                           
                            
                            for (int i =0; i<nPlanes; i++) {



                                std::cout<< "XYT= ["<<x[i]<<", "<<y[i]<<", "<<trackID[i]<<"]"<<std::endl;
                                points.push_back(make_pair(x[i],y[i]));
                                
                                std::pair<int,int> XY = make_pair(x[i],y[i]);
                                pointsTrackID.push_back(make_pair(trackID[i],XY));
                            }

                            
                             ROOT::Math::Minimizer* minimum = ROOT::Math::Factory::CreateMinimizer("Minuit2");
                            // set tolerance , etc...
                                minimum->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
                                minimum->SetMaxIterations(100000);  // for GSL
                                minimum->SetTolerance(0.01);
                                minimum->SetPrintLevel(1);                            //TVirtualFitter* minuit = TVirtualFitter::Fitter(0,nPlanes);

                                // create function wrapper for minimizer
                                 // a IMultiGenFunction type
                                  ROOT::Math::Functor f(&chi2,4);
                                 double step[4] = {0.01,0.01,0.01,0.01};
 
                                minimum->SetFunction(f);
 
                                // Set the free variables to be minimized !
                                minimum->SetVariable(0,"x0",x[0], step[0]);
                                minimum->SetVariable(1,"y0",y[0], step[1]);
                                minimum->SetVariable(2,"Vx",0, step[2]);
                                minimum->SetVariable(3,"Vy",0, step[3]);
                             
                                // do the minimization
                                minimum->Minimize();
                                Double_t chi2 = minimum->MinValue();
                      
                            chi2Points.push_back(make_pair(chi2,pointsTrackID));
                            points.clear();
                            pointsTrackID.clear();
                           
                        }
                    }
                }
            }
            std::sort(chi2Points.begin(), chi2Points.end()); 
            
            return chi2Points;

 }




////// The kind of input exected for the TrackSelector is something like this:
/*


        CMOSPixel* defoult = new CMOSPixel();
        std::vector< CMOSPixel*>  det1, det2, det3, det4;
        det1.push_back(default);
        det2.push_back(default);
        det3.push_back(default);
        det4.push_back(default);

        nTracks =0;
        for(it2=Counter.begin(); it2!=Counter.end(); it2++)
        {
         unsigned short int Plane = (*it2).first;
         unsigned short int nHitsInPlane = (*it2).second.size();
        if (nHitsInPlane < nTracks || nTracks ==0) nTracks = nHitsInPlane;
        if (Plane==0)  det1 = (*it2).second;
        if (Plane==1)  det2 = (*it2).second;
        if (Plane==2)  det3 = (*it2).second;
        if (Plane==3)  det4 = (*it2).second;
        }
*/
//std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>> TrackSelector( std::vector<std::pair<Double_t, std::vector<std::pair<int,int>>>> sortedTracks)
std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>> pCTTrackingManager::TrackSelector(std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>> sortedTracks, int nTracks)

{

std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>>::iterator it,it2;
std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>> outPut,chi2Points;

double bestChi2SUM=0;
double chi2 = 0;
unsigned int Try =0;
for(it=sortedTracks.begin(); it!=sortedTracks.end(); it++) //run over the full set of tracks
        {
                        
                //std::vector<std::pair<int,int>>::iterator itPoint; //this iterator run over pairs (XY points) 
                std::vector<std::pair <int ,std::pair <int,int>>>::iterator itPoint; //this iterator run over pairs (XY points) 
                std::cout<<"This is the try number " << Try << std::endl;
                std::cout << "bestChi2 Sum = " << bestChi2SUM << " first chi2 is " << (*it).first << " and the cut is " << float(bestChi2SUM/(nTracks))<< std::endl;
                   if(bestChi2SUM != 0 and (*it).first >= float(bestChi2SUM/(nTracks))) {
                    std::cout<<"We get out here! (By chi2 Limit)"<<std::endl;
                    return outPut;}
                    

 
                std::vector<std::pair<int,int>> points;
                std::vector<std::pair <int ,std::pair <int,int>>> pointsTrackID;

    
                for(it2=it; it2!=sortedTracks.end(); it2++) //run over the new set of tracks starting from a new point
                {
                    if (int(chi2Points.size())==nTracks) break;
                    if (int(chi2Points.size())==0)
                    {
                        chi2+=(*it2).first;
                        chi2Points.push_back(*it2);
                        continue;
                      }
                     
                    bool addTrack =true; //this bool will only be true if any of the points pair fo the track is not
                    for (int i =0; i< int(chi2Points.size()); i++)
                        {
                        for(itPoint=(*it2).second.begin();itPoint!=(*it2).second.end();itPoint++)
                            {
                                
                            for (int j = 0; j< int(chi2Points[i].second.size());j++)
                                 {
                                     
                                    if( ((*itPoint).second.first == chi2Points[i].second[j].second.first )  && ((*itPoint).second.second == chi2Points[i].second[j].second.second) )
                                        { 
                                           addTrack = false;
                                           break;
                                        }

                                    if (!addTrack) break;
                                }
                            if (!addTrack) break;
                            }
                       } 
                    if (addTrack)
                        {
                         chi2+=(*it2).first;
                         chi2Points.push_back(*it2);
                            if (int(chi2Points.size())==nTracks)     break;
                          }
                }
                if ((bestChi2SUM == 0 || chi2 < bestChi2SUM) && int(chi2Points.size())==nTracks)
                {   if (int(chi2Points.size())==nTracks) bestChi2SUM = chi2;
                    if (int(chi2Points.size())==nTracks)    outPut = chi2Points;
                }
                Try += 1;
        } 
    if (int(outPut.size())!=nTracks)
        {
        std::cout<< "Error 0, Not enough tracks were reconstructed! "<< nTracks << " were expected."<<std::endl;
        return outPut;
         }
return outPut;
}








