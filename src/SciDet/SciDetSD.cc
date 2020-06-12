
#include "SciDetSD.hh"
#include "SciDetBar.hh"

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

SciDetSD::SciDetSD(G4String SDname)
  : G4VSensitiveDetector(SDname)
{
  collectionName.insert(SDname);
  HCID = -1;
  pEvtID = -1;
}

SciDetSD::~SciDetSD(){}

G4bool SciDetSD::ProcessHits(G4Step *step, G4TouchableHistory *)
{

  pCTRootPersistencyManager *InputPersistencyManager = pCTRootPersistencyManager::GetInstance();
  pCTXMLInput = InputPersistencyManager->GetXMLInput();

  G4TouchableHandle touchable = step->GetPreStepPoint()->GetTouchableHandle();
  G4int particle_id = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
  G4double edep = step->GetTotalEnergyDeposit();
  G4int copyNumber  = touchable->GetReplicaNumber();

  G4int layerID       = (int) copyNumber/pCTXMLInput->GetSciDetNLayers();
  G4int barID         = copyNumber - layerID;

  //G4cout << "plane: " << planeCopyNo << ", volName: " << touchable->GetVolume()->GetName() << G4endl;
  G4cout << "layer: " << layerID << ", bar: " << barID << ", copyNumber: " << copyNumber <<G4endl;

   // get step points in world coordinate system
  G4ThreeVector point1 = step->GetPreStepPoint()->GetPosition();
  G4ThreeVector point2 = step->GetPostStepPoint()->GetPosition();
  G4ThreeVector worldPosition = point1 + G4UniformRand()*(point2 - point1);   
  G4ThreeVector localPosition = touchable->GetHistory()->GetTopTransform().TransformPoint(worldPosition);  
        
  // SciDetHit::SciDetHit(const G4int copyID, const G4int layerID,const G4int barID,
  //                    const G4int orientation, const G4bool isScint, const G4ThreeVector position,
  //                    const G4bool edep, const G4int particle_ID)

  // create a hit and populate it with information
  SciDetHit* hit = new SciDetHit(copyNumber,layerID,barID, layerID%2, false, localPosition, edep, particle_id);
  
  hitCollection->insert(hit);
  
  return true;
}

void SciDetSD::Initialize(G4HCofThisEvent* HCE)
{

  // ------------------------------
  // -- Creation of the collection
  // ------------------------------
  // -- collectionName[0] is "SciDetHitCollection", as declared in constructor
  hitCollection = new SciDetHitCollection(SensitiveDetectorName, collectionName[0]);

  // ----------------------------------------------------------------------------
  // -- and attachment of this collection to the "Hits Collection of this Event":
  // ----------------------------------------------------------------------------
  // -- To insert the collection, we need to get an index for it. This index
  // -- is unique to the collection. It is provided by the GetCollectionID(...)
  // -- method (which calls what is needed in the kernel to get this index).
    
  if (HCID<0) HCID = GetCollectionID(0); // <<-- this is to get an ID for collectionName[0]
  HCE->AddHitsCollection(HCID, hitCollection);
 
}

#include "G4Timer.hh"

