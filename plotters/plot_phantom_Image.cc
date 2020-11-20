//
// C. Jesús Valls 
// cjesus@ifae.es
//
// M. Granado-González
// MMG998@student.bham.ac.uk
//

#include "DMAPSPixel.hh"
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
    gStyle->SetPalette(kGreyScale);

    // iterate over number of protons
    for (int i(0); i<2; i++){

        TFile* file;
        TH2F* h_imag;

        if(i==0)
            file = new TFile("../output/image_3mm_1protons_analysis.root", "READ");
        else if(i==1)
            file = new TFile("../output/image_3mm_2protons_analysis.root", "READ");

        h_imag  = (TH2F*) file->Get("image");

        TCanvas* c1 = new TCanvas("c1","", 600,600);
        c1->cd();

        gPad->SetTopMargin( 0.18 );
        gPad->SetRightMargin( 0.18 );
        gPad->SetLeftMargin( 0.18 );
        gPad->SetBottomMargin( 0.18 );

        //gPad->SetLogz();

        h_imag->SetTitle("");

        h_imag->GetYaxis()->SetTitleOffset(1.2);

        h_imag->GetXaxis()->SetTitle("mm");
        h_imag->GetYaxis()->SetTitle("mm");

        //if(i==0) h_imag->GetZaxis()->SetRangeUser(165,168);
        //else if(i==1) h_imag->GetZaxis()->SetRangeUser(167,170);

        h_imag->GetZaxis()->SetRangeUser(167,170);
        h_imag->GetXaxis()->SetRangeUser(-20,20);
        h_imag->GetYaxis()->SetRangeUser(-20,20);
        h_imag->DrawCopy("CONT1Z");
        c1->Update();
        if(i==0){
            c1->SaveAs("../plots/phantom_image_enhanced_1protons.pdf");
            c1->SaveAs("../plots/phantom_image_enhanced_1protons.png");
        }
        else if(i==1){
            c1->SaveAs("../plots/phantom_image_enhanced_2protons.pdf");
            c1->SaveAs("../plots/phantom_image_enhanced_2protons.png");
        }

        TCanvas* c2 = new TCanvas("c2","", 600,600);
        c2->cd();

        gPad->SetTopMargin( 0.18 );
        gPad->SetRightMargin( 0.18 );
        gPad->SetLeftMargin( 0.18 );
        gPad->SetBottomMargin( 0.18 );
        //gPad->SetLogz();

        h_imag->SetTitle("");

        h_imag->GetYaxis()->SetTitleOffset(1.2);

        h_imag->GetXaxis()->SetTitle("mm");
        h_imag->GetYaxis()->SetTitle("mm");

        h_imag->GetZaxis()->SetRangeUser(153,178);
        h_imag->GetXaxis()->SetRangeUser(-20,20);
        h_imag->GetYaxis()->SetRangeUser(-20,20);
        h_imag->DrawCopy("COLZ");
        c2->Update();
        if(i==0){
            c2->SaveAs("../plots/phantom_image_raw_1protons.pdf");
            c2->SaveAs("../plots/phantom_image_raw_1protons.png");
        }
        else if(i==1){
            c2->SaveAs("../plots/phantom_image_raw_2protons.pdf");
            c2->SaveAs("../plots/phantom_image_raw_2protons.png");
        }
        
        file->Close();
    }

    theApp->Run();
}



