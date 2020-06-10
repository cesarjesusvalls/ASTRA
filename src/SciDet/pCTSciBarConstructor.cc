//
// C. Jesús Valls 
// cjesus@ifae.es
//

#include <globals.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4PVPlacement.hh>

#include <G4RotationMatrix.hh>

#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Trap.hh>
#include <G4SubtractionSolid.hh>

#include "G4SDManager.hh"
#include "pCTSciBarConstructor.hh"

// used to keep a list of SD logical volumes
#include "G4RegionStore.hh"
#include <G4Region.hh> 

pCTSciBarConstructor::~pCTSciBarConstructor() {;}

void pCTSciBarConstructor::Init() {
  
    SetBarX(1*CLHEP::cm);
    SetBarY(1*CLHEP::cm);
    SetBarZ(1*CLHEP::cm);
    SetCoatingThickness(0*CLHEP::mm);
    SetGap(0.0*CLHEP::mm);
    SetVisibility(false);
    SetCoatingMaterial("PlasticScintillator");
    SetScintillatorMaterial("PlasticScintillator"); 

}

G4LogicalVolume* pCTSciBarConstructor::GetPiece(void) {
  
  pCTRootPersistencyManager* InputPersistencyManager
    = pCTRootPersistencyManager::GetInstance();
  pCTXMLInput = InputPersistencyManager->GetXMLInput();

  G4VSolid* cube = NULL;
  G4VSolid* extrusion = NULL;
  G4VSolid* scintillator = NULL;

  //G4cout << "SuperFGD Cube base = " << GetBarX() << G4endl;
  //G4cout << "SuperFGD Cube height = " << GetBarY() << G4endl;
  //G4cout << "SuperFGD Cube length = " << GetBarZ() << G4endl;
  //G4cout << "SuperFGD Hole radius = " << GetHoleRadius() << G4endl;
  
  G4VisAttributes *visAtt_Scint = new G4VisAttributes();
  visAtt_Scint->SetColor(0.0,0.8,0.8,0.); // gray
  visAtt_Scint->SetForceSolid(true);
  G4VisAttributes *visAtt_Coat = new G4VisAttributes();
  visAtt_Coat->SetColor(1.0,1.0,1.0); // white
  visAtt_Coat->SetForceSolid(true);

  cube = new G4Box(GetName(),
		   GetBarX()/2,
		   GetBarY()/2,
		   GetBarZ()/2
		   );

  extrusion = new G4Box(GetName()+"/Extrusion",
			GetBarX()/2,
			GetBarY()/2,
			GetBarZ()/2
			);
  
  scintillator = new G4Box(GetName()+"/Extrusion/Core",
   			   GetBarX()/2-GetCoatingThickness(),
  			   GetBarY()/2-GetCoatingThickness(),
  			   GetBarZ()/2-GetCoatingThickness()
			   );

  // logical volumes
  G4LogicalVolume* cubeVolume
    = new G4LogicalVolume(cube,
			  FindMaterial("Air"),
			  GetName());
  
  G4LogicalVolume* extrusionVolume
    = new G4LogicalVolume(extrusion,
			  FindMaterial(GetCoatingMaterial()),
			  GetName()+"/Extrusion");
			  
  G4LogicalVolume *scintVolume;
  scintVolume = new G4LogicalVolume(scintillator,
				    FindMaterial(GetScintillatorMaterial()),
				    GetName()+"/Extrusion/Core");
    
  // // Define the volume of plastic scintillator as sensitive detector
  // scintVolume->SetSensitiveDetector( aTrackerSD ); 

  // Place the scintillator inside the extrusion volume
  new G4PVPlacement(0,G4ThreeVector(0,0,0),scintVolume,GetName()+"/Extrusion/Core",extrusionVolume,false,0);                  // copy number

  // Place the extrusion inside the cube volume
  new G4PVPlacement(0,G4ThreeVector(0,0,0),extrusionVolume,GetName()+"/Extrusion",cubeVolume,false,0);                  // copy number

  cubeVolume      ->SetVisAttributes(G4VisAttributes::Invisible);
  //extrusionVolume ->SetVisAttributes(visAtt_Coat); 
  extrusionVolume ->SetVisAttributes(G4VisAttributes::Invisible); 
  scintVolume     ->SetVisAttributes(visAtt_Scint);


  return cubeVolume;
}
