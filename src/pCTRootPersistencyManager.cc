//
// C. Jesús Valls 
// cjesus@ifae.es
//
// M. Granado-González
// MMG998@student.bham.ac.uk
//

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

#include "pCTRootPersistencyManager.hh"

#include <memory>
#include <cmath>
#include <algorithm>

#include <globals.hh>

#include <G4Event.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4UnitsTable.hh>
#include <G4ParticleTable.hh>
#include <G4PrimaryVertex.hh>
#include <G4PrimaryParticle.hh>
#include <G4SDManager.hh>
#include <G4HCtable.hh>
#include <G4VisAttributes.hh>
#include <G4VSolid.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Sphere.hh>
#include <G4Polyhedra.hh>
#include <G4Trap.hh>
#include <G4StepStatus.hh>
#include <G4TransportationManager.hh>
#include <G4FieldManager.hh>
#include <G4PrimaryVertex.hh>

#include "G4RunManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"


TROOT root("ROOT","Root of Everything");

pCTRootPersistencyManager::pCTRootPersistencyManager() :
  fOutput(NULL),fEventTree(NULL), fpCTXMLInput(),fEventFirst(-99999),fNEvents(-99999),fEventsNotSaved(0){}

pCTRootPersistencyManager* pCTRootPersistencyManager::GetInstance() {
    pCTRootPersistencyManager *current = dynamic_cast<pCTRootPersistencyManager*>(G4VPersistencyManager::GetPersistencyManager());

    if (!current) current = new pCTRootPersistencyManager();
    return current;
}

pCTRootPersistencyManager::~pCTRootPersistencyManager() {
    if (fOutput) delete fOutput; 
    fOutput = NULL;
    delete fpCTXMLInput; fpCTXMLInput = NULL;
}

bool pCTRootPersistencyManager::IsOpen() {
    if (fOutput && fOutput->IsOpen()) {
      fOutput->cd();
      return true;
    }
    return false;
}

void pCTRootPersistencyManager::OpenXML(G4String filename){
    fpCTXMLInput = new pCTXML(filename); 
}

bool pCTRootPersistencyManager::Open(G4String filename) {  
    SetFilename(filename +".root");
    G4cout << "pCTRootPersistencyManager::Open " << GetFilename() << G4endl;
    fOutput    = new TFile(GetFilename().c_str(),"RECREATE");
    fEventTree = new TTree("pCT_Events","pCT Tree of Events");  
    fpCTEvent  = new pCTEvent();
    fEventTree->Branch("Event",&fpCTEvent,128000,0);
    fEventsNotSaved = 0;
    return true;
}

bool pCTRootPersistencyManager::Close() {
    if (!fOutput) {
        G4ExceptionDescription msg;
        msg << "No Output File" << G4endl; 
        G4Exception("pCTRootPersistencyManager::Close",
        "ExN02Code001",FatalException, msg);
        return false;
    }

    fOutput->cd();
    fEventTree->Write();
    fpCTXMLInput->Write("XMLinput");
    fOutput->Close();

    delete fpCTXMLInput; fpCTXMLInput=NULL;
    G4cout << "Output file " << GetFilename() << " closed." << G4endl;

    fEventTree = NULL;
    fpCTEvent  = NULL;
    
    return true;
}

G4bool pCTRootPersistencyManager::Store(const G4Event* anEvent) {

  std::map <int, double > trackIdToGunEnergy;
  G4PrimaryVertex* vtx = anEvent->GetPrimaryVertex();
  while(vtx){
      trackIdToGunEnergy[vtx->GetPrimary()->GetTrackID()] = vtx->GetPrimary()->GetKineticEnergy();
      vtx = vtx->GetNext();
  }

  fpCTEvent->SetEvtId(anEvent->GetEventID());
  //G4cout << trackIdToGunEnergy.size() << G4endl;
  fpCTEvent->SetGunEnergyMap(trackIdToGunEnergy);
  fEventTree->Fill();
  if(anEvent->GetEventID()%10==0) G4cout << "STATUS: ...Processing Event " << anEvent->GetEventID() << G4endl;
  fpCTEvent->ResetEvent();

  return false;
}

G4bool pCTRootPersistencyManager::Store(const G4Run* aRun) {
  (void)aRun;
  //pCTSevere(" -- Run store called without a save method " 
  //<< GetFilename());
  return false;
}

G4bool pCTRootPersistencyManager::Store(const G4VPhysicalVolume* aWorld) {
  (void)aWorld;
  //pCTSevere(" -- Geometry store called without a save method " 
  //<< GetFilename());
  return false;
}
