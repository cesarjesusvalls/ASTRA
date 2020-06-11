
#include "CMOSSD.hh"
#include "CMOSPixel.hh"
//#include "HistoManager.hh"

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

#include "stdio.h"


CMOSSD::CMOSSD(G4String SDname)
  : G4VSensitiveDetector(SDname)
{
  // 'collectionName' is a protected data member of base class G4VSensitiveDetector.
  // Here we declare the name of the collection we will be using.
  collectionName.insert(SDname);
  //collectionName.insert("CMOSHitCollection");
 
  // Note that we may add as many collection names we would wish: ie
  // a sensitive detector can have many collections.
  HCID = -1;
  pEvtID = -1;
}

CMOSSD::~CMOSSD()
{
  
        G4cout << "BLAH" << " deleting SD called "<< GetName() << G4endl;
      	fout.close();
	rename(fname.c_str(), ("Test_"+GetName()+".txt").c_str()); 
	//	rename(fname.c_str(), (fHistoManager->GetFileName()+"_"+GetName()+".bin").c_str());
}

G4bool CMOSSD::ProcessHits(G4Step *step, G4TouchableHistory *)
{
  // step is guaranteed to be in Strip volume : no need to check for volume
  
  G4TouchableHandle touchable = step->GetPreStepPoint()->GetTouchableHandle();
  G4int particle_id = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
  // energy deposit in this step 
  G4double edep = step->GetTotalEnergyDeposit();
  G4int planeCopyNo = touchable->GetReplicaNumber();
   // get step points in world coordinate system
  G4ThreeVector point1 = step->GetPreStepPoint()->GetPosition();
  G4ThreeVector point2 = step->GetPostStepPoint()->GetPosition();
	G4ThreeVector worldPosition = point1 + G4UniformRand()*(point2 - point1);	
	// convert this to local position within the strip
  G4ThreeVector localPosition = touchable->GetHistory()->GetTopTransform().TransformPoint(worldPosition);  
		
  // create a hit and populate it with information
  CMOSHit* hit = new CMOSHit(planeCopyNo,localPosition,particle_id);
  hit->SetEnergyDeposited(edep);
  
  // finally store the hit in the collection
  //electron filter
  //if(particle_id != 11)
    hitCollection->insert(hit);
  
  return true;
}

void CMOSSD::Initialize(G4HCofThisEvent* HCE)
{

  // ------------------------------
  // -- Creation of the collection
  // ------------------------------
  // -- collectionName[0] is "CMOSHitCollection", as declared in constructor
  //hitCollection = new CMOSHitCollection(GetName(), collectionName[0]);
  hitCollection = new CMOSHitCollection(SensitiveDetectorName, collectionName[0]);

  // ----------------------------------------------------------------------------
  // -- and attachment of this collection to the "Hits Collection of this Event":
  // ----------------------------------------------------------------------------
  // -- To insert the collection, we need to get an index for it. This index
  // -- is unique to the collection. It is provided by the GetCollectionID(...)
  // -- method (which calls what is needed in the kernel to get this index).
  //static G4int HCID = -1;
    
  if (HCID<0) HCID = GetCollectionID(0); // <<-- this is to get an ID for collectionName[0]
  HCE->AddHitsCollection(HCID, hitCollection);
 
}

#include "G4Timer.hh"

void CMOSSD::EndOfEvent(G4HCofThisEvent*)
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
	G4cout << "\nCMOSSD::EndOfEvent method of SD `" << GetName() << "' called." << G4endl;
	G4cout << "\thitCollection " << collectionName[0] << " has " << nHits << " hits" << G4endl;
	
	// container to add the edep for multiple hits on a strip (if this happens)
	// first.first = planeNumber
	// first.second = stripNumber
	// second.first = localPosition in strip width
	// second.second = total energy deposited
	std::map<std::pair<G4int, std::pair<G4int,G4int> >, G4double> Digits;
	
	for ( G4int h = 0 ; (h<nHits) ; ++h )
	{
			const CMOSHit* hit = static_cast<const CMOSHit*>( hitCollection->GetHit( h ) );
            
			//Uncomment this line if you want to record only
			//primary energy depositions
			//if ( hit->GetIsPrimary() == false ) continue;
			
			G4ThreeVector particle_pos = hit->GetLocalPosition();
			const G4int planeNumber = hit->GetPlaneNumber();
       			//G4int xPixel = (int) ((particle_pos.getX()+2.240*mm) / pitchX);
			//G4int yPixel = (int) ((particle_pos.getY()+4.032*mm) / pitchY);

			///////
			//////// test for a single flavour detector
			////////
			
			G4int xPixel = (int) ((particle_pos.getX()+4*2.240*mm) / pitchX);
			G4int yPixel = (int) ((particle_pos.getY()+4.032*mm) / pitchY);  
			G4int particleID = hit->GetParticleID();
			
			if(planeNumber==3){
			xPixel =0;
			yPixel =0;
			}
			
			
			std::pair<G4int,G4int> Pixel(xPixel,yPixel);
			std::pair<G4int, std::pair<G4int, G4int> > hit_key(planeNumber, Pixel); // make a pair of layer number and strip number
			std::map<std::pair<G4int, std::pair<G4int,G4int> >, G4double>::iterator it = Digits.find(hit_key);
			if(Digits.find(hit_key) != Digits.end()) // if already exists then add an edep
				(*it).second += hit->GetEnergyDeposited();
			else                               // if it does not exist add it to the map and set edep to edep
				Digits[hit_key] = hit->GetEnergyDeposited() ;
  }
		
	G4cout << "\n\t----- Total Energy Deposited -----" << G4endl;
	
	std::map<G4int, std::vector< CMOSPixel* > > Counter;
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
		  //		  G4cout << "\tPlane = " << Plane << " Pixel (" << X << ", " << Y << ") has " << nElectrons << " electrons liberated"<< G4endl;
			CMOSPixel* pixel_temp = new CMOSPixel();
			pixel_temp->SetPlaneNumber(Plane);
			pixel_temp->SetPixelIndex((*it).first.second);
			pixel_temp->SetElectronsLiberated(nElectrons);

			std::map<G4int, std::vector< CMOSPixel* > >::iterator it2 = Counter.find(Plane);
 			if(Counter.find(Plane) != Counter.end())
			{
 				(*it2).second.push_back(pixel_temp);
			}
 			else
			{
			      std::vector<CMOSPixel* > spam;
			      spam.push_back(pixel_temp);
			      Counter[Plane] = spam;
			}
		}				
	}
	
