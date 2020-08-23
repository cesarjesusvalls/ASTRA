
#include "pCTTrackingManager.hh"
#include <TCanvas.h>

#include <TEveManager.h>
#include <TEvePlot3D.h>
#include <TEveTrans.h>
#include <TGLViewer.h>
#include <TF2.h>
#include <TH3.h>


pCTTrackingManager::pCTTrackingManager(pCTEvent* evt, pCTXML* cnf)
	: event(evt), config(cnf) {Init(); return;}

pCTTrackingManager::pCTTrackingManager(pCTEvent* evt, pCTXML* cnf, int ntrks, std::map<int, TVector3> pnts, std::map<int, TVector3> vecs)
	: event(evt), config(cnf), NCMOSTracks(ntrks), seedPoints(pnts), seedVecs(vecs) {Init(); return;}

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
