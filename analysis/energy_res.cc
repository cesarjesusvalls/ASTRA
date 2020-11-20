//
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
#include "pCTTrackingManager.hh"
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

int total_reco = 0;

bool GenerateNewRngToEnergyTable = true;
int  observable = 0;

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
    TString fileOut        = "/Users/cjesus/Dev/protonCT/output/analysis_outfile.root";
    int evtIni             = 0;
    int evtFin             = 0;

    for (int iarg=0; iarg<gApplication->Argc(); iarg++){
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
        else if (string( gApplication->Argv(iarg))=="-m"){
            iarg++;
            observable = atoi(gApplication->Argv(iarg));
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
        else if (string( gApplication->Argv(iarg))=="-UsePreFitInfo"){
            GenerateNewRngToEnergyTable = false;
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

    //_______THE RESULT HITOS_____

    TH1F* h_recoE           = new TH1F("h_recoE","h_recoE",20,150,185);
    TH1F* h_imageDist       = new TH1F("h_imageDist","h_imageDist",100,80,250);
    TH2F* h_StraightnessVsE = new TH2F("h_StraightnessVsE","h_StraightnessVsE",100,0,250,100,0,1);

    int    eff_cnt [10] = {0};
    double eff_bin [10] = {1.,0.99,0.98,0.96,0.94,0.92,0.9,0.85,0.8,0.7};

    const int    nplanes(4);
    const double pitchX(config->GetPixelX());
    const double pitchY(config->GetPixelY());
    const double nrows(config->GetPlaneRows());
    const double ncols(config->GetPlaneColumns());

    int nbins_imag = 150;
    int min_pos = 30;
    TH2F* h_hitsMap     = new TH2F("image","image",nbins_imag,-min_pos,min_pos,nbins_imag,-min_pos,min_pos);
    TH2F* h_hitsMapNorm = new TH2F("imageNorm","imageNorm",nbins_imag,-min_pos,min_pos,nbins_imag,-min_pos,min_pos);
    TString xLabel = "# Pixel (";
    xLabel += pitchX;
    xLabel += "#mum/Pixel)";
    TString yLabel = "# Pixel (";
    yLabel += pitchY;
    yLabel += "#mum/Pixel)";
    h_hitsMap->GetXaxis()->SetTitle(xLabel.Data());
    h_hitsMap->GetYaxis()->SetTitle(yLabel.Data());
    h_hitsMap->GetXaxis()->SetTickLength(0.);
    h_hitsMap->GetYaxis()->SetTickLength(0.);
    h_hitsMap->GetYaxis()->SetTitleOffset(1.6);

    int maxE         = 240;
    int nResBinWidth = 10;
    TH2F* h_trueEvsRng;

    const int nbars(config->GetSciDetNBars());
    const int nlayers(config->GetSciDetNLayers());

    h_trueEvsRng = new TH2F("h_trueEvsRng","",50,0,nlayers+10,50,0,maxE);

    TF1* fitval = new TF1("fitval","[0]+[1]*x+[2]*sqrt([3]*x)",0,nlayers+10);
    //fitval->SetParameters(13.5133,0.241717,4.7242,3.0556); 
    fitval->SetParameters(13.5237,0.241549,4.78621,2.97965); 

    //_______THE ANALYSIS______

    double hitsMap [nlayers][nbars][2];
    memset( hitsMap, 0, nlayers*nbars*2*sizeof(int) );

    cout << "Tot # Events: " << data->GetEntries() << endl;
    for(int ievt(evtIni); ievt<evtFin; ievt++){
        if(!GenerateNewRngToEnergyTable) continue;
        if(ievt == maxEvents-1 or selEvents >= maxSelEvents) break;

        data->GetEntry(ievt);
        if(ievt%10000==0)cout << "Event: " << ievt << endl;

        std::map <int, double > trackIdToGunEnergy = event->GetGunEnergyMap();
        std::map <int, double >::iterator gunEit;

        std::vector< SciDetHit* > listOfSciHits = event->GetSciDetHits();
        std::vector< SciDetHit* >::iterator sciHit;

        hitsMap[(*sciHit)->GetLayerID()][(*sciHit)->GetBarID()][(*sciHit)->GetOrientation()] 
        = (*sciHit)->GetEnergyDeposited();

        if(!config->GetUseCMOS()){
            std::vector< pCTTrack* > recoTracks = event->Reconstruct(config);
            if(recoTracks.size() == 1){
                h_trueEvsRng->Fill((*recoTracks.begin())->GetRecoMeas(2),(*trackIdToGunEnergy.begin()).second);
            }
        }
        else{
            pCTTrackingManager* trkMan = new pCTTrackingManager(event,config);
            trkMan->DoCMOSChi2Tracking();
            trkMan->DoRTTracking();
            auto recoTracks = trkMan->GetRecoTracks();
            total_reco+=recoTracks.size();
            for(int i(0); i<recoTracks.size(); i++){
                std::vector<TVector3> track3Dhits = recoTracks[i]->Get3DHits(); 
                if(track3Dhits.size()){
                    double trueEnergy = recoTracks[i]->GetTrueEnergy();
                    double range = (track3Dhits.back()-(*track3Dhits.begin())).Mag();
                    h_trueEvsRng->Fill(range,trueEnergy);
                }
            }
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
                if(h_trueEvsRng->GetBinContent(i+1,j+1) < maxBinCnt or maxBinCnt <10) h_trueEvsRng->SetBinContent(i+1,j+1,0);
        }
        h_trueEvsRng->Fit(fitval,"R");
    }

    int nResBins = maxE/nResBinWidth;
    TH1F* h_EResByRng [nResBins];
    for(int bin(0); bin<nResBins; bin++){
        TString hname = "h_EResByRng_";
        hname += bin;
        h_EResByRng[bin] = new TH1F(hname.Data(),hname.Data(),200,-50,50); // resolution in percentage.
    }

    for(int ievt(evtIni); ievt<evtFin; ievt++){
        if(ievt == maxEvents-1 or selEvents >= maxSelEvents) break;
        
        data->GetEntry(ievt);
        if(ievt%10000==0)cout << "Event: " << ievt << endl;

        std::map <int, double > trackIdToGunEnergy = event->GetGunEnergyMap();
        std::map <int, double >::iterator gunEit;

        std::vector< SciDetHit* > listOfSciHits = event->GetSciDetHits();
        std::vector< SciDetHit* >::iterator sciHit;

        hitsMap[(*sciHit)->GetLayerID()][(*sciHit)->GetBarID()][(*sciHit)->GetOrientation()] 
        = (*sciHit)->GetEnergyDeposited();

        if(!config->GetUseCMOS()){
            std::vector< pCTTrack* > recoTracks = event->Reconstruct(config);
            if(recoTracks.size() == 1){
                double trueEnergy = (*trackIdToGunEnergy.begin()).second;
                double range      = (*recoTracks.begin())->GetRecoMeas(2);
                double accumRng   = (*recoTracks.begin())->GetRecoMeas(3);
                double straightness = accumRng == 0 ? 1 : range/accumRng;
                h_StraightnessVsE->Fill(trueEnergy,straightness);
                for (int it(0); it<10; it++) if(straightness >= eff_bin[it]) eff_cnt[it]++;
                double recoEnergyByRng = fitval->Eval(range);
                int RngResBin = (int) trueEnergy/nResBinWidth;
                double RngRes = 100*(trueEnergy-recoEnergyByRng)/trueEnergy;
                h_StraightnessVsE->Fill(trueEnergy,straightness);
                for (int it(0); it<10; it++) if(straightness >= eff_bin[it]) eff_cnt[it]++;
                if(straightness < straightness_cut) continue;
                h_EResByRng[RngResBin]->Fill(RngRes);
            }
        }
        else{
            pCTTrackingManager* trkMan = new pCTTrackingManager(event,config);
            trkMan->DoCMOSChi2Tracking();
            trkMan->DoRTTracking();
            trkMan->phantomPositions();
            auto recoTracks = trkMan->GetRecoTracks();

            for(int i(0); i<recoTracks.size(); i++){
                double trueEnergy = recoTracks[i]->GetTrueEnergy();
                std::vector<TVector3> track3Dhits = recoTracks[i]->Get3DHits(); 
                if(track3Dhits.size()){
                    double range = (track3Dhits.back()-(*track3Dhits.begin())).Mag();
                    double recoEnergyByRng = fitval->Eval(range);
                    int RngResBin = (int) trueEnergy/nResBinWidth;
                    double RngRes = 100*(trueEnergy-recoEnergyByRng)/trueEnergy;
                    double accumRng = 0;
                    for(int j(0); j<track3Dhits.size(); j++){
                        if (j>0) accumRng += (track3Dhits[j]-track3Dhits[j-1]).Mag();
                    }
                    double straightness = accumRng == 0 ? 1 : range/accumRng;
                    h_StraightnessVsE->Fill(trueEnergy,straightness);
                    for (int it(0); it<10; it++) if(straightness >= eff_bin[it]) eff_cnt[it]++;
                    if(straightness < straightness_cut) continue;
                    h_EResByRng[RngResBin]->Fill(RngRes);

                    if (recoEnergyByRng>135){
                        h_hitsMap->Fill(recoTracks[i]->GetPhantomProjX(),recoTracks[i]->GetPhantomProjY(),recoEnergyByRng);
                        h_recoE->Fill(recoEnergyByRng);
                        h_hitsMapNorm->Fill(recoTracks[i]->GetPhantomProjX(),recoTracks[i]->GetPhantomProjY(),1);
                    }
                }
            }
        }
    }

    TGraphErrors* g_EffVsStraigness = new TGraphErrors();
    g_EffVsStraigness->SetName("g_EffVsStraigness");
    for (int it(0); it<10; it++){
        g_EffVsStraigness->SetPoint(g_EffVsStraigness->GetN(),eff_bin[it],1.*eff_cnt[it]/h_StraightnessVsE->GetEntries());
    }

    cout << "TOTAL RECO:  " << total_reco << endl;

    TF1* fitRes = new TF1("fitRes","gaus",-80,80);
    fitRes->SetParameters(100,0,30); 
    TCanvas* c1 = new TCanvas("c1");
    c1->cd();
    TGraphErrors* g_EResByRng = new TGraphErrors();
    g_EResByRng->SetName("EResByRng");
    for(int bin(0); bin<nResBins; bin++){
        if (bin*nResBinWidth+nResBinWidth/2 > 100) fitRes = new TF1("fitRes","gaus",-10,10);
        if(!h_EResByRng[bin]->GetEntries()) continue;
        fitRes->SetParameters(100,0,10); 
        h_EResByRng[bin]->Fit("fitRes","RQ");
        double fitmean  = fitRes->GetParameter(1);
        double fitsigma = fitRes->GetParameter(2);
        if (bin*nResBinWidth+nResBinWidth/2 < 40 and fitsigma<3.5) continue;
        if( bin*nResBinWidth+nResBinWidth/2 < 50 and nlayers < 120) continue;
        if( bin*nResBinWidth+nResBinWidth/2 < 60 and nlayers < 50) continue;
        g_EResByRng->SetPoint(g_EResByRng->GetN(),bin*nResBinWidth+nResBinWidth/2,fitsigma);
        g_EResByRng->SetPointError(g_EResByRng->GetN()-1,0,fitRes->GetParError(2));
        c1->Update();
        c1->WaitPrimitive();
    }

    TCanvas* c2 = new TCanvas("c2");
    c2->cd();
    h_trueEvsRng->DrawCopy("COLZ");
    fitval->Draw("same");
    c2->Update();

    TCanvas* c3 = new TCanvas("c3");
    c3->cd();
    g_EResByRng->SetMarkerStyle(22);
    g_EResByRng->SetMarkerColor(kBlue);
    g_EResByRng->Draw("AP");
    c3->Update();

    TCanvas* c6 = new TCanvas("c6");
    c6->cd();
    //gStyle->SetOptStat(0);
    gPad->SetLeftMargin(1.6);
    h_StraightnessVsE->GetXaxis()->SetTitle("Proton True Kinetic Energy [MeV]");
    h_StraightnessVsE->GetYaxis()->SetTitle("Straightness");
    h_StraightnessVsE->DrawCopy("COLZ");
    c6->Update();

    TCanvas* c7 = new TCanvas("c7");
    c7->cd();
    g_EffVsStraigness->Draw("AP");
    c7->Update();

    TCanvas* c4 = new TCanvas("c4");
    c4->Divide(1,5);
    c4->cd(1);
    h_hitsMap->DrawCopy("COLZ");
    c4->cd(2);
    h_hitsMapNorm->DrawCopy("COLZ");
    c4->cd(3);
    h_hitsMap->Divide(h_hitsMapNorm);
    h_hitsMap->GetZaxis()->SetRangeUser(140,190);
    h_hitsMap->DrawCopy("COLZ");
    c4->cd(4);
    h_hitsMap->DrawCopy("CONTZ");
    c4->cd(5);
    h_hitsMap->DrawCopy("CONT1Z");
    c4->Update();

    for(int i(0); i<100; i++)
        for(int j(0); j<100; j++)
            h_imageDist->Fill(h_hitsMap->GetBinContent(i+1,j+1));

    TCanvas* c5 = new TCanvas("c5");
    c5->cd();
    //h_imageDist->Draw("HIST");
    h_recoE->Draw("HIST");
    c5->Update();

    TCanvas* c8 = new TCanvas("c8");
    c8->cd();
    h_hitsMap->GetZaxis()->SetRangeUser(150,180);
    h_hitsMap->DrawCopy("COLZ");
    c8->Update();
    //c8->WaitPrimitive();

    //_____________________

    outFile->cd();
    h_StraightnessVsE->Write();
    g_EffVsStraigness->Write();
    h_recoE->Write();
    h_imageDist->Write();
    h_trueEvsRng->Write();
    g_EResByRng->Write();
    h_hitsMap->Write();
    outFile->Close();

    if(batch) return 0;

    inputFile->Close();
    theApp->Run();

    return 0;
}
