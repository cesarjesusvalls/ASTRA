//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file pCTDetectorConstruction.cc
/// \brief Implementation of the pCTDetectorConstruction class

#include "pCTDetectorConstruction.hh"
#include "G4SDManager.hh"
#include "CMOSSD.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Ellipsoid.hh"
#include "G4Tubs.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnionSolid.hh"
#include "G4IntersectionSolid.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

pCTDetectorConstruction::pCTDetectorConstruction()
  : G4VUserDetectorConstruction()//,  fScoringVolume(0)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

pCTDetectorConstruction::~pCTDetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* pCTDetectorConstruction::Construct()
{  
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  
  // Envelope parameters
  //
  G4double env_sizeXY = 20*cm, env_sizeZ = 30*cm;
  G4Material* env_mat = nist->FindOrBuildMaterial("G4_AIR");
   
  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //     
  // World
  //
  G4double world_sizeXY = 1.2*env_sizeXY;
  G4double world_sizeZ  = 1.2*env_sizeZ;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size
      
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name
                                   
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
                     
  //     
  // Envelope
  //  
  G4Box* solidEnv =    
    new G4Box("Envelope",                    //its name
        0.5*env_sizeXY, 0.5*env_sizeXY, 0.5*env_sizeZ); //its size
      
  G4LogicalVolume* logicEnv =                         
    new G4LogicalVolume(solidEnv,            //its solid
                        env_mat,             //its material
                        "Envelope");         //its name
               
  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(),         //at (0,0,0)
                    logicEnv,                //its logical volume
                    "Envelope",              //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking


  
  ///Phantom creation
 G4ThreeVector phantomPos = G4ThreeVector(0,0,-3*cm);
  G4Material* phantom_mat = nist->FindOrBuildMaterial("G4_Cu");

  G4Ellipsoid* solidEllipsoid = new  G4Ellipsoid ("ph_ellipsoid", 1.5*mm, 2.5*mm , 0.6*mm,0,0);
  //G4LogicalVolume* logicEllipsoid =    new G4LogicalVolume(solidEllipsoid, phantom_mat, "Envelope");         //its name 
 // new G4PVPlacement(0,G4ThreeVector(),logicEllipsoid,"ph_ellipsoid",logicEnv,false,0,checkOverlaps);			

  G4Tubs* solidCylinder =  new G4Tubs("cylinder", 0, 4.*mm, 0.6*mm, 0,2*M_PI);
 //G4LogicalVolume* logicCylinder =    new G4LogicalVolume(solidCylinder, phantom_mat, "Envelope");         //its name
 G4Box* solidBox = new G4Box("box",4,4,0.25);
 
 G4VSolid* solidUnion_EC = new G4UnionSolid("Ellipsoid+Cylinder", solidEllipsoid,solidCylinder,0,G4ThreeVector(0,0.,0.7*mm));
 G4VSolid* solidIntersec_BC = new G4IntersectionSolid("Box+Cylinder", solidCylinder,solidBox,0,G4ThreeVector(4*mm,2.*mm,0.05*mm));
 G4VSolid* solidUnion = new G4UnionSolid("Finalform", solidUnion_EC,solidIntersec_BC,0,G4ThreeVector(0,0.,1.25*mm));
 


 G4LogicalVolume* logicPhantom = new G4LogicalVolume(solidUnion, phantom_mat,"Phantom");
 
 new G4PVPlacement(0,phantomPos,logicPhantom,"phantom",logicEnv,false,0,checkOverlaps);
 
 


 //     
  // 3 detectors configuration (25um epitaxial 75um substrate +100um OPTIONAL)
  //
G4Material* Silicon = nist->FindOrBuildMaterial("G4_Si");
//G4Material* Aluminium = nist->FindOrBuildMaterial("G4_Al");
G4ThreeVector pos1Epi = G4ThreeVector(0, 0*cm, -12.00*cm);
//G4ThreeVector pos1Epi = G4ThreeVector(0, 0*cm, -10.00*cm);
G4ThreeVector pos1Sub = G4ThreeVector(0, 0*cm, -11.995*cm);
//G4ThreeVector pos1Sub = G4ThreeVector(0, 0*cm, -9.995*cm);
//G4ThreeVector pos1Al = G4ThreeVector(0, 0*cm, -10.99*cm);
//G4ThreeVector pos2Epi = G4ThreeVector(0, 0*cm, -5.*cm);
G4ThreeVector pos2Epi = G4ThreeVector(0, 0*cm, -2.*cm);
//G4ThreeVector pos2Sub = G4ThreeVector(0, 0*cm, -4.995*cm);
G4ThreeVector pos2Sub = G4ThreeVector(0, 0*cm, -1.995*cm);
//G4ThreeVector pos2Al = G4ThreeVector(0, 0*cm, 0.01*cm);
//G4ThreeVector pos3Epi = G4ThreeVector(0, 0*cm, 0.*cm);
G4ThreeVector pos3Epi = G4ThreeVector(0, 0*cm, 3.*cm);
//G4ThreeVector pos3Sub = G4ThreeVector(0, 0*cm, 0.005*cm);
G4ThreeVector pos3Sub = G4ThreeVector(0, 0*cm, 3.005*cm);
G4ThreeVector RTpos = G4ThreeVector(0, 0*cm, 10.5*cm);

//G4ThreeVector pos4Al = G4ThreeVector(0, 0*cm, 5.01*cm);
// Detector Shapes
// G4double Si_epi_dx = 0.448*cm;
//G4double Si_sub_dx = 2*cm;
// G4double Si_dy = 0.8064*cm;      

/////////////
///////////////test for a single flavour detector or 5x5cm2 (if 2.5*..)
///////////////
 G4double Si_epi_dx = 4*0.448*cm;
 G4double Si_sub_dx = 2*cm; 
 G4double Si_dy = 0.8064*cm;

 //////////////////////////////////// Comune thiness
G4double Si_epi_dz  = 0.025*mm;
//G4double Al_dz  = 0.1*mm;
G4double Si_sub_dz  = 0.075*mm;

//First detector
G4Box* epiShape =
      new G4Box("epiShape",                      //its name
                0.5*Si_epi_dx,
                0.5*Si_dy,
                0.5*Si_epi_dz); //its size
                
 epiLogic =
    new G4LogicalVolume(epiShape,         //its solid
                        Silicon,          //its material
                        "epi");           //its name
  
  new G4PVPlacement(0,                       //no rotation
                    pos1Epi,                    //at position
                    epiLogic,             //its logical volume
                    "epi",                //its name
                    logicEnv,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking
                
G4Box* subShape = new G4Box("subShape", 0.5*Si_sub_dx, 0.5*Si_dy, 0.5*Si_sub_dz);
    G4LogicalVolume* subLogic = new G4LogicalVolume(subShape,Silicon, "sub");
    new G4PVPlacement(0, //no rotation
              pos1Sub, // at position
              subLogic, //its logical volume
              "sub", // its name
              logicEnv, //its mother volume
              false, //no boolean operation
              0, //copy number
              checkOverlaps); //overlaps checking
    
    
    
        
// 2nd Detector
    
new G4PVPlacement(0, pos2Epi, epiLogic, "epi", logicEnv, false,1, checkOverlaps);          //overlaps checking

new G4PVPlacement(0, pos2Sub, subLogic, "sub", logicEnv, false, 1,checkOverlaps); //checking overlaps

    
// 3rd Detector
        
new G4PVPlacement(0, pos3Epi, epiLogic, "epi", logicEnv, false,2, checkOverlaps);          //overlaps checking

new G4PVPlacement(0, pos3Sub, subLogic, "sub", logicEnv, false, 2,checkOverlaps); //checking overlaps

       


// Range Telescope

 G4Box* RTShape = new G4Box("RT",2.5*cm,2.5*cm,4.5*cm);
 logicRT = new G4LogicalVolume(RTShape,Silicon, "RT");
 new G4PVPlacement(0, RTpos, logicRT, "RT", logicEnv, false, 3,checkOverlaps); 


 
    // Set first Detector as scoring volume
  //
 //fScoringVolume = logicRT;

  //
  //always return the physical World
  //
  return physWorld;
}

// this function is looked for in newer versions of geant4 to work with the SD

void pCTDetectorConstruction::ConstructSDandField()

{
  G4cout << "Constructing SDs" << G4endl;
 G4SDManager* SDman = G4SDManager::GetSDMpointer();
 CMOSSD* sensitive_det = new CMOSSD("CMOS");
  SDman->AddNewDetector(sensitive_det);
  epiLogic->SetSensitiveDetector(sensitive_det);      
  logicRT->SetSensitiveDetector(sensitive_det);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
