
#include <iostream>

#include "CMOSPixel.hh"
#include "TrackFitter.cc"
#include "pCTEvent.hh"
#include "pCTXML.hh"

#include "TROOT.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TBox.h"
#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"
#include "TString.h"
#include "TF1.h"
#include "TApplication.h"
#include "TGraphErrors.h"

#include "EvNavHandler.hh"

#include "pCTTrackingManager.cc"


#include <TSystem.h>
#include <TEveManager.h>
#include <TEveGeoShapeExtract.h>
#include <TEveGeoShape.h>
#include <TEveBrowser.h>
#include <TRootBrowser.h>
#include <TGButton.h>
#include <TGTab.h>
#include <TGFrame.h>

#include "TQObject.h"
#include "TQConnection.h"

#include <TEveElement.h>
#include <TEvePlot3D.h>
#include <TEveArrow.h>
#include <TGeoManager.h>
#include <TEveGeoNode.h>
#include <TGeoNode.h>
#include <TEveTrans.h>
#include <TGLViewer.h>
#include <TF2.h>
#include <TF3.h>
#include <TH3.h>

TTree*   data;
pCTEvent* event;
TGeoNode* SciDet;
pCTXML* config;
int event_id = 0;

//___________________________________________________________________________
inline void make_gui()
{
   // Create minimal GUI for event navigation.
   TEveBrowser* browser = gEve->GetBrowser();
   browser->StartEmbedding(TRootBrowser::kLeft);
   TGMainFrame* frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
   frmMain->SetWindowName("XX GUI");
   frmMain->SetCleanup(kDeepCleanup);
   TGHorizontalFrame* hf = new TGHorizontalFrame(frmMain);
   {
        TString icondir( Form("%s/icons/", gSystem->Getenv("ROOTSYS")) );
        TGPictureButton* b = 0;
        EvNavHandler    *fh = new EvNavHandler(data,&event_id,event,config,SciDet);
        b = new TGPictureButton(hf, gClient->GetPicture(icondir+"GoBack.gif"));
        hf->AddFrame(b);
        b->Connect("Clicked()", "EvNavHandler", fh, "Bck()");
        b = new TGPictureButton(hf, gClient->GetPicture(icondir+"GoForward.gif"));
        hf->AddFrame(b);
        b->Connect("Clicked()", "EvNavHandler", fh, "Fwd()");
        fh->Fwd();
   }
   frmMain->AddFrame(hf);
   frmMain->MapSubwindows();
   frmMain->Resize();
   frmMain->MapWindow();
   browser->StopEmbedding();
   browser->SetTabTitle("Event Control", 0);
}

