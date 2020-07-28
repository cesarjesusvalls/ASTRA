#include "pCTEvent.hh"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TBox.h"
#include "TMarker.h"
#include <TRandom3.h>

double EdepToNphot = 150;
double MPPCEff     = 0.35;
double threshold   = 2.99;

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


void pCTEvent::DrawCMOSHits(pCTXML* config){

    //const int nplanes(config->GetCMOSNPlanes());
    const int    nplanes(3);
    const double pitchX(40);
    const double pitchY(36);
    const double nrows(448);
    const double ncols(224);

    TCanvas *canv = new TCanvas("canv","canv",1200,400);
    canv->Divide(nplanes,1);
    gStyle->SetOptStat(0);

    TH2F* h_hitsMap[nplanes];
    for(int p(0); p<nplanes; p++){
        TString hname = "PLANE ";
        hname += p;
        h_hitsMap[p] = new TH2F(hname.Data(),hname.Data(),nrows,0,nrows,ncols,0,ncols);
    }

    TString xLabel = "# Pixel (";
    xLabel += pitchX;
    xLabel += "#mum/Pixel)";
    TString yLabel = "# Pixel (";
    yLabel += pitchY;
    yLabel += "#mum/Pixel)";

    int gridColor = kGray;
    for (ushort p(0); p<nplanes; p++){
        canv->cd(p+1);
        gPad->SetLeftMargin(0.16);
        h_hitsMap[p]->GetXaxis()->SetTitle(xLabel.Data());
        h_hitsMap[p]->GetYaxis()->SetTitle(yLabel.Data());
        h_hitsMap[p]->GetXaxis()->SetTickLength(0.);
        h_hitsMap[p]->GetYaxis()->SetTickLength(0.);
        h_hitsMap[p]->GetYaxis()->SetTitleOffset(1.6);
        h_hitsMap[p]->Draw("COLZ");
        TBox *cmosBox = new TBox(0,0,nrows,ncols);
        cmosBox->SetFillColor(gridColor);
        cmosBox->Draw("same");

        std::map<G4int, std::vector< CMOSPixel* > > Counter = this->GetPixelHitsMap();
        std::map<G4int, std::vector< CMOSPixel*> >::iterator it;
        for(it=Counter.begin(); it!=Counter.end(); it++){
            ushort Plane = (*it).first;
            if (Plane != p) continue;
            ushort nHitsInPlane = (*it).second.size();
            for(ushort index(0); index<nHitsInPlane; index++)
            {
                ushort X = (*it).second.at(index)->GetX();
                ushort Y = (*it).second.at(index)->GetY();
                ushort e = (*it).second.at(index)->GetElectronsLiberated();

                TMarker *markHit = new TMarker(X,Y,29);
                markHit->SetMarkerColor(index+1);
                markHit->SetMarkerSize(3);
                markHit->Draw("same");
                //h_hitsMap[Plane]->SetBinContent(X+1,Y+1,e);
            }
        }
    }
    
    canv->Update();
    canv->WaitPrimitive();

    for (ushort plane(0); plane<nplanes; plane++) delete h_hitsMap[plane];
    delete canv;

}

std::vector< pCTTrack* > pCTEvent::Reconstruct(pCTXML* config){

    // concept (for 1 proton):
    // for the time being, just check that the track is continuous both in XZ and YZ and the deflection is ~small.

    bool IsSelected = true;
    if (fGunEnergy.size()>1) {cout << "pCTEvent::TrackSelection || Selection relays on single proton assumptions!" << endl; exit(0);}

    // in the future use this to add more than 1 proton...
    std::vector< pCTTrack* > recoTracks;
    pCTTrack* recoProt = new pCTTrack();

    std::vector< SciDetHit* > listOfSciHits = this->GetSciDetHits();
    std::vector< SciDetHit* >::iterator sciHit;

    double ini[3] = {0};
    double fin[3] = {0};

    double pos[3] = {0};

    int nbars(config->GetSciDetNBars());

    double maxEdep[config->GetSciDetNLayers()];
    memset( maxEdep, 0, config->GetSciDetNLayers()*sizeof(double) );

    std::vector<TVector3> listOf3Dhits;

    double calDep   = 0;
    double accumRng = 0;
    int    prelayerID = -999;
    for(sciHit=listOfSciHits.begin(); sciHit!=listOfSciHits.end(); sciHit++){
        if ((*sciHit)->GetLayerID() < prelayerID) {cout << "pCTEvent::TrackSelection || LAYERS ARE NOT ORDERED! required assumption not fullfilled." << endl; exit(0);}
        if ((*sciHit)->GetEnergyDeposited() > maxEdep[(*sciHit)->GetLayerID()]){
            maxEdep[(*sciHit)->GetLayerID()] = (*sciHit)->GetEnergyDeposited();
            if(!(*sciHit)->GetBarID() or (*sciHit)->GetBarID() == nbars-1)  IsSelected = false;
            (*sciHit)->GetOrientation() ? pos[0] = (*sciHit)->GetBarID() : pos[1] = (*sciHit)->GetBarID();
            pos[2] = (*sciHit)->GetLayerID();
            if ((*sciHit)->GetLayerID()){
                listOf3Dhits.push_back(TVector3(pos[0],pos[1],pos[2]-0.5));
                if(listOf3Dhits.size()>1) accumRng += (*(listOf3Dhits.end()-2)-*(listOf3Dhits.end()-1)).Mag();
            }
        }

        int hitPE = 0;
        for (int phot(0); phot < (int) ((*sciHit)->GetEnergyDeposited()*EdepToNphot+0.5); phot++)
            if (fRndm->Uniform() < MPPCEff) hitPE++;

        if(hitPE > threshold) calDep += hitPE;

        // reject if there are holes!
        if((*sciHit)->GetLayerID()) if((*sciHit)->GetLayerID() - prelayerID > 1) IsSelected = false;
        prelayerID = (*sciHit)->GetLayerID();
    }

    if(listOf3Dhits.size()){
        recoProt->Set3DHits(listOf3Dhits);
        
        recoProt->SetRecoMeas(0, calDep);
        //cout << (*listOf3Dhits.begin()).Z() << "," <
        recoProt->SetRecoMeas(1,-(*listOf3Dhits.begin()).Z()+(*(listOf3Dhits.end()-1)).Z()+0.5);
        recoProt->SetRecoMeas(2,((*listOf3Dhits.begin())-*(listOf3Dhits.end()-1)).Mag()+0.5);
        recoProt->SetRecoMeas(3, accumRng + 0.5);
        
        double straightness = (recoProt->GetRecoMeas(3)-recoProt->GetRecoMeas(2))/recoProt->GetRecoMeas(3);
        recoProt->SetStraightness(straightness);

        if (IsSelected) recoTracks.push_back(recoProt);
    }

    return recoTracks;
}
