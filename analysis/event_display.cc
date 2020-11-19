//Do
// C. Jesús Valls 
// cjesus@ifae.es
//
// M. Granado-González
// MMG998@student.bham.ac.uk
//

#include "CMOSPixel.hh"
#include "pCTEvent.hh"
#include "../utils/global_tools.cc"
#include "pCTXML.hh"
#include <iostream>

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

#include "pCTTrackingManager.cc"

bool GenerateNewRngToEnergyTable = true;
double straightness_cut = 0;

int main(int argc,char** argv){

    TApplication* theApp=new TApplication("App",&argc,argv);

    TString localStyleName = "pCTstyle";
    // -- WhichStyle --
    // 1 = presentation large fonts
    // 2 = presentation small fonts
    // 3 = publication/paper
    Int_t localWhichStyle = 2;
    TStyle* pCTstyle = SetpCTStyle(localWhichStyle, localStyleName);
    gROOT->SetStyle(localStyleName);

    bool    batch          = false;
    int     maxEvents      = std::numeric_limits<int>::max();;
    int     maxSelEvents   = std::numeric_limits<int>::max();;
    int     selEvents      = 0;
    float   start_time     = clock();
    bool    show_SciDet    = false;
    bool    show_CMOS      = false;
    bool    show_reco      = false;
    TString fileIn         = "/Users/cjesus/Dev/protonCT/output/simulation_file_3_0.05_CMOS.root";
    TString fileOut        = "/Users/cjesus/Dev/protonCT/output/erase_me.root";
    int evtIni             = 0;
    int evtFin             = 0;

    for (int iarg=0; iarg<gApplication->Argc(); iarg++){
        //cout << "iarg: " << iarg << "," << string( gApplication->Argv(iarg)) << endl;
        if (string( gApplication->Argv(iarg))=="-i" || string( gApplication->Argv(iarg))=="--input" ){
            iarg++;
            fileIn = gApplication->Argv(iarg);
            if (fileIn.Contains(".root")) {
                cout << "Input file: " << fileIn << endl;
            }
            else {
                cerr << "input file must be a .root file!" << endl;
                exit(1);
            }
        }
        else if (string( gApplication->Argv(iarg))=="-o" || string( gApplication->Argv(iarg))=="--output" ){
            iarg++;
            fileOut = gApplication->Argv(iarg);
            cout << "Input file: " << fileOut << endl;
        }
        else if (string( gApplication->Argv(iarg))=="-b" || string( gApplication->Argv(iarg))=="--batch" ){
            batch = true;
            gROOT->SetBatch(kTRUE);
        }
        else if (string( gApplication->Argv(iarg))=="-a" || string( gApplication->Argv(iarg))=="--evtIni" ){
            iarg++;
            evtIni = atoi(gApplication->Argv(iarg));
        }
        else if (string( gApplication->Argv(iarg))=="-z" || string( gApplication->Argv(iarg))=="--evtFin" ){
            iarg++;
            evtFin = atoi(gApplication->Argv(iarg));
        }
        else if (string( gApplication->Argv(iarg))=="-s"){
            iarg++;
            straightness_cut = atof(gApplication->Argv(iarg));
        }
        else if (string( gApplication->Argv(iarg))=="-showSciDet"){
            show_SciDet = true;
        }
        else if (string( gApplication->Argv(iarg))=="-showCMOS"){
            show_CMOS = true;
        }
        else if (string( gApplication->Argv(iarg))=="-showRECO"){
            show_reco = true;
        }
    }

    TFile* outFile = new TFile(fileOut.Data(),"RECREATE");
    TFile* inputFile = new TFile(fileIn.Data(),"read");

    if(!inputFile) {std::cout << "no output file! exit." << std::endl; exit(1);}
    
    TTree*   data       = (TTree*) inputFile->Get("pCT_Events");
    if(!data) {std::cout << "TTree not found." << std::endl; exit(1);}
    TBranch* dataBranch = data->GetBranch("Event");
    pCTEvent* event = new pCTEvent();
    dataBranch->SetAddress(&event);
    pCTXML* config = (pCTXML*) inputFile->Get("XMLinput");

    int nEvents = data->GetEntries();

    if(!evtFin) evtFin = nEvents;
    if(evtFin > nEvents) evtFin = nEvents;
    if(!nEvents){
        cerr << "Input file is empty!" << endl;
        exit(1);
    }
    if(evtIni < 0) {
        cerr << "evtIni can not be negative!" << endl;
        exit(1);
    }
    if(evtFin <= evtIni){
        cerr << "evtFin must be larger than event Ini!" << endl;
    }

    cout << "Tot # Events: " << data->GetEntries() << endl;
    for(int ievt(evtIni); ievt<evtFin; ievt++){
        if(ievt == maxEvents-1 or selEvents >= maxSelEvents) break;

        data->GetEntry(ievt);
        cout << "Event: " << ievt << endl;

        cout << "# RT hits: " << event->GetSciDetHits().size() << endl;

        if(show_SciDet) event->DrawSciDetHits(config);
        if(show_CMOS)   event->DrawCMOSHits(config);

        if(show_reco){
            pCTTrackingManager* trkMan = new pCTTrackingManager(event,config);
            trkMan->DoCMOSChi2Tracking();
            trkMan->DoRTTracking();
            auto recoTracks = trkMan->GetRecoTracks();
            if(recoTracks.size()==4){
                event->DrawSciDetHits(config);
                event->DrawRecoTracks(config,recoTracks);
                event->DrawRecoTracks3D(config,recoTracks);
            }
        }
    }

    inputFile->Close();
    outFile->Close();
    theApp->Run();
    return 0;
}



