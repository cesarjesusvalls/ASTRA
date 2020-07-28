
#include "CMOSPixel.hh"
#include "pCTEvent.hh"
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

int main(int argc,char** argv){

    TApplication* theApp=new TApplication("App",&argc,argv);

    std::cout << "Starting Analysis Example..." << std::endl;

    TString inputFileName = "/Users/cjesus/Dev/protonCT/output/piposi_test__10_0.root";
    TFile* inputFile = new TFile(inputFileName.Data(),"update");
    if(!inputFile) {std::cout << "no output file! exit." << std::endl; exit(1);}
    
    TTree*   data       = (TTree*) inputFile->Get("pCT_Events");
    if(!data) {std::cout << "TTree not found." << std::endl; exit(1);}
    TBranch* dataBranch = data->GetBranch("Event");
    pCTEvent* event = new pCTEvent();
    dataBranch->SetAddress(&event);

    pCTXML* config = (pCTXML*) inputFile->Get("XMLinput");
    std::cout << config->GetSciDetNBars()  <<"-------------------------------\n";

    TH1F* h_result[3];
    h_result[0] = new TH1F ("h_result_el","all", 100,0,0.01);
    h_result[1] = new TH1F ("h_result_pi","all", 100,0,0.01);
    h_result[2] = new TH1F ("h_result_pr","all", 100,0,0.01);

    const int nbars(config->GetSciDetNBars());

    for(int ievt(0); ievt<data->GetEntries(); ievt++){
        data->GetEntry(ievt);

        std::vector< SciDetHit* > listOfSciHits = event->GetSciDetHits();
        std::vector< SciDetHit* >::iterator sciHit;

        int    prelayerID = -999;
        double    calDep     = 0;
        bool   IsSelected = true;
        // for(sciHit=listOfSciHits.begin(); sciHit!=listOfSciHits.end(); sciHit++){
        //     if ((*sciHit)->GetLayerID() < prelayerID) {cout << "error." << endl; exit(0);}
        //     if(!(*sciHit)->GetBarID() or (*sciHit)->GetBarID() == nbars-1)  IsSelected = false;
        //     if((*sciHit)->GetLayerID()) if((*sciHit)->GetLayerID() - prelayerID > 1) IsSelected = false;
        //     prelayerID = (*sciHit)->GetLayerID();
        // }

        std::map <int, double > trackIdToGunEnergy = event->GetGunEnergyMap();
        std::map <int, double >::iterator gunEit;
        gunEit = trackIdToGunEnergy.begin();

        int mode = 0;
        if ((*gunEit).second<700) mode = 1;
        if ((*gunEit).second<400) mode = 2;
    
        // cout << trackIdToGunEnergy.size() << endl;
        for ( gunEit = trackIdToGunEnergy.begin(); gunEit != trackIdToGunEnergy.end(); gunEit++ )
            std::cout << mode << "," << (*gunEit).second << std::endl;

        cout << "hits: " <<  listOfSciHits.size() << endl;

        if (IsSelected){
            //cout << "here " << endl;
            prelayerID = 0;
            for(sciHit=listOfSciHits.begin(); sciHit!=listOfSciHits.end(); sciHit++){
                if ((*sciHit)->GetLayerID() < prelayerID) {cout << "error." << endl; exit(0);}
                calDep += (*sciHit)->GetEnergyDeposited();
                //if (prelayerID != (*sciHit)->GetLayerID() and !prelayerID) {h_result[mode]->Fill(calDep); cout << "calDep: " << calDep << endl; calDep = 0;}
                prelayerID = (*sciHit)->GetLayerID();
            }
            h_result[mode]->Fill(calDep); cout << "calDep: " << calDep << endl; calDep = 0;
        }

    }
    //_____________________

    TCanvas* c1 = new TCanvas("c1");
    c1->cd();

    h_result[0]->Scale(1/h_result[0]->Integral("width"));
    h_result[1]->Scale(1/h_result[1]->Integral("width"));
    h_result[2]->Scale(1/h_result[2]->Integral("width"));

    h_result[0]->SetLineColor(kGreen+1);
    h_result[1]->SetLineColor(kRed);
    h_result[2]->SetLineColor(kBlue);

    h_result[1]->Draw("HIST");
    h_result[0]->Draw("HIST same");
    h_result[2]->Draw("HIST same");
    c1->Update();
    c1->WaitPrimitive();

    inputFile->Close();
    theApp->Run();

    return 0;
}
