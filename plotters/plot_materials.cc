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

    TF1* g1 = new TF1("g1","gaus",150,185);
    TF1* g2 = new TF1("g2","gaus",150,185);
    TF1* g3 = new TF1("g3","gaus",150,185);
    TF1* g4 = new TF1("g4","gaus",150,185);
    TF1* g5 = new TF1("g5","gaus",150,185);

    g1->SetParameters(100,160,5);
    g2->SetParameters(100,160,5);
    g3->SetParameters(100,160,5);
    g4->SetParameters(100,160,5);
    g5->SetParameters(100,160,5); 

    TCanvas* c1 = new TCanvas("c1","");
    c1->cd();

    gPad->SetRightMargin(0.04);
    gPad->SetLeftMargin(0.14);

    h_imag[0]->SetStats(0);
    h_imag[1]->SetStats(0);
    h_imag[2]->SetStats(0);
    h_imag[3]->SetStats(0);
    h_imag[4]->SetStats(0);

    h_imag[0]->SetLineColor(kRed);
    h_imag[1]->SetLineColor(kGreen+1);
    h_imag[2]->SetLineColor(kOrange-5);
    h_imag[3]->SetLineColor(kAzure+2);
    h_imag[4]->SetLineColor(kCyan);

    h_imag[0]->SetMarkerColor(kRed);
    h_imag[1]->SetMarkerColor(kGreen+1);
    h_imag[2]->SetMarkerColor(kOrange-5);
    h_imag[3]->SetMarkerColor(kAzure+2);
    h_imag[4]->SetMarkerColor(kCyan);

    g1->SetNpx(300);
    g2->SetNpx(300);
    g3->SetNpx(300);
    g4->SetNpx(300);
    g5->SetNpx(300);

    g1->SetLineColor(kRed);
    g2->SetLineColor(kGreen+1);
    g3->SetLineColor(kOrange-5);
    g4->SetLineColor(kAzure+2);
    g5->SetLineColor(kCyan);

    g1->SetLineWidth(2);
    g2->SetLineWidth(2);
    g3->SetLineWidth(2);
    g4->SetLineWidth(2);
    g5->SetLineWidth(2);

    h_imag[0]->SetLineWidth(0);
    h_imag[1]->SetLineWidth(0);
    h_imag[2]->SetLineWidth(0);
    h_imag[3]->SetLineWidth(0);
    h_imag[4]->SetLineWidth(0);

    h_imag[0]->Fit("g1");
    h_imag[1]->Fit("g2");
    h_imag[2]->Fit("g3");
    h_imag[3]->Fit("g4");
    h_imag[4]->Fit("g5");

    h_imag[0]->GetXaxis()->SetTitle("Reconstructed Energy [MeV]");
    h_imag[0]->GetYaxis()->SetTitle("Entres");
    h_imag[0]->GetYaxis()->SetTitleOffset(1.4);
    h_imag[0]->SetTitle("");

    h_imag[0]->GetYaxis()->SetRangeUser(0,4000);
    h_imag[0]->DrawCopy("P9"); 
    h_imag[1]->DrawCopy("P9 same");
    h_imag[2]->DrawCopy("P9 same");
    h_imag[3]->DrawCopy("P9 same");
    h_imag[4]->DrawCopy("P9 same");

    h_imag[2]->DrawCopy("same");
    h_imag[1]->DrawCopy("same");
    h_imag[4]->DrawCopy("same");
    h_imag[3]->DrawCopy("same");
    h_imag[0]->DrawCopy("same");


    auto leg1 = new TLegend(0.15, 0.905, 0.94, 0.98);
    leg1->SetNColumns(5);
    leg1->AddEntry(g3, "hcBone ","l");
    leg1->AddEntry(g2, "ribBone ","l");
    leg1->AddEntry(g5, "water ","l");
    leg1->AddEntry(g4, "adipose ","l");
    leg1->AddEntry(g1, "lung","l");

    leg1->SetBorderSize(0);
    leg1->Draw("same");

    c1->Update();


    for(uint k(0); k<5; ++k) file[k]->Close();
    theApp->Run();
}
