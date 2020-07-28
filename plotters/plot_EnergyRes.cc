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
    TString inputFileName = "/Users/cjesus/Dev/protonCT/output/simulation_file.root";


    int     measurements [4] = {0,1,2,3};
    double  coating      [4] = {0,0.01,0.02,0.1};
    int     barZthick    [3] = {3,5,10};

    TString fnames [4][4][3];
    TFile* files [4][4][3];
    TGraphErrors* h_ERes [4][4][3];

    TString baseName = "/Users/cjesus/Dev/protonCT/output/energyRes_";
    for(uint i(0); i<4; ++i)
        for(uint j(0); j<4; ++j)
            for(uint k(0); k<3; ++k){
                TString coating_val;
                if( j == 0)           coating_val = Form("%.0f",coating[j]);
                if( j == 1 or j == 2) coating_val = Form("%.2f",coating[j]);
                if( j == 3)           coating_val = Form("%.1f",coating[j]);
                fnames[i][j][k]    = baseName +  measurements[i] + "_" + barZthick[k] + "_" + coating_val.Data() + ".root"; 
                files[i][j][k]     = new TFile(fnames[i][j][k].Data(), "READ");
                h_ERes[i][j][k]    = (TGraphErrors*) files[i][j][k]->Get("EResByRng");
            }

    // NO COATING different bar size for different true energy estimators:

    TBox *line1pcnt = new TBox(30,1-0.005,240,1+0.005);
    line1pcnt->SetFillColor(kRed);
    TCanvas* c1 = new TCanvas("c1","");
    c1->cd();

    for(uint i(0); i<4; ++i){
        cout << "plot for different estimators (no coating)..." << endl;
        h_ERes[i][0][0]->SetMarkerColor(kRed);
        h_ERes[i][0][1]->SetMarkerColor(kBlue);
        h_ERes[i][0][2]->SetMarkerColor(kGreen+1);

        h_ERes[i][0][0]->SetMarkerStyle(21);
        h_ERes[i][0][1]->SetMarkerStyle(22);
        h_ERes[i][0][2]->SetMarkerStyle(23);

        h_ERes[i][0][2]->GetXaxis()->SetTitle("Proton True Kinetic Energy [MeV]");
        h_ERes[i][0][2]->GetYaxis()->SetTitle("Energy resolution #sigma = (True-Reco) / True [%]");

        h_ERes[i][0][2]->GetYaxis()->SetRangeUser(0,5);

        h_ERes[i][0][2]->Draw("AP");
        h_ERes[i][0][1]->Draw("P same");
        h_ERes[i][0][0]->Draw("P same");

        auto leg1 = new TLegend(0.55,0.65,0.85,0.85);
        leg1->AddEntry(h_ERes[i][0][0], "3  mm ","P");
        leg1->AddEntry(h_ERes[i][0][1], "5  mm ","P");
        leg1->AddEntry(h_ERes[i][0][2], "10 mm ","P");
        line1pcnt->Draw("same");

        leg1->SetBorderSize(0);
        leg1->Draw("same");

        c1->Update();
        c1->SaveAs(TString::Format("/Users/cjesus/Work/plots_pCT/DiffEstim/eRes_NoCoat_Estim_%i.pdf",i).Data());
        c1->WaitPrimitive();
    }

    for(uint i(0); i<3; ++i){
        cout << "plot for different coatings (estimator == 2)..." << endl;
        h_ERes[2][0][i]->SetMarkerColor(kRed);
        h_ERes[2][1][i]->SetMarkerColor(kBlue);
        h_ERes[2][2][i]->SetMarkerColor(kGreen+1);
        h_ERes[2][3][i]->SetMarkerColor(kBlack);

        h_ERes[2][0][i]->SetMarkerStyle(21);
        h_ERes[2][1][i]->SetMarkerStyle(22);
        h_ERes[2][2][i]->SetMarkerStyle(23);
        h_ERes[2][3][i]->SetMarkerStyle(47);

        h_ERes[2][3][i]->GetXaxis()->SetTitle("Proton True Kinetic Energy [MeV]");
        h_ERes[2][3][i]->GetYaxis()->SetTitle("Energy resolution #sigma = (True-Reco) / True [%]");

        h_ERes[2][3][i]->GetYaxis()->SetRangeUser(0,5);

        h_ERes[2][3][i]->Draw("AP");
        h_ERes[2][2][i]->Draw("P same");
        h_ERes[2][1][i]->Draw("P same");
        h_ERes[2][0][i]->Draw("P same");

        auto leg2 = new TLegend(0.55,0.65,0.85,0.85);
        for(uint j(0); j<4; ++j)
            leg2->AddEntry(h_ERes[2][j][i],TString::Format("%i mm | coat: %.2f",barZthick[i],coating[j]).Data(),"P");

        line1pcnt->Draw("same");
        leg2->SetBorderSize(0);
        leg2->Draw("same");

        c1->Update();
        c1->SaveAs(TString::Format("/Users/cjesus/Work/plots_pCT/DiffCoating/eRes_Estim2_Coat_%i.pdf",i).Data());
        c1->WaitPrimitive();
    }

    for(uint i(0); i<3; ++i){
        cout << "plot for different thickness (no coating)..." << endl;
        h_ERes[0][0][i]->SetMarkerColor(kRed);
        h_ERes[1][0][i]->SetMarkerColor(kBlue);
        h_ERes[2][0][i]->SetMarkerColor(kGreen+1);
        h_ERes[3][0][i]->SetMarkerColor(kBlack);

        h_ERes[0][0][i]->SetMarkerStyle(21);
        h_ERes[1][0][i]->SetMarkerStyle(22);
        h_ERes[2][0][i]->SetMarkerStyle(23);
        h_ERes[3][0][i]->SetMarkerStyle(47);

        h_ERes[3][0][i]->GetXaxis()->SetTitle("Proton True Kinetic Energy [MeV]");
        h_ERes[3][0][i]->GetYaxis()->SetTitle("Energy resolution #sigma = (True-Reco) / True [%]");

        h_ERes[3][0][i]->GetYaxis()->SetRangeUser(0,5);

        h_ERes[3][0][i]->Draw("AP");
        h_ERes[2][0][i]->Draw("P same");
        h_ERes[1][0][i]->Draw("P same");
        h_ERes[0][0][i]->Draw("P same");

        auto leg3 = new TLegend(0.55,0.65,0.85,0.85);

        for(uint j(0); j<4; ++j)
            leg3->AddEntry(h_ERes[j][0][i],TString::Format("Z-thick: %i mm | rng estimator: %i",barZthick[i],measurements[j]).Data(),"P");

        line1pcnt->Draw("same");
        leg3->SetBorderSize(0);
        leg3->Draw("same");

        c1->Update();
        c1->SaveAs(TString::Format("/Users/cjesus/Work/plots_pCT/DiffThick/eRes_NoCoat_Thick_%i.pdf",i).Data());
        c1->WaitPrimitive();
    }

    // TGraphErrors* g_RngAndCal = new TGraphErrors();
    // g_RngAndCal->SetName("g_RngAndCal");

    // for (int it(0); it<h_ERes[0][0][0]->GetN(); it++){

    //     double x_val_1;
    //     double y_val_1;
    //     double x_val_2;
    //     double y_val_2;

    //     h_ERes[0][0][0]->GetPoint(it,x_val_1,y_val_1);
    //     h_ERes[1][0][0]->GetPoint(it,x_val_2,y_val_2);

    //     double y_val_err_1 = h_ERes[0][0][0]->GetErrorY(it);
    //     double y_val_err_2 = h_ERes[1][0][0]->GetErrorY(it);

    //     cout << x_val_1 << ", " << x_val_2 << ", " << y_val_1 << ", " << y_val_2 << ", " << y_val_err_1 << ", " << y_val_err_2 << endl; 

    //     double new_x = x_val_1;
    //     double new_y = (y_val_1/y_val_err_1+y_val_2/y_val_err_2)*y_val_err_1*y_val_err_2/(y_val_err_1+y_val_err_2);

    //     g_RngAndCal->SetPoint(g_RngAndCal->GetN(),new_x,new_y);
    // }

    // g_RngAndCal->Draw("AP");
    // h_ERes[0][0][0]->Draw("P same");
    // h_ERes[1][0][0]->Draw("P same");
    // c1->Update();
    // c1->WaitPrimitive();
    // c1->SaveAs(TString::Format("/Users/cjesus/Work/plots_pCT/eRes_RngAndCal.pdf").Data());



    for(uint i(0); i<4; ++i)
        for(uint j(0); j<4; ++j)
            for(uint k(0); k<3; ++k)
                files[i][j][k]->Close();

    return 0;
    theApp->Run();
}
