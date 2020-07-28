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

bool GenerateNewRngToEnergyTable = true;

int main(int argc,char** argv){

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
    TString fileOut        = "/Users/cjesus/Dev/protonCT/output/analysis_out_3.root";
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
        else if (string( gApplication->Argv(iarg))=="-showSciDet"){
            show_SciDet = true;
        }
        else if (string( gApplication->Argv(iarg))=="-showCMOS"){
            show_CMOS = true;
        }
    }

    TApplication* theApp=new TApplication("App",&argc,argv);

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









    TF1* fitval = new TF1("fitval","pol3",0,120);
    fitval->SetParameters(34.0868,3.20458,-0.0219488,8.48856e-05); 




    //_______THE RESULT HITOS_____

    TH1F* h_recoE = new TH1F("h_recoE","h_recoE",100,0,400);
    TH1F* h_imageDist = new TH1F("h_imageDist","h_imageDist",100,80,240);

    const int    nplanes(3);
    const double pitchX(40);
    const double pitchY(36);
    const double nrows(448);
    const double ncols(224);

    TH2F* h_hitsMap     = new TH2F("image","image",100,0,nrows,100,0,ncols);
    TH2F* h_hitsMapNorm = new TH2F("imageNorm","imageNorm",100,0,nrows,100,0,ncols);
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

    int maxE         = 250;
    int nResBinWidth = 10;
    TH2F* h_trueEvsRng = new TH2F("h_trueEvsRng","",50,0,250,50,0,maxE);

    //_______THE ANALYSIS______

    const int nbars(config->GetSciDetNBars());
    const int nlayers(config->GetSciDetNLayers());

    double hitsMap [nlayers][nbars][2];
    memset( hitsMap, 0, nlayers*nbars*2*sizeof(int) );

    cout << "Tot # Events: " << data->GetEntries() << endl;
    cout << "COMPUTING TRUE ENERGY TO RECO RNG RELATION" << endl;
    for(int ievt(evtIni); ievt<evtFin; ievt++){
        if(ievt == maxEvents-1 or selEvents >= maxSelEvents) break;

        data->GetEntry(ievt);
        if(ievt%1000==0)cout << "Event: " << ievt << endl;

        std::map <int, double > trackIdToGunEnergy = event->GetGunEnergyMap();
        std::map <int, double >::iterator gunEit;

        //if((*trackIdToGunEnergy.begin()).second>80) continue;

        // cout << trackIdToGunEnergy.size() << endl;
        // for ( gunEit = trackIdToGunEnergy.begin(); gunEit != trackIdToGunEnergy.end(); gunEit++ )
        //     std::cout << (*gunEit).first << "," << (*gunEit).second << std::endl;

        std::vector< SciDetHit* > listOfSciHits = event->GetSciDetHits();
        std::vector< SciDetHit* >::iterator sciHit;

        hitsMap[(*sciHit)->GetLayerID()][(*sciHit)->GetBarID()][(*sciHit)->GetOrientation()] 
        = (*sciHit)->GetEnergyDeposited();

        std::vector< pCTTrack* > recoTracks = event->Reconstruct(config);
        if(recoTracks.size() == 1)
                h_trueEvsRng->Fill((*recoTracks.begin())->GetRecoMeas(1),(*trackIdToGunEnergy.begin()).second);

        //cout << (*trackIdToGunEnergy.begin()).second << "," << fitval->Eval((*trackIdToGunEnergy.begin()).second) << endl;

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
        h_trueEvsRng->Fit(fitval,"R");
    }

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
        if(ievt%1000==0)cout << "Event: " << ievt << endl;

        std::map <int, double > trackIdToGunEnergy = event->GetGunEnergyMap();
        std::map <int, double >::iterator gunEit;

        std::vector< SciDetHit* > listOfSciHits = event->GetSciDetHits();
        std::vector< SciDetHit* >::iterator sciHit;

        hitsMap[(*sciHit)->GetLayerID()][(*sciHit)->GetBarID()][(*sciHit)->GetOrientation()] 
        = (*sciHit)->GetEnergyDeposited();

        std::vector< pCTTrack* > recoTracks = event->Reconstruct(config);
        if(recoTracks.size() != 1) continue;

        double trueEnergy      = (*trackIdToGunEnergy.begin()).second;


//+0.5*(maxEdep[prelayerID]-maxEdep[prelayerID-1])/(maxEdep[prelayerID]+maxEdep[prelayerID-1])


        double recoEnergyByRng = fitval->Eval((*recoTracks.begin())->GetRecoMeas(1));
        int RngResBin = (int) (*trackIdToGunEnergy.begin()).second/nResBinWidth;
        double RngRes = 100*(trueEnergy-recoEnergyByRng)/trueEnergy;
        h_EResByRng[RngResBin]->Fill(RngRes);

        for (ushort p(0); p<nplanes; p++){
            if(!p) continue;
            std::map<G4int, std::vector< CMOSPixel* > > Counter = event->GetPixelHitsMap();
            std::map<G4int, std::vector< CMOSPixel*> >::iterator it;
            for(it=Counter.begin(); it!=Counter.end(); it++){
                ushort Plane = (*it).first;
                if (Plane != p) continue;
                ushort nHitsInPlane = (*it).second.size();
                if(nHitsInPlane>1) continue;
                for(ushort index(0); index<nHitsInPlane; index++)
                {
                    ushort X = (*it).second.at(index)->GetX();
                    ushort Y = (*it).second.at(index)->GetY();
                    ushort e = (*it).second.at(index)->GetElectronsLiberated();
                    h_hitsMap->Fill(X+1,Y+1,recoEnergyByRng);
                    h_recoE->Fill(recoEnergyByRng);
                    h_hitsMapNorm->Fill(X+1,Y+1,1);
                }
            }
        }


        //if(trueEnergy<80) cout << "Bin/True/Reco/RngRes: " << (*recoTracks.begin())->GetRecoMeas()+(*recoTracks.begin())->GetRngCorr() << "," << trueEnergy << "," << recoEnergyByRng << "," << RngRes << endl;
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
        cout << bin*nResBinWidth+nResBinWidth/2 << endl;
        g_EResByRng->SetPoint(g_EResByRng->GetN(),bin*nResBinWidth+nResBinWidth/2,fitsigma);
        g_EResByRng->SetPointError(g_EResByRng->GetN()-1,0,fitRes->GetParError(2));
        // c1->Update();
        // c1->WaitPrimitive();
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

    TCanvas* c4 = new TCanvas("c4");
    c4->Divide(1,5);
    c4->cd(1);
    h_hitsMap->DrawCopy("COLZ");
    c4->cd(2);
    h_hitsMapNorm->DrawCopy("COLZ");
    c4->cd(3);
    h_hitsMap->Divide(h_hitsMapNorm);
    h_hitsMap->GetZaxis()->SetRangeUser(120,160);
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
    h_imageDist->Draw("HIST");
    c5->Update();
    //c5->WaitPrimitive();

    //_____________________

    outFile->cd();
    g_EResByRng->Write();
    outFile->Close();

    inputFile->Close();
    theApp->Run();

    return 0;
}