// 	write out to the binary file
 	const int evtID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
	
	// if pEvtID  ==-1 then no runs have occured before and we need to make the file
	// if pEVTID > current event ID then a new run has started and we need a new file
	if(pEvtID>evtID && fout.is_open())
	  fout.close();
	
	if(pEvtID==-1 || pEvtID>evtID)
	{	  
	  //  fHistoManager = HistoManager::GetHistoManager();
	  //fname = fHistoManager->GetFileName()+"_"+GetName()+".bin";
	  fname = "Test_"+GetName()+".txt"; 
	  //	  fout.open(fname.c_str(), std::ios::out | std::ios::binary);// | std::ios::app);
	  //std::ofstream fout;
	  fout.open(fname.c_str(), std::ios::out); //| std::ios::app);  
	}

 	unsigned short int nPlanes = Counter.size();
	
	//	fout.write((const char*)&evtID, sizeof(evtID));
	//fout.write((const char*)&nPlanes, sizeof(nPlanes));
	//fout << "Event ID: " << evtID << " Number of planes: " << nPlanes << std::endl;

 	pCTRootPersistencyManager* persistencyManager = pCTRootPersistencyManager::GetInstance();
	pCTEvent* pCT_Event = persistencyManager->GetEvent();
	pCT_Event->SetPixelHits(Counter);

	std::cout << "Nhits:" << pCT_Event->GetPixelHits().size() << std::endl;

 	std::map<G4int, std::vector< CMOSPixel*> >::iterator it2;
 	for(it2=Counter.begin(); it2!=Counter.end(); it2++)
 	{
//		if(fHistoManager)
//			fHistoManager->FillProfile("HitsPerLayer", (*it2).first, (*it2).second);
 		unsigned short int Plane = (*it2).first;
 		unsigned short int nHitsInPlane = (*it2).second.size();
      		G4cout << "Plane " << Plane << " has " << nHitsInPlane << " pixels above threshold (" << threshold << "e-)" << G4endl;
		//std::cout<<"Plane " << Plane << " has " << nHitsInPlane << " pixels above threshold (" << threshold << "e-)" << G4endl;
		//fout.write((const char*)&Plane, sizeof(Plane));
		//fout.write((const char*)&nHitsInPlane, sizeof(nHitsInPlane));
		//fout <<"Plane = " << Plane << " Number of hits: " << nHitsInPlane << G4endl;
		
		//fout << "X " << "Y " << "Charghe [e]" << G4endl;
 		for(unsigned index(0); index<nHitsInPlane; index++)
		{
		  unsigned short int X = (*it2).second.at(index)->GetX();
		  unsigned short int Y = (*it2).second.at(index)->GetY();
		  unsigned int e = (*it2).second.at(index)->GetElectronsLiberated();
		  
		  //fout.write((const char*)&X, sizeof(X));
		  //fout.write((const char*)&Y, sizeof(Y));
		  //fout.write((const char*)&e, sizeof(e));

		  fout << X << ", " << Y << ", "<< Plane <<", " << e <<"," <<G4endl;
		}
 	}
	
	pEvtID = evtID;
	
	timer->Stop();
	G4cout << "real time elapsed in CMOSSD::EndOfAction() = " << timer->GetRealElapsed() << G4endl; 
	
  // -- we could have attached the collection to the G4HCofThisEvent in this
  // -- method as well (instead of Initialize).
}
