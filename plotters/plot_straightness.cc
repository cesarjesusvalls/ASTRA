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

    gStyle->SetOptStat(0);

    TFile* file = new TFile("../output/resolution_3mm_1protons_analysis.root", "READ");
    TH2F* h_straightness         =  (TH2F*) file->Get("h_StraightnessVsE");
    TGraphErrors* g_eff          =  (TGraphErrors*) file->Get("g_EffVsStraigness");

    TCanvas* c1 = new TCanvas("c1","", 900,600);
    c1->cd();

    gPad->SetTopMargin( 0.08 );
    gPad->SetRightMargin( 0.18 );
    gPad->SetLeftMargin( 0.18 );
    gPad->SetBottomMargin( 0.14 );

    h_straightness->SetTitle("");
    h_straightness->GetYaxis()->SetRangeUser(0.7,1);
    h_straightness->GetYaxis()->SetTitleOffset(1.8);
    h_straightness->GetXaxis()->SetTitle("True Kinetic Energy [MeV]");
    h_straightness->GetYaxis()->SetTitle("Straightness");

    h_straightness->DrawCopy("COLZ");
    c1->SaveAs("../plots/straightness_vs_energy.pdf");
    c1->SaveAs("../plots/straightness_vs_energy.png");


    TCanvas* c2 = new TCanvas("c2","", 600,600);
    c2->cd();

    gPad->SetTopMargin( 0.08 );
    gPad->SetRightMargin( 0.08 );
    gPad->SetLeftMargin( 0.14 );
    gPad->SetBottomMargin( 0.14 );

    g_eff->SetTitle("");
    g_eff->GetYaxis()->SetTitleOffset(1.4);
    g_eff->GetXaxis()->SetTitle("Straightness Cut");
    g_eff->GetYaxis()->SetTitle("Efficiency");

    g_eff->Draw("AP");

    c2->SaveAs("../plots/straightness_eff.pdf");
    c2->SaveAs("../plots/straightness_eff.png");
    
    file->Close();

    theApp->Run();
}



