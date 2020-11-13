#include "G4Step.hh"
#include "Randomize.hh"

#include "G4HCofThisEvent.hh"
#include "G4HCtable.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4UIcommand.hh"
#include "pCTRootPersistencyManager.hh"
#include "pCTEvent.hh"
#include "G4Timer.hh"
#include "stdio.h"
#include "phantom.hh"
#include <TVector3.h>


Phantom::Phantom(G4String SDname)
  : G4VSensitiveDetector(SDname)
{
collectionName.insert(SDname);
HCID = -1;
pEvtID = -1;
}

Phantom::~Phantom(){}

G4bool Phantom::ProcessHits(G4Step *step, G4TouchableHistory *)
{
pCTRootPersistencyManager *InputPersistencyManager = pCTRootPersistencyManager::GetInstance();
pCTXMLInput = InputPersistencyManager->GetXMLInput();
G4TouchableHandle touchable = step->GetPreStepPoint()->GetTouchableHandle();
G4int particle_id           = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
G4int track_id              = step->GetTrack()->GetTrackID ();
G4double edep  = step->GetTotalEnergyDeposit();

G4ThreeVector point1        = step->GetPreStepPoint()->GetPosition();
G4ThreeVector point2        = step->GetPostStepPoint()->GetPosition();
G4ThreeVector worldPosition = point1 + G4UniformRand()*(point2 - point1);
G4ThreeVector localPosition = touchable->GetHistory()->GetTopTransform().TransformPoint(worldPosition);
G4bool ifPhantom = pCTXMLInput->GetUsePhantom();

 PhantomHit* hit = new PhantomHit(worldPosition, ifPhantom, edep, particle_id, track_id); 

hitCollection->insert(hit);

 return true;
 
}


void Phantom::Initialize(G4HCofThisEvent* HCE)
{
  // -- Creation of the collection     
hitCollection = new PhantomHitCollection(SensitiveDetectorName, collectionName[0]);
 if (HCID<0) HCID = GetCollectionID(0);
 HCE->AddHitsCollection(HCID, hitCollection);
  

}


void Phantom::EndOfEvent(G4HCofThisEvent*)
{
G4Timer* timer = new G4Timer();
 timer->Start();
pCTRootPersistencyManager *InputPersistencyManager = pCTRootPersistencyManager::GetInstance();
pCTXMLInput = InputPersistencyManager->GetXMLInput();
float phantomPosZ = (pCTXMLInput->GetPosZ2()+pCTXMLInput->GetPosZ1())*0.5;
TVector3 midPos(-999,-999,-999);

G4int nHits = hitCollection->entries();
if(nHits==0) return;

float eventID = (float)G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
 
 std::map<G4int, vector< pair <TVector3,double> >> hitsMap;

for ( G4int h = 0 ; (h<nHits) ; ++h )
  {

    PhantomHit* hit = static_cast<PhantomHit*>( hitCollection->GetHit( h ) );
    G4int trackID = hit->GetTrackID();
    double edep = hit->GetEnergyDeposited();
    G4ThreeVector pos_ = hit->GetLocalPosition();
    TVector3 pos(pos_.getX(),pos_.getY(),pos_.getZ());

    if (abs(midPos.Z() - phantomPosZ) > abs(pos.Z()-phantomPosZ)) midPos=pos;
    //std::cout << "Phantom mid pos =  " << midPos.X() << ", " << midPos.Y() << ", " << midPos.Z()<< std::endl;

    // if already exists then add the new point and the edep 
    if(hitsMap.find(trackID) != hitsMap.end()){
      hitsMap[trackID].push_back( make_pair(pos,edep) );
    }
    else
      {// if it does not exist add it to the map and set the first point and edep 
	vector< pair <TVector3,double> > vect;
	vect.push_back( make_pair(pos,edep) );
	hitsMap[trackID]=vect;
      }
pCTRootPersistencyManager* persistencyManager = pCTRootPersistencyManager::GetInstance();
pCTEvent* pCT_Event = persistencyManager->GetEvent();
pCT_Event->SetPhantomHits(hitsMap);
pCT_Event->SetPhantomMidPos(midPos);
 
}
}