void SciDetSD::EndOfEvent(G4HCofThisEvent*)
{
    G4Timer* timer = new G4Timer();
    timer->Start();
    
    G4int nHits = hitCollection->entries(); 
    if(nHits==0) return;
    
    G4double pitchX = 40.0*um;
    G4double pitchY = 36.0*um;
    G4double threshold = 550;// 3.3*150; //DUT to e- conversion for TPAC = 3.3e. Basic threshold = 150 DUT
    
    float eventID = (float)G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    G4cout << "EndOfEvent " << eventID << G4endl;
  
    // test output of hits
    G4cout << "\nSciDetSD::EndOfEvent method of SD `" << GetName() << "' called." << G4endl;
    G4cout << "\thitCollection " << collectionName[0] << " has " << nHits << " hits" << G4endl;
    
    // container to add the edep for multiple hits on a strip (if this happens)
    std::map<std::pair<G4int, std::pair<G4int,G4int> >, G4double> Digits;
    
    for ( G4int h = 0 ; (h<nHits) ; ++h )
    {
            const SciDetHit* hit = static_cast<const SciDetHit*>( hitCollection->GetHit( h ) );
            
            //Uncomment this line if you want to record only
            //primary energy depositions
            //if ( hit->GetIsPrimary() == false ) continue;
            
            G4ThreeVector particle_pos = hit->GetLocalPosition();
            const G4int planeNumber = hit->GetLayerID();

            ///////
            //////// test for a single flavour detector
            ////////
            
            G4int xBar = (int) ((particle_pos.getX()+4*2.240*mm) / pitchX);
            G4int yBar = (int) ((particle_pos.getY()+4.032*mm) / pitchY);  
            G4int particleID = hit->GetParticleID();
            
            if(planeNumber==3){
            xBar =0;
            yBar =0;
            }
            
            std::pair<G4int,G4int> Bar(xBar,yBar);
            std::pair<G4int, std::pair<G4int, G4int> > hit_key(planeNumber, Bar); // make a pair of layer number and strip number
            std::map<std::pair<G4int, std::pair<G4int,G4int> >, G4double>::iterator it = Digits.find(hit_key);
            if(Digits.find(hit_key) != Digits.end()) // if already exists then add an edep
                (*it).second += hit->GetEnergyDeposited();
            else                               // if it does not exist add it to the map and set edep to edep
                Digits[hit_key] = hit->GetEnergyDeposited() ;
  }
        
    G4cout << "\n\t----- Total Energy Deposited -----" << G4endl;
    
    std::map<G4int, std::vector< SciDetBar* > > Counter;
    // now loop through the map and check if above threshold
    std::map<std::pair<G4int, std::pair<G4int,G4int> >, G4double>::iterator it;
    for(it=Digits.begin(); it!=Digits.end(); it++)
    {       
        G4int Plane = (*it).first.first;
        G4double edep = (*it).second;
        G4double nElectrons = (edep/eV) / 3.6;
                
        //
        // do the charge spreading
        //
        if( nElectrons > threshold )
        {               
            SciDetBar* pixel_temp = new SciDetBar();
            pixel_temp->SetPlaneNumber(Plane);
            pixel_temp->SetBarIndex((*it).first.second);

            std::map<G4int, std::vector< SciDetBar* > >::iterator it2 = Counter.find(Plane);
            if(Counter.find(Plane) != Counter.end())
            {
                (*it2).second.push_back(pixel_temp);
            }
            else
            {
                  std::vector<SciDetBar* > spam;
                  spam.push_back(pixel_temp);
                  Counter[Plane] = spam;
            }
        }               
    }
    
    unsigned short int nPlanes = Counter.size();
    
    // pCTRootPersistencyManager* persistencyManager = pCTRootPersistencyManager::GetInstance();
    // pCTEvent* pCT_Event = persistencyManager->GetEvent();
    // pCT_Event->SetBarHits(Counter);
    //std::cout << "Nhits:" << pCT_Event->GetBarHits().size() << std::endl;

    std::map<G4int, std::vector< SciDetBar*> >::iterator it2;
    for(it2=Counter.begin(); it2!=Counter.end(); it2++)
    {
        unsigned short int Plane = (*it2).first;
        unsigned short int nHitsInPlane = (*it2).second.size();
            G4cout << "oOoOoPlane " << Plane << " has " << nHitsInPlane << " pixels above threshold (" << threshold << "e-)" << G4endl;

        for(unsigned index(0); index<nHitsInPlane; index++)
        {
          unsigned short int X = (*it2).second.at(index)->GetX();
          unsigned short int Y = (*it2).second.at(index)->GetY();
          
          G4cout << X << ", " << Y << ", "<< Plane <<"," <<G4endl;
        }
    }
}
