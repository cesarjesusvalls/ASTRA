
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

#include "pCTEventHandler.hh"

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
TGeoNode* Astra;
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
        pCTEventHandler    *fh = new pCTEventHandler(data,&event_id,event,config,Astra);
        b = new TGPictureButton(hf, gClient->GetPicture(icondir+"GoBack.gif"));
        hf->AddFrame(b);
        b->Connect("Clicked()", "pCTEventHandler", fh, "Bck()");
        b = new TGPictureButton(hf, gClient->GetPicture(icondir+"GoForward.gif"));
        hf->AddFrame(b);
        b->Connect("Clicked()", "pCTEventHandler", fh, "Fwd()");
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
    if (cfg->GetUsePhantom()) nCMOS ++;

    TGeoNode* CMOS[nCMOS];
    TGeoNode* CMOSepi[nCMOS];
    TGeoNode* CMOSsub[nCMOS];
    for (int it(0); it<nCMOS; ++it){
        CMOS[it] = topNode->GetDaughter(it);
        CMOSepi[it] = CMOS[it]->GetDaughter(0);
        CMOSsub[it] = CMOS[it]->GetDaughter(1);
    }
    Astra = topNode->GetDaughter(nCMOS);

    int nbars(cfg->GetAstraNBars());
    int nlayers(cfg->GetAstraNLayers());

    TEveGeoTopNode* tmpEve = new TEveGeoTopNode(gGeoManager, Astra);
    gEve->AddElement(tmpEve);
    gEve->AddElement(new TEveGeoTopNode(gGeoManager, Astra));

    gEve->GetBrowser()->GetTabRight()->SetTab(1);
    gEve->GetBrowser()->GetTabRight()->SetTab(2);
    make_gui();
}

int main(int argc,char** argv){

    TApplication* theApp=new TApplication("App",&argc,argv);

    std::cout << "Starting Analysis Example..." << std::endl;

    TString inputFileName = "../output/simulation_file_3mm_4protons_geant4.root";
    TFile* inputFile = new TFile(inputFileName.Data(),"update");
    if(!inputFile) {std::cout << "no output file! exit." << std::endl; exit(1);}
    
    data       = (TTree*) inputFile->Get("pCT_Events");
    if(!data) {std::cout << "TTree not found." << std::endl; exit(1);}
    TBranch* dataBranch = data->GetBranch("Event");
    event = new pCTEvent();
    dataBranch->SetAddress(&event);

    config = (pCTXML*) inputFile->Get("XMLinput");

    make_display(config);

    theApp->Run();

    return 0;
}
