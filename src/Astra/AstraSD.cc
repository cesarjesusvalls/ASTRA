
#include "AstraSD.hh"
#include "AstraBar.hh"

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

#include "pCTXML.hh"
#include "pCTRootPersistencyManager.hh"

#include "stdio.h"

AstraSD::AstraSD(G4String SDname)
  : G4VSensitiveDetector(SDname)
{
    collectionName.insert(SDname);
    HCID = -1;
    pEvtID = -1;
}

AstraSD::~AstraSD(){}

G4bool AstraSD::ProcessHits(G4Step *step, G4TouchableHistory *)
{
    pCTRootPersistencyManager *InputPersistencyManager = pCTRootPersistencyManager::GetInstance();
    pCTXMLInput = InputPersistencyManager->GetXMLInput();


    G4TouchableHandle touchable = step->GetPreStepPoint()->GetTouchableHandle();
    G4int particle_id           = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
    G4int track_id              = step->GetTrack()->GetTrackID ();
    G4double edep               = step->GetTotalEnergyDeposit();
    G4int copyNumber            = touchable->GetReplicaNumber();
    G4int layerID               = (int) copyNumber/pCTXMLInput->GetAstraNBars();
    G4int barID                 = copyNumber - pCTXMLInput->GetAstraNBars()*(layerID);

    //G4cout << layerID << ", " << barID << ", " << copyNumber << ", " << (int) copyNumber/pCTXMLInput->GetAstraNBars() << ", " << copyNumber/pCTXMLInput->GetAstraNBars()<< endl;
    //G4cout << step->GetTrack()->GetTrackID () << "," << step->GetTrack()->GetParentID () << G4endl;

    // get step points in world coordinate system
    G4ThreeVector point1        = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector point2        = step->GetPostStepPoint()->GetPosition();
    G4ThreeVector worldPosition = point1 + G4UniformRand()*(point2 - point1); 

    double  t1        = step->GetPreStepPoint()->GetGlobalTime();
    double  t2        = step->GetPostStepPoint()->GetGlobalTime();
    double hit_time   = t1 + G4UniformRand()*(t2 - t1); 

    if(edep) InputPersistencyManager->GetTimeVsEdepHisto()->Fill(hit_time,edep);
    G4ThreeVector localPosition = touchable->GetHistory()->GetTopTransform().TransformPoint(worldPosition);  
      
    // create a hit and populate it with information
    AstraHit* hit = new AstraHit(copyNumber,layerID,barID, layerID%2, false, edep, particle_id, track_id);
    hitCollection->insert(hit);

    return true;
}

void AstraSD::Initialize(G4HCofThisEvent* HCE)
{
    // ------------------------------
    // -- Creation of the collection
    // ------------------------------
    hitCollection = new AstraHitCollection(SensitiveDetectorName, collectionName[0]);
    if (HCID<0) HCID = GetCollectionID(0);
    HCE->AddHitsCollection(HCID, hitCollection);
 
}

#include "G4Timer.hh"

void AstraSD::EndOfEvent(G4HCofThisEvent*)
{   
    G4int nHits = hitCollection->entries(); 
    if(nHits==0) return;
    // container to add the edep for multiple hits on a bar (if this happens)
    std::map<G4int, AstraHit*> hitsMap;    
    for ( G4int h = 0 ; (h<nHits) ; ++h )
    {
        AstraHit* hit = static_cast<AstraHit*>( hitCollection->GetHit( h ) );
        G4int copyID = hit->GetCopyID();

        // here it may be interesting to add pID and tID as fContributors .. ?
        if(hitsMap.find(copyID) != hitsMap.end()){// if already exists then add an edep
            hitsMap[copyID]->AddTrackID(hit->GetTrackID());
            hitsMap[copyID]->SetEnergyDeposited(hitsMap[copyID]->GetEnergyDeposited()+hit->GetEnergyDeposited());
        }
        else{// if it does not exist add it to the map and set edep to edep
            hitsMap[copyID] = hit;//hit->GetEnergyDeposited();
            hitsMap[copyID]->AddTrackID(hit->GetTrackID());
        }
    }
    std::vector< AstraHit* > mergedHits;
    std::map<G4int, AstraHit*>::iterator it;
    for(it=hitsMap.begin(); it!=hitsMap.end(); it++) mergedHits.push_back((*it).second);
    pCTRootPersistencyManager* persistencyManager = pCTRootPersistencyManager::GetInstance();
    pCTEvent* pCT_Event = persistencyManager->GetEvent();
    pCT_Event->SetAstraHits(mergedHits);
}
