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
#include "TApplication.h"


int main(int argc,char** argv){

    TString localStyleName = "pCTstyle";
    // -- WhichStyle --
    // 1 = presentation large fonts
    // 2 = presentation small fonts
    // 3 = publication/paper
    Int_t localWhichStyle = 2;
    TStyle* pCTstyle = SetpCTStyle(localWhichStyle, localStyleName);
    gROOT->SetStyle(localStyleName);

    TApplication* theApp=new TApplication("App",&argc,argv);
    TString inputFileName = "/Users/cjesus/Dev/protonCT/output/simulation_file.root";
    TFile* inputFile = new TFile(inputFileName.Data(),"update");
    if(!inputFile) {std::cout << "no output file! exit." << std::endl; exit(1);}
    
    TTree*   data       = (TTree*) inputFile->Get("pCT_Events");
    if(!data) {std::cout << "TTree not found." << std::endl; exit(1);}
    TBranch* dataBranch = data->GetBranch("Event");
    pCTEvent* event = new pCTEvent();
    dataBranch->SetAddress(&event);
    pCTXML* config = (pCTXML*) inputFile->Get("XMLinput");

    //_______THE ANALYSIS______

    const int nbars(config->GetSciDetNBars());
    const int nlayers(config->GetSciDetNLayers());

    double hitsMap [nlayers][nbars][2];
    memset( hitsMap, 0, nlayers*nbars*2*sizeof(int) );
    TCanvas *canv = new TCanvas("canv","canv",600,1200);
    canv->Divide(1,2);
    gStyle->SetOptStat(0);
    TH2F* h_hitsMap[2];
    h_hitsMap[0] = new TH2F("h_hitsMapZY","TOP VIEW",nlayers,0,nlayers,nbars,0,nbars);
    h_hitsMap[1] = new TH2F("h_hitsMapZX","SIDE VIEW",nlayers,0,nlayers,nbars,0,nbars);
    for(int ievt(0); ievt<data->GetEntries(); ievt++){
        data->GetEntry(ievt);
        std::vector< SciDetHit* > listOfSciHits = event->GetSciDetHits();
        std::vector< SciDetHit* >::iterator sciHit;

        hitsMap[(*sciHit)->GetLayerID()][(*sciHit)->GetBarID()][(*sciHit)->GetOrientation()] 
        = (*sciHit)->GetEnergyDeposited();

        event->DrawSciDetHits(config);
    }
    
    //_____________________

    inputFile->Close();
    theApp->Run();

    return 0;
}
