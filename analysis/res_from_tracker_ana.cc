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
    TString fileIn         = "/Users/cjesus/Dev/protonCT/output/simulation_file.root";
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

    const int nbars(config->GetSciDetNBars());
    const int nlayers(config->GetSciDetNLayers());

    TF1* fitval = new TF1("fitval","pol3",0,nlayers*3);
    fitval->SetParameters(34.0868,3.20458,-0.0219488,8.48856e-05); 

    //_______THE RESULT HITOS_____
    int maxE = 250;
    int nResBinWidth = 10;
    TH2F* h_trueEvsRng = new TH2F("h_trueEvsRng","",50,0,nlayers*3,50,0,maxE);

    //_______THE ANALYSIS______


    cout << "Tot # Events: " << data->GetEntries() << endl;
    for(int ievt(evtIni); ievt<evtFin; ievt++){
        if(ievt == maxEvents-1 or selEvents >= maxSelEvents) break;

        data->GetEntry(ievt);
        if(ievt%10000==0)cout << "Event: " << ievt << endl;

        std::map <int, double > trackIdToGunEnergy = event->GetGunEnergyMap();
        pCTTrackingManager* trkMan = new pCTTrackingManager(event,config);
        trkMan->DoCMOSTracking();
        auto recoTracks = trkMan->DoRTTracking();

        for(int i(0); i<recoTracks.size(); i++){

            std::vector<TVector3> track3Dhits = recoTracks[i]->Get3DHits(); 
            double range = (track3Dhits.back()-(*track3Dhits.begin())).Mag();
            //cout << "range: " << range << endl;
            h_trueEvsRng->Fill(range,(*trackIdToGunEnergy.begin()).second);
        }

        if(show_SciDet) event->DrawSciDetHits(config);
        if(show_CMOS)   event->DrawCMOSHits(config);
    }

    if(GenerateNewRngToEnergyTable){
        for(int i(0); i<50; i++){
            int maxBin      = -1;
            int maxBinCnt   = -1;
            for(int j(0); j<50; j++)
                if(maxBinCnt < h_trueEvsRng->GetBinContent(i+1,j+1))
                    {maxBin = j; maxBinCnt = h_trueEvsRng->GetBinContent(i+1,j+1);}
            for(int j(0); j<50; j++)
                if(h_trueEvsRng->GetBinContent(i+1,j+1) < maxBinCnt) h_trueEvsRng->SetBinContent(i+1,j+1,0);
        }
        h_trueEvsRng->Fit(fitval,"RQ");
    }

    TCanvas* ccc = new TCanvas("ccc");
    ccc->cd();
    h_trueEvsRng->Draw("COLZ");
    fitval->Draw("same");
    ccc->Update();
    ccc->WaitPrimitive();

    int nResBins = maxE/nResBinWidth;
    TH1F* h_EResByRng [nResBins];
    for(int bin(0); bin<nResBins; bin++){
        TString hname = "h_EResByRng_";
        hname += bin;
        h_EResByRng[bin] = new TH1F(hname.Data(),hname.Data(),100,-50,50); // resolution in percentage.
    }

    for(int ievt(evtIni); ievt<evtFin; ievt++){
        if(ievt == maxEvents-1 or selEvents >= maxSelEvents) break;
        
        data->GetEntry(ievt);
        if(ievt%10000==0)cout << "Event: " << ievt << endl;

        std::map <int, double > trackIdToGunEnergy = event->GetGunEnergyMap();
        std::vector<TVector3> points;
        std::vector<TVector3> vecs;
        std::map<int, std::vector< CMOSPixel* > > Counter = event->GetPixelHitsMap();
        std::map<int, std::vector< CMOSPixel*> >::iterator it;
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
        if(! points.size()) continue;
        
        pCTTrackingManager* trkMan = new pCTTrackingManager(event,config);
        trkMan->DoCMOSTracking();
        auto recoTracks = trkMan->DoRTTracking();
        
        for(int i(0); i<recoTracks.size(); i++){
            if(recoTracks.size() != 1) continue;
            auto track3Dhits = recoTracks[i]->Get3DHits(); 
            double range = (track3Dhits.back()-(*track3Dhits.begin())).Mag();
            double trueEnergy      = (*trackIdToGunEnergy.begin()).second;
            double recoEnergyByRng = fitval->Eval(range);
            int RngResBin = (int) (*trackIdToGunEnergy.begin()).second/nResBinWidth;
            double RngRes = 100*(trueEnergy-recoEnergyByRng)/trueEnergy;
            h_EResByRng[RngResBin]->Fill(RngRes);
        }
    }

    TF1* fitRes = new TF1("fitRes","gaus",-20,20);
    fitRes->SetParameters(100,0,10); 
    TCanvas* c1 = new TCanvas("c1");
    c1->cd();
    TGraphErrors* g_EResByRng = new TGraphErrors();
    g_EResByRng->SetName("EResByRng");
    for(int bin(0); bin<nResBins; bin++){
        if(!h_EResByRng[bin]->GetEntries()) continue;
        fitRes->SetParameters(100,0,10); 
        h_EResByRng[bin]->Fit("fitRes","RQ");
        double fitmean  = fitRes->GetParameter(1);
        double fitsigma = fitRes->GetParameter(2);
        if (bin*nResBinWidth+nResBinWidth/2 < 40) continue;
        g_EResByRng->SetPoint(g_EResByRng->GetN(),bin*nResBinWidth+nResBinWidth/2,fitsigma);
        g_EResByRng->SetPointError(g_EResByRng->GetN()-1,0,fitRes->GetParError(2));
        c1->Update();
        c1->WaitPrimitive();
    }

    TCanvas* c2 = new TCanvas("c2");
    c2->cd();
    h_trueEvsRng->Draw("COLZ");
    fitval->Draw("same");
    c2->Update();

    TCanvas* c3 = new TCanvas("c3");
    c3->cd();
    g_EResByRng->SetMarkerStyle(22);
    g_EResByRng->SetMarkerColor(kBlue);
    g_EResByRng->Draw("AP");
    c3->Update();

    //_____________________

    outFile->cd();
    g_EResByRng->Write();
    outFile->Close();

    if(batch) return 0;

    inputFile->Close();
    theApp->Run();

    return 0;
}
