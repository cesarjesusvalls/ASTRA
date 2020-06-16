#include "pCTEvent.hh"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TBox.h"

void pCTEvent::DrawSciDetHits(pCTXML* config){

	TCanvas *canv = new TCanvas("canv","canv",600,1200);
    canv->Divide(1,2);
    gStyle->SetOptStat(0);

	const int nbars(config->GetSciDetNBars());
    const int nlayers(config->GetSciDetNLayers());

	TH2F* h_hitsMap[2];
    h_hitsMap[0] = new TH2F("h_hitsMapZY","TOP VIEW",nlayers,0,nlayers,nbars,0,nbars);
    h_hitsMap[1] = new TH2F("h_hitsMapZX","SIDE VIEW",nlayers,0,nlayers,nbars,0,nbars);

    std::vector< SciDetHit* > listOfSciHits = this->GetSciDetHits();
    std::vector< SciDetHit* >::iterator sciHit;
    for(sciHit=listOfSciHits.begin(); sciHit!=listOfSciHits.end(); sciHit++){
        int     layerID = (*sciHit)->GetLayerID();
        int     barID   = (*sciHit)->GetBarID();
        double  edep    = (*sciHit)->GetEnergyDeposited();
        h_hitsMap[(*sciHit)->GetOrientation()]->SetBinContent(layerID+1,barID+1,edep);
    }

    TString xLabel = "# Layer (";
    xLabel += config->GetSciDetBarZ();
    xLabel += "mm/Layer)";

    TString yLabel = "# Bar (";
    yLabel += config->GetSciDetBarX();
    yLabel += "mm/Bar)";

    h_hitsMap[0]->GetXaxis()->SetTitle(xLabel.Data());
    h_hitsMap[0]->GetYaxis()->SetTitle(yLabel.Data());

    h_hitsMap[0]->GetXaxis()->SetTickLength(0.);
    h_hitsMap[1]->GetXaxis()->SetTickLength(0.);
    h_hitsMap[0]->GetYaxis()->SetTickLength(0.);
    h_hitsMap[1]->GetYaxis()->SetTickLength(0.);

    h_hitsMap[1]->GetXaxis()->SetTitle(xLabel.Data());
    h_hitsMap[1]->GetYaxis()->SetTitle(yLabel.Data());

    canv->cd(1);
    int gridColor = kGray;
    h_hitsMap[0]->Draw("COLZ");
    for(int i(1); i<nbars; i++){
        TBox *bargrid = new TBox(0,i-0.025,nlayers,i+0.025);
        bargrid->SetFillColor(gridColor);
        bargrid->Draw("same");
    }
    for(int i(1); i<nlayers; i+=2){
        TBox *blindbox = new TBox(i,0,i+1,nbars);
        blindbox->SetFillColor(gridColor);
        blindbox->Draw("same");
    }
    canv->cd(2);
    h_hitsMap[1]->Draw("COLZ");
    for(int i(1); i<nbars; i++){
        TBox *bargrid = new TBox(0,i-0.025,nlayers,i+0.025);
        bargrid->SetFillColor(gridColor);
        bargrid->Draw("same");
    }
    for(int i(0); i<nlayers; i+=2){
        TBox *blindbox = new TBox(i,0,i+1,nbars);
        blindbox->SetFillColor(gridColor);
        blindbox->Draw("same");
    }
    canv->Update();
    canv->WaitPrimitive();

    delete h_hitsMap[0];
    delete h_hitsMap[1];
    delete canv;

}