//___________________________________________________________________________
inline void make_display(pCTXML* cfg)
{
    TEveManager::Create();
    TFile::SetCacheFileDir(".");
    auto gGeoManager = gEve->GetGeometry("geometry.root");
    gGeoManager->DefaultColors();

    TGLViewer *v = gEve->GetDefaultGLViewer();
    v->ColorSet().Background().SetColor(kMagenta+4);
    
    TGeoNode* topNode = gGeoManager->GetTopVolume()->FindNode("Envelope_0");

    int nCMOS = 4;
    if (!cfg->GetUse4thCMOS()) nCMOS = 3;

    TGeoNode* CMOS[nCMOS];
    TGeoNode* CMOSepi[nCMOS];
    TGeoNode* CMOSsub[nCMOS];
    for (int it(0); it<nCMOS; ++it){
        CMOS[it] = topNode->GetDaughter(it);
        CMOSepi[it] = CMOS[it]->GetDaughter(0);
        CMOSsub[it] = CMOS[it]->GetDaughter(1);
    }
    SciDet = topNode->GetDaughter(nCMOS);

    int nbars(cfg->GetSciDetNBars());
    int nlayers(cfg->GetSciDetNLayers());

    TGeoNode* SciDetbar[nbars*nlayers*2];
    for (int it(0); it<nbars*nlayers; ++it){
        SciDetbar[it] = SciDet->GetDaughter(it);
        TEveGeoNode* tmpEve = new TEveGeoNode(SciDetbar[it]);
        // tmpEve->SetMainColor(3);
        //tmpEve->SetMainTransparency (100);
        tmpEve->SetRnrSelf(0);
        if((it%20)==0){
            tmpEve->SetRnrSelf(1);
            tmpEve->SetMainColor(2);
        }
        tmpEve->SetMainTransparency (10);
        // else{
        //     tmpEve->SetMainColor(3);
        //     tmpEve->SetMainTransparency (100);       
        // }
    }

    TEveGeoTopNode* tmpEve = new TEveGeoTopNode(gGeoManager, SciDet);
    // tmpEve->SetMainColor(2);
    // tmpEve->SetMainTransparency (50);
    gEve->AddElement(tmpEve);


    // TEveGeoTopNode* tmpEve = new TEveGeoTopNode(gGeoManager, SciDet);
    // tmpEve->SetMainColor(2);
    // tmpEve->SetMainTransparency (20);
    // gEve->AddGlobalElement(tmpEve);

    // TGeoNode* node2 = node1-> GetDaughter(4);
    // TGeoNode* node3 = node2-> GetDaughter(1);
    


    // node3->GetMotherVolume()->SetVisibility(0);
    // node3->InspectNode ();
    // TEveGeoTopNode* inn = new TEveGeoTopNode(gGeoManager, node3);
    // inn->SetMainColor(1);
    // inn->SetMainTransparency (50);
    // gEve->AddGlobalElement(inn);

    // TEveArrow* a1 = new TEveArrow(0., 0., 100., 0., 0., 0.);
    // a1->SetMainColor(kBlue);
    // a1->SetTubeR(0.1);
    // a1->SetPickable(kTRUE);
    // gEve->AddElement(a1);

    gEve->GetBrowser()->GetTabRight()->SetTab(1);
    gEve->GetBrowser()->GetTabRight()->SetTab(2);
    make_gui();
}

int main(int argc,char** argv){

    TApplication* theApp=new TApplication("App",&argc,argv);

    std::cout << "Starting Analysis Example..." << std::endl;

    TString inputFileName = "../output/simulation_file.root";
    TFile* inputFile = new TFile(inputFileName.Data(),"update");
    if(!inputFile) {std::cout << "no output file! exit." << std::endl; exit(1);}
    
    data       = (TTree*) inputFile->Get("pCT_Events");
    if(!data) {std::cout << "TTree not found." << std::endl; exit(1);}
    TBranch* dataBranch = data->GetBranch("Event");
    event = new pCTEvent();
    dataBranch->SetAddress(&event);

    config = (pCTXML*) inputFile->Get("XMLinput");

    double Z_pos[4] = {0.,5.,10.,15.};

    /*
    for(int ievt(0); ievt<data->GetEntries(); ievt++){
            data->GetEntry(ievt);
    
    //     std::vector<TVector3> points;
    //     std::vector<TVector3> vecs;
    
    //     std::map<G4int, std::vector< CMOSPixel* > > Counter = event->GetPixelHitsMap();
    //     std::map<G4int, std::vector< CMOSPixel*> >::iterator it;
    //     for(it=Counter.begin(); it!=Counter.end(); it++){
    //         ushort Plane = (*it).first;
    //         if (Plane != 3) continue;
    //         ushort nHitsInPlane = (*it).second.size();
    //         for(ushort index(0); index<nHitsInPlane; index++)
    //         {
    //             points.push_back(TVector3((*it).second.at(index)->GetX(),(*it).second.at(index)->GetY(),0));
    //             vecs.push_back(TVector3(0,0,0));
    //         }
    //     }
    
    //     std::cout << "#seeds: " << points.size() << std::endl;
    //     if(points.size())
    //         std::cout <<  points.back().X() << "," << points.back().Y() << "," << points.back().Z() << std::endl;
    //     pCTTrackingManager* trkMan = new pCTTrackingManager(event,config,points,vecs);
    //     trkMan->DoTracking();
    //     delete trkMan;


         event->DrawSciDetHits(config);
    //     event->DrawCMOSHits(config);
    }
    */
    make_display(config);

    theApp->Run();

    return 0;
}


