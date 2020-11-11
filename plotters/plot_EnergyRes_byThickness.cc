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

    TString fnames          [4];
    TFile* files            [4];
    TGraphErrors* h_ERes    [4];

    TString baseName = "/Users/cjesus/Dev/protonCT/output/energyRes_2_";
    for(uint k(0); k<3; ++k){
        fnames  [k]    = baseName + barZthick[k] + "_0.05.root"; 
        files   [k]    = new TFile(fnames[k].Data(), "READ");
        h_ERes  [k]    = (TGraphErrors*) files[k]->Get("EResByRng");
    }

    fnames  [3]    = "/Users/cjesus/Dev/protonCT/output/energyRes_2_3_0.05_CMOS.root"; 
    files   [3]    = new TFile(fnames[3].Data(), "READ");
    h_ERes  [3]    = (TGraphErrors*) files[3]->Get("EResByRng");

    cout << "alive\n";

    // NO COATING different bar size for different true energy estimators:


    TCanvas* c1 = new TCanvas("c1","");
    c1->cd();

    gPad->SetTopMargin(0.04);
    gPad->SetRightMargin(0.04);
    gPad->SetLeftMargin(0.12);
    gPad->SetBottomMargin(0.12);

    cout << "plot for different thickness..." << endl;
    h_ERes[0]->SetMarkerColor(kOrange+10);
    h_ERes[1]->SetMarkerColor(kBlue);
    h_ERes[2]->SetMarkerColor(8);
    h_ERes[3]->SetMarkerColor(kBlack);

    h_ERes[0]->SetMarkerStyle(20);
    h_ERes[1]->SetMarkerStyle(21);
    h_ERes[2]->SetMarkerStyle(22);
    h_ERes[3]->SetMarkerStyle(22);
    
    h_ERes[0]->SetMarkerSize(1.4);
    h_ERes[1]->SetMarkerSize(1.4);
    h_ERes[2]->SetMarkerSize(1.4);
    h_ERes[3]->SetMarkerSize(1.4);

    h_ERes[0]->GetYaxis()->SetTitleOffset(1.2);
    h_ERes[0]->GetXaxis()->SetTitle("Proton True Kinetic Energy [MeV]");
    h_ERes[0]->GetYaxis()->SetTitle("Energy resolution #sigma = (True-Reco) / True [%]");

    double min_x = 0;
    double max_x = 240;

    h_ERes[0]->GetYaxis()->SetRangeUser(0,5);
    h_ERes[0]->GetXaxis()->SetRangeUser(min_x,max_x);
    TBox *line1pcnt = new TBox(min_x,1-0.01,max_x,1+0.01);
    line1pcnt->SetFillColorAlpha(kCyan, 0.25);

    h_ERes[0]->Draw("AP");
    h_ERes[1]->Draw("P same");
    h_ERes[2]->Draw("P same");
    h_ERes[3]->Draw("P same");

    auto leg1 = new TLegend(0.75,0.65,0.95,0.9);
    leg1->AddEntry(h_ERes[0], "3  mm ","P");
    leg1->AddEntry(h_ERes[1], "6  mm ","P");
    leg1->AddEntry(h_ERes[2], "9 mm ","P");
    leg1->AddEntry(h_ERes[3], "3  mm + CMOS ","P");
    //line1pcnt->Draw("same");

    leg1->SetBorderSize(0);
    leg1->Draw("same");

    c1->Update();
    c1->SaveAs("/Users/cjesus/Work/plots_pCT/eRes_by_Thickness_bis.pdf");
    c1->WaitPrimitive();


    TCanvas* c2 = new TCanvas("c2","");
    c2->cd();

    gPad->SetTopMargin(0.04);
    gPad->SetRightMargin(0.04);
    gPad->SetLeftMargin(0.12);
    gPad->SetBottomMargin(0.12);

    cout << "plot for different thickness..." << endl;

    min_x = 80;
    max_x = 240;

    h_ERes[0]->GetYaxis()->SetRangeUser(0.5,1);
    h_ERes[0]->GetXaxis()->SetRangeUser(min_x,max_x);

    h_ERes[0]->GetYaxis()->SetTitleOffset(1.2);
    h_ERes[0]->Draw("AP");
    h_ERes[1]->Draw("P same");
    h_ERes[2]->Draw("P same");
    h_ERes[3]->Draw("P same");

    auto leg2 = new TLegend(0.15,0.15,0.35,0.4);
    leg2->AddEntry(h_ERes[0], "3  mm ","P");
    leg2->AddEntry(h_ERes[1], "6  mm ","P");
    leg2->AddEntry(h_ERes[2], "9  mm ","P");
    leg2->AddEntry(h_ERes[3], "3  mm + CMOS ","P");

    leg2->SetBorderSize(0);
    leg2->Draw("same");

    c2->Update();
    c2->SaveAs("/Users/cjesus/Work/plots_pCT/eRes_by_Thickness_detail_bis.pdf");
    c2->WaitPrimitive();

    for(uint k(0); k<3; ++k) files[k]->Close();

    return 0;
    theApp->Run();
}
