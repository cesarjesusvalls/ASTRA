
#include <iostream>

#include "CMOSPixel.hh"
#include "TrackFitter.cc"
#include "pCTEvent.hh"
#include "pCTXML.hh"

#include "TROOT.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TBox.h"
#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"
#include "TString.h"
#include "TF1.h"
#include "TApplication.h"
#include "TGraphErrors.h"

int main(int argc,char** argv){

    TApplication* theApp=new TApplication("App",&argc,argv);

    std::cout << "Starting Analysis Example..." << std::endl;

    TString inputFileName = "/Users/cjesus/Dev/protonCT/output/simulation_file.root";
    TFile* inputFile = new TFile(inputFileName.Data(),"update");
    if(!inputFile) {std::cout << "no output file! exit." << std::endl; exit(1);}
    
    TTree*   data       = (TTree*) inputFile->Get("pCT_Events");
    if(!data) {std::cout << "TTree not found." << std::endl; exit(1);}
    TBranch* dataBranch = data->GetBranch("Event");
    pCTEvent* event = new pCTEvent();
    dataBranch->SetAddress(&event);

    pCTXML* config = (pCTXML*) inputFile->Get("XMLinput");
    std::cout << config->GetSciDetNBars()  <<"-------------------------------\n";

    double Z_pos[4] = {0.,5.,10.,15.};

    for(int ievt(0); ievt<data->GetEntries(); ievt++){
        data->GetEntry(ievt);

        std::vector <TVector3> pointsToFit;

        for (ushort p(0); p<4; p++){
            std::map<G4int, std::vector< CMOSPixel* > > Counter = event->GetPixelHitsMap();
            std::map<G4int, std::vector< CMOSPixel*> >::iterator it;
            for(it=Counter.begin(); it!=Counter.end(); it++){
                ushort Plane = (*it).first;
                if (Plane != p) continue;
                ushort nHitsInPlane = (*it).second.size();
                for(ushort index(0); index<nHitsInPlane; index++)
                {
                    ushort X = (*it).second.at(index)->GetX();
                    ushort Y = (*it).second.at(index)->GetY();
                    pointsToFit.push_back(TVector3(X,Y,Z_pos[p]));
                    //std::cout << X << "," << Y << "," << Z_pos[p] << std::endl;
                    std::cout << pointsToFit[pointsToFit.size()-1].X() << "," << pointsToFit[pointsToFit.size()-1].Y() << "," << pointsToFit[pointsToFit.size()-1].Z() << std::endl;
                }
            }
        }

        std::cout << pointsToFit.size() << std::endl;
        if(pointsToFit.size() !=4) continue;
        event->DrawCMOSHits(config);
        // event->DrawSciDetHits(config);
        TrackFitter(pointsToFit,1);        
    }
    //_____________________

    TCanvas* c1 = new TCanvas("c1");
    c1->cd();

    c1->Update();
    c1->WaitPrimitive();

    inputFile->Close();
    theApp->Run();

    return 0;
}