/*
    // TVector3 CMOS_pos  (config->GetPosX(), config->GetPosY(), config->GetPosZ3());
    // if(!config->GetUse4thCMOS()) CMOS_pos.SetZ(config->GetPosZ2());

    // TVector3 SciDet_pos(0,0, CMOS_pos.Z()+0.5);

    // std::cout << "CMOS_pos: " << CMOS_pos.X() << "," << CMOS_pos.Y() << "," << CMOS_pos.Z() << std::endl;
    // std::cout << "SciDet_pos: " << SciDet_pos.X() << "," << SciDet_pos.Y() << "," << SciDet_pos.Z() << std::endl;

    for(int ievt(0); ievt<data->GetEntries(); ievt++){
            data->GetEntry(ievt);
    
        std::vector<TVector3> points;
        std::vector<TVector3> vecs;
    
        std::map<G4int, std::vector< CMOSPixel* > > Counter = event->GetPixelHitsMap();
        std::map<G4int, std::vector< CMOSPixel*> >::iterator it;
        for(it=Counter.begin(); it!=Counter.end(); it++){
            ushort Plane = (*it).first;
            if (Plane != 3) continue;
            ushort nHitsInPlane = (*it).second.size();
            for(ushort index(0); index<nHitsInPlane; index++)
            {
                points.push_back(TVector3((*it).second.at(index)->GetX(),(*it).second.at(index)->GetY(),0));
                vecs.push_back(TVector3(0,0,0));
            }
        }
    
        std::cout << "#seeds: " << points.size() << std::endl;
        if(points.size())
            std::cout <<  points.back().X() << "," << points.back().Y() << "," << points.back().Z() << std::endl;
        pCTTrackingManager* trkMan = new pCTTrackingManager(event,config,points,vecs);
        trkMan->DoTracking();
        delete trkMan;


        event->DrawSciDetHits(config);
        event->DrawCMOSHits(config);
    }
    
    TEveManager::Create();
    // { // Simple geometry
    // //TFile::SetCacheFileDir(".");
    //    TFile* geom = TFile::Open("geometry.root", "READ");
    //    if (!geom)
    //       return 0;
    //    TEveGeoShapeExtract* gse = (TEveGeoShapeExtract*) geom->Get("ND280Geometry");
    //    auto gGeomGentle = TEveGeoShape::ImportShapeExtract(gse, 0);
    //    geom->Close();
    //    delete geom;
    //    gEve->AddGlobalElement(gGeomGentle);
    // }

   TFile::SetCacheFileDir(".");
   auto gGeoManager = gEve->GetGeometry("geometry.root");
   gGeoManager->DefaultColors();
   TGeoNode* node1 = gGeoManager->GetTopVolume()->FindNode("Envelope_0");
   TGeoNode* node2 = node1-> GetDaughter(4);
   TGeoNode* node3 = node2-> GetDaughter(1);
   node3->GetMotherVolume()->SetVisibility(0);
   node3->InspectNode ();
   TEveGeoTopNode* inn = new TEveGeoTopNode(gGeoManager, node3);
   std::cout << "transparency: " << inn->GetMainTransparency() << std::endl;
   inn->SetMainColor(1);
   inn->SetMainTransparency (50);
   gEve->AddGlobalElement(inn);

    TEveArrow* a1 = new TEveArrow(1., 1., 100., 10., 4., 0.);
    a1->SetMainColor(kBlue);
    a1->SetTubeR(0.2);
    a1->SetPickable(kTRUE);
    gEve->AddElement(a1);

    // // Standard multi-view
    // //=====================
    // gMultiView = new MultiView;
    // gMultiView->ImportGeomRPhi(gGeomGentle);
    // gMultiView->ImportGeomRhoZ(gGeomGentle);
    // // HTML summary view
    // //===================
    // fgHtmlSummary = new HtmlSummary("Alice Event Display Summary Table");
    // slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
    // fgHtml = new TGHtml(0, 100, 100);
    // TEveWindowFrame *wf = slot->MakeFrame(fgHtml);
    // fgHtml->MapSubwindows();
    // wf->SetElementName("Summary");
    // Final stuff
    //=============
    gEve->GetBrowser()->GetTabRight()->SetTab(1);
    make_gui();

   // gEve->GetDefaultGLViewer()->SetCurrentCamera(TGLViewer::kCameraPerspXOY);

   // TF2 *f2 = new TF2("f2","x**2 + y**2 - x**3 -8*x*y**4", -1., 1.2, -1.5, 1.5);
   // f2->SetFillColor(45);
   // auto x = new TEvePlot3D("EvePlot - TF2");
   // x->SetLogZ(kTRUE);
   // x->SetPlot(f2,"glsurf4");
   // x->RefMainTrans().MoveLF(2, 1);
   // gEve->AddElement(x);

   // TF3* forml = new TF3("f3", "sin(x)*sin(y)*sin(z)/(x*y*z)");

   // TH3F *h31 = new TH3F("h31", "h31", 10, -1, 1, 10, -1, 1, 10, -1, 1);
   // h31->FillRandom("f3",400);
   // h31->SetFillColor(2);
   // x = new TEvePlot3D("EvePlot - TH3F");
   // x->SetPlot(h31, "glbox");
   // x->RefMainTrans().MoveLF(2, -1);
   // gEve->AddElement(x);

    //load_event();
    gEve->Redraw3D(kTRUE);
    gEve->FullRedraw3D(kTRUE);

    //for(int ievt(0); ievt<data->GetEntries(); ievt++){
        //data->GetEntry(ievt);

        // std::vector <TVector3> pointsToFit;

        // for (ushort p(0); p<4; p++){
        //     std::map<G4int, std::vector< CMOSPixel* > > Counter = event->GetPixelHitsMap();
        //     std::map<G4int, std::vector< CMOSPixel*> >::iterator it;
        //     for(it=Counter.begin(); it!=Counter.end(); it++){
        //         ushort Plane = (*it).first;
        //         if (Plane != p) continue;
        //         ushort nHitsInPlane = (*it).second.size();
        //         for(ushort index(0); index<nHitsInPlane; index++)
        //         {
        //             ushort X = (*it).second.at(index)->GetX();
        //             ushort Y = (*it).second.at(index)->GetY();
        //             pointsToFit.push_back(TVector3(X,Y,Z_pos[p]));
        //             std::cout << pointsToFit[pointsToFit.size()-1].X() << "," << pointsToFit[pointsToFit.size()-1].Y() << "," << pointsToFit[pointsToFit.size()-1].Z() << std::endl;
        //         }
        //     }
        // }

        // std::cout << pointsToFit.size() << std::endl;
        // if(pointsToFit.size() !=4) continue;
        // event->DrawCMOSHits(config);
        // // event->DrawSciDetHits(config);
        // TrackFitter(pointsToFit,1);  

        // pCTTrackingManager* trackMan = new pCTTrackingManager();
        // trackMan->DrawGeometry();      
    
       // TEveManager::Create();
       // gEve->GetDefaultGLViewer()->SetCurrentCamera(TGLViewer::kCameraPerspXOY);
    
       // TF2 *f2 = new TF2("f2","x**2 + y**2 - x**3 -8*x*y**4", -1., 1.2, -1.5, 1.5);
       // f2->SetFillColor(45);
       // auto x = new TEvePlot3D("EvePlot - TF2");
       // x->SetLogZ(kTRUE);
       // x->SetPlot(f2,"glsurf4");
       // x->RefMainTrans().MoveLF(2, 1);
       // gEve->AddElement(x);
    
       // TF3* forml = new TF3("f3", "sin(x)*sin(y)*sin(z)/(x*y*z)");
    
       // TH3F *h31 = new TH3F("h31", "h31", 10, -1, 1, 10, -1, 1, 10, -1, 1);
       // h31->FillRandom("f3",400);
       // h31->SetFillColor(2);
       // x = new TEvePlot3D("EvePlot - TH3F");
       // x->SetPlot(h31, "glbox");
       // x->RefMainTrans().MoveLF(2, -1);
       // gEve->AddElement(x);
    
       // gEve->Redraw3D(kTRUE);

       // gEve->WaitPrimitive();

    //}
    //_____________________

    // TCanvas* c1 = new TCanvas("c1");
    // c1->cd();

    // c1->Update();
    // c1->WaitPrimitive();

    // inputFile->Close();

    theApp->Run();

    return 0;
}
*/
