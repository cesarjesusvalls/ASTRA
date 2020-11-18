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

    TFile* file = new TFile("/Users/cjesus/Dev/protonCT/output/image_2_3_0.05_CMOS.root", "READ");
    TH2F* h_imag  = (TH2F*) file->Get("image");

    // NO COATING different bar size for different true energy estimators:


    gStyle->SetOptStat(0);
    gStyle->SetPalette(kGreyScale);

    TCanvas* c1 = new TCanvas("c1","", 600,600);
    c1->cd();

    gPad->SetTopMargin( 0.18 );
    gPad->SetRightMargin( 0.18 );
    gPad->SetLeftMargin( 0.18 );
    gPad->SetBottomMargin( 0.18 );

    //gPad->SetLogz();

    h_imag->SetTitle("");

    h_imag->GetYaxis()->SetTitleOffset(1.2);

    h_imag->GetXaxis()->SetTitle("cm");
    h_imag->GetYaxis()->SetTitle("cm");

    h_imag->GetZaxis()->SetRangeUser(165,168);
    h_imag->GetXaxis()->SetRangeUser(-20,20);
    h_imag->GetYaxis()->SetRangeUser(-20,20);
    h_imag->DrawCopy("CONT1Z");
    c1->Update();


    TCanvas* c2 = new TCanvas("c2","", 600,600);
    c2->cd();

    gPad->SetTopMargin( 0.18 );
    gPad->SetRightMargin( 0.18 );
    gPad->SetLeftMargin( 0.18 );
    gPad->SetBottomMargin( 0.18 );
    //gPad->SetLogz();

    h_imag->SetTitle("");

    h_imag->GetYaxis()->SetTitleOffset(1.2);

    h_imag->GetXaxis()->SetTitle("cm");
    h_imag->GetYaxis()->SetTitle("cm");

    h_imag->GetZaxis()->SetRangeUser(153,178);
    h_imag->GetXaxis()->SetRangeUser(-20,20);
    h_imag->GetYaxis()->SetRangeUser(-20,20);
    h_imag->DrawCopy("COLZ");
    c2->Update();



    file->Close();
    theApp->Run();
}



