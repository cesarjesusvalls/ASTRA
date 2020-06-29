
#include "CMOSPixel.hh"
#include "pCTEvent.hh"
#include "pCTXML.hh"
#include <iostream>

#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"
#include "TString.h"

int main(int argc,char** argv){

    std::cout << "Starting Analysis Example..." << std::endl;

    TString inputFileName = "/Users/marc/pCT_Geant4/protonCT/output/outputFile.root";
    TFile* inputFile = new TFile(inputFileName.Data(),"update");
    if(!inputFile) {std::cout << "no output file! exit." << std::endl; exit(1);}
    
    TTree*   data       = (TTree*) inputFile->Get("pCT_Events");
    if(!data) {std::cout << "TTree not found." << std::endl; exit(1);}
    TBranch* dataBranch = data->GetBranch("Event");
    pCTEvent* event = new pCTEvent();
    dataBranch->SetAddress(&event);

    pCTXML* config = (pCTXML*) inputFile->Get("XMLinput");
    std::cout << config->GetSciDetNBars()  <<"-------------------------------\n";

    for(int ievt(0); ievt<data->GetEntries(); ievt++){
        data->GetEntry(ievt);

        std::cout << "-oOo--oOo--oOo--oOo--oOo--oOo-\n";
        std::cout << "Event: " << ievt << std::endl;
        std::cout << "-------------CMOS-------------" << std::endl;
        std::cout << "SciDet: " << ievt << std::endl;
        std::map<G4int, std::vector< CMOSPixel* > > Counter = event->GetPixelHitsMap();
        std::cout << "Nhits:" << Counter.size() << std::endl;
        std::map<G4int, std::vector< CMOSPixel*> >::iterator it2;
        for(it2=Counter.begin(); it2!=Counter.end(); it2++)
        {
         unsigned short int Plane = (*it2).first;
         unsigned short int nHitsInPlane = (*it2).second.size();
                 std::cout << "Plane " << Plane << " has " << nHitsInPlane << " pixels above threshold" << std::endl;

         for(unsigned index(0); index<nHitsInPlane; index++)
            {
              unsigned short int X = (*it2).second.at(index)->GetX();
              unsigned short int Y = (*it2).second.at(index)->GetY();
              unsigned int e = (*it2).second.at(index)->GetElectronsLiberated();

              std::cout << X << ", " << Y << ", "<< Plane <<", " << e <<"," << std::endl;
            }
        }
        std::cout << "------------SciDet------------" << std::endl;
        std::vector< SciDetHit* > listOfSciHits = event->GetSciDetHits();
        std::cout << "# SciHits: " << listOfSciHits.size() << std::endl;
        std::vector< SciDetHit* >::iterator sciHit;
        for(sciHit=listOfSciHits.begin(); sciHit!=listOfSciHits.end(); sciHit++) std::cout << "Edep: " <<  (*sciHit)->GetEnergyDeposited() << endl;
    }
    //_____________________

    inputFile->Close();

    return 0;
}
