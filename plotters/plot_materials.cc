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
#include "TLegend.h"
#include "TStyle.h"
#include "TBox.h"
#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"
#include "TString.h"
#include "TF1.h"
#include "TApplication.h"
#include "TGraphErrors.h"

bool GenerateNewRngToEnergyTable = false;

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

    double  coating  = 0.1;
    int     barZthick[3] = {3,6,9};

    TFile* file[5];
    TH2F* h_imag[5];

    for (int i(0); i<5; i++){
        file[i] = new TFile(TString::Format("/Users/cjesus/Dev/protonCT/output/dist_M%i.root",i+1).Data(), "READ");
        h_imag[i]  = (TH2F*) file[i]->Get("h_recoE");
    }

    TCanvas* c1 = new TCanvas("c1","");
    c1->cd();

    h_imag[0]->SetLineColor(kRed);
    h_imag[1]->SetLineColor(kGreen+1);
    h_imag[2]->SetLineColor(kOrange-5);
    h_imag[3]->SetLineColor(kAzure+2);
    h_imag[4]->SetLineColor(kCyan);

    h_imag[0]->DrawCopy("HIST");
    h_imag[1]->DrawCopy("HIST same");
    h_imag[2]->DrawCopy("HIST same");
    h_imag[3]->DrawCopy("HIST same");
    h_imag[4]->DrawCopy("HIST same");

    auto leg1 = new TLegend(0.55,0.65,0.85,0.85);
    leg1->AddEntry(h_imag[0], "lung","l");
    leg1->AddEntry(h_imag[1], "ribBone ","l");
    leg1->AddEntry(h_imag[2], "hcBone ","l");
    leg1->AddEntry(h_imag[3], "adipose ","l");
    leg1->AddEntry(h_imag[4], "water ","l");

    leg1->SetBorderSize(0);
    leg1->Draw("same");

    c1->Update();


    for(uint k(0); k<5; ++k) file[k]->Close();
    theApp->Run();
}
