
#include "pCTTrackingManager.hh"
#include <TCanvas.h>

#include <TEveManager.h>
#include <TEvePlot3D.h>
#include <TEveTrans.h>
#include <TGLViewer.h>
#include <TF2.h>
#include <TH3.h>


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

    double min_dist = 10;
    for (int s(0); s<seedPoints.size(); ++s){

        // set true by default and make it false if the track building fails.
        isReco[s] = true;
        
        // steps
        // for each seed, compute a prediction in the first SciDet layer.
        double delta = 0; // in the future compute distance CMOS to first layer using config...

        TVector3 seed_pos = seedPoints[s];
        TVector3 seed_vec = seedVecs[s];

        TVector3 pred((seed_pos.X()-n_xpixels/2)*pitchX+seed_vec.X()*delta, (seed_pos.Y()-n_ypixels/2)*pitchY+seed_vec.Y()*delta, 0.5);
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
                //std::cout << "new_point : " << new_point.X() << "," << new_point.Y() << "," << new_point.Z() << std::endl;
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
            std::cout << "dist: " << dist << std::endl;
            if ((pred-(*candidate)).Mag() < min_dist){
                ++Naccepted;
                listOf3Dhits.push_back((*candidate));
                listOfBarIDs.push_back(candidates_barIDs[counter][0]);
                listOfBarIDs.push_back(candidates_barIDs[counter][1]);
                //std::cout << "ini bar ids: " << candidates_barIDs[counter][0] << ", " << candidates_barIDs[counter][1] << std::endl;
            }
            counter++;
        }

        //if the seed candidates are too close or if there is not accepted seed candidate, the track is not reconstructed.
        std::cout << "Naccepted: " << Naccepted << std::endl;
        if (Naccepted != 1){
            std::cout << "breaking!!" << std::endl;
            isReco[s] = false;
            continue;
        }

        // process iteratively layer after layer.
        // for each layer generate a list of candidates and evaluate how many are
        // closer than 'min_dist' to the previous layer 3D point for the track being built.
        for (int layerNum(2); layerNum<nlayers; layerNum++){
            std::vector<TVector3> layer_candidates;
            std::vector<int> layer_barIDs;
            for(std::vector< SciDetHit* >::iterator hit2d=listOfSciHits.begin(); hit2d!=listOfSciHits.end(); hit2d++){
                if ((*hit2d)->GetLayerID() != layerNum) continue;
                double pos[2] = {0};
                (*hit2d)->GetOrientation() ? pos[0] = (-(*hit2d)->GetBarID()+nbars/2)*barWidth : pos[1] = (-(*hit2d)->GetBarID()+nbars/2)*barWidth;
                (*hit2d)->GetOrientation() ? pos[1] = listOf3Dhits.back().Y() : pos[0] = listOf3Dhits.back().X();
                TVector3 step_candidate(pos[0],pos[1],(*hit2d)->GetLayerID()*barWidth);

                double dist = (listOf3Dhits.back()-step_candidate).Mag();
                std::cout << "dist: " << dist << std::endl;
                if(dist < min_dist){
                    layer_candidates.push_back(step_candidate);
                    layer_barIDs.push_back(((*hit2d)->GetLayerID()+1)*nbars+(*hit2d)->GetBarID());
                }
            }
            if     (layer_candidates.size()==1){
                listOf3Dhits.push_back(layer_candidates.back());
                std::cout << "adding bar ids: " << layer_barIDs.back() << std::endl;
                listOfBarIDs.push_back(layer_barIDs.back());
            }
            else if(layer_candidates.size() >1){
                // if there is more than one candidate the info can not be separated and the track is not reconstructed.
                isReco[s] = false;
                break;
            }
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










