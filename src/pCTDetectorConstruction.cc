//
// C. Jesús Valls 
// cjesus@ifae.es
//
// M. Granado-González
// MMG998@student.bham.ac.uk
//

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
#include <G4VisAttributes.hh>

#include "pCTRootPersistencyManager.hh"
#include "pCTSciDetConstructor.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

pCTDetectorConstruction::pCTDetectorConstruction()
  : G4VUserDetectorConstruction() //,  fScoringVolume(0)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

pCTDetectorConstruction::~pCTDetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* pCTDetectorConstruction::Construct()
{  
    this->DefineMaterials();

    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    fCMOSSD = new CMOSSD("CMOS");
    SDman->AddNewDetector(fCMOSSD);

    pCTRootPersistencyManager *InputPersistencyManager = pCTRootPersistencyManager::GetInstance();
    pCTXMLInput = InputPersistencyManager->GetXMLInput();

    // Get nist material manager
    G4NistManager* nist = G4NistManager::Instance();
    // Option to switch on/off checking of volumes overlaps
    G4bool checkOverlaps = true;

    // Envelope parameters
    G4double env_sizeXY = 20*cm, env_sizeZ = 30*cm;
    G4Material* env_mat = nist->FindOrBuildMaterial("G4_AIR");

    // World
    G4double world_sizeXY         = 1.2*env_sizeXY;
    G4double world_sizeZ          = 1.2*env_sizeZ;
    G4Material* world_mat         = nist->FindOrBuildMaterial("G4_AIR");
    G4Box* solidWorld             = new G4Box("World",0.5*world_sizeXY,0.5*world_sizeXY,0.5*world_sizeZ);
    G4LogicalVolume* logicWorld   = new G4LogicalVolume(solidWorld,world_mat,"World");                  
    G4VPhysicalVolume* physWorld  = new G4PVPlacement(0,G4ThreeVector(),logicWorld,"World",0,false,0,checkOverlaps);
     
    //_____ PHANTOM _____
    G4Box* solidEnv               = new G4Box("Envelope",0.5*env_sizeXY,0.5*env_sizeXY,0.5*env_sizeZ);
    G4LogicalVolume* logicEnv     = new G4LogicalVolume(solidEnv,env_mat,"Envelope");
    new G4PVPlacement(0,G4ThreeVector(),logicEnv,"Envelope",logicWorld,false,0,checkOverlaps);
    //___________________

    //_____ PHANTOM _____
    G4ThreeVector phantomPos      = G4ThreeVector(0,0,-3*cm);            // TODO: get from XML.
    G4Material*   phantom_mat     = nist->FindOrBuildMaterial("G4_Cu");
    G4Ellipsoid*  solidEllipsoid  = new G4Ellipsoid ("ellipsoid", 1.5*mm, 2.5*mm , 0.6*mm,0,0);	
    G4Tubs*       solidCylinder   = new G4Tubs("cylinder", 0, 4.*mm, 0.6*mm, 0,2*M_PI);
    G4Box*        solidBox        = new G4Box("box",4,4,0.25);
    G4VSolid* solidUnion_EC       = new G4UnionSolid("ellipsoid+cylinder",  solidEllipsoid,solidCylinder,0,G4ThreeVector(0,0.,0.7*mm));
    G4VSolid* solidIntersec_BC    = new G4IntersectionSolid("boc+cylinder", solidCylinder,solidBox,0,G4ThreeVector(4*mm,2.*mm,0.05*mm));
    G4VSolid* solidUnion          = new G4UnionSolid("phantomShape",        solidUnion_EC,solidIntersec_BC,0,G4ThreeVector(0,0.,1.25*mm));
    G4LogicalVolume* logicPhantom = new G4LogicalVolume(solidUnion, phantom_mat,"Phantom"); 
    new G4PVPlacement(0,phantomPos,logicPhantom,"phantom",logicEnv,false,0,checkOverlaps);
    //___________________

    // 3 detectors configuration (25um epitaxial 75um substrate +100um OPTIONAL)
    G4Material* Silicon   = nist->FindOrBuildMaterial("G4_Si");
    G4ThreeVector pos1Epi = G4ThreeVector(0, 0*cm, -12.00*cm);
    G4ThreeVector pos1Sub = G4ThreeVector(0, 0*cm, -11.995*cm);
    G4ThreeVector pos2Epi = G4ThreeVector(0, 0*cm, -2.*cm);
    G4ThreeVector pos2Sub = G4ThreeVector(0, 0*cm, -1.995*cm);
    G4ThreeVector pos3Epi = G4ThreeVector(0, 0*cm, 3.*cm);
    G4ThreeVector pos3Sub = G4ThreeVector(0, 0*cm, 3.005*cm);
    G4ThreeVector RTpos   = G4ThreeVector(0, 0*cm, 10.5*cm);

    ///////////////test for a single flavour detector or 5x5cm2 (if 2.5*..)
    G4double Si_epi_dx = 4*0.448*cm;
    G4double Si_sub_dx = 2*cm; 
    G4double Si_dy = 0.8064*cm;

    //////////////////////////////////// Comune thiness
    G4double Si_epi_dz  = 0.025*mm;
    G4double Si_sub_dz  = 0.075*mm;

    G4Box* epiShape = new G4Box("epiShape",0.5*Si_epi_dx,0.5*Si_dy,0.5*Si_epi_dz);              
    epiLogic = new G4LogicalVolume(epiShape,Silicon,"epi");
    
    G4Box* subShape = new G4Box("subShape", 0.5*Si_sub_dx, 0.5*Si_dy, 0.5*Si_sub_dz);
    G4LogicalVolume* subLogic = new G4LogicalVolume(subShape,Silicon, "sub");
     
    // 1st Detecotr
    new G4PVPlacement(0, pos1Epi, epiLogic, "epi", logicEnv, false, 0, checkOverlaps);
    new G4PVPlacement(0, pos1Sub, subLogic, "sub", logicEnv, false, 0, checkOverlaps);

    // 2nd Detector
    new G4PVPlacement(0, pos2Epi, epiLogic, "epi", logicEnv, false, 1, checkOverlaps);
    new G4PVPlacement(0, pos2Sub, subLogic, "sub", logicEnv, false, 1, checkOverlaps);
        
    // 3rd Detector
    new G4PVPlacement(0, pos3Epi, epiLogic, "epi", logicEnv, false, 2, checkOverlaps);
    new G4PVPlacement(0, pos3Sub, subLogic, "sub", logicEnv, false, 2,checkOverlaps);
         
    epiLogic->SetVisAttributes(G4Colour(0.0, 1.0, 0.0));
    subLogic->SetVisAttributes(G4Colour(1.0, 0.0, 0.0));

    // Range Telescope
    // G4Box* RTShape = new G4Box("RT",2.5*cm,2.5*cm,4.5*cm);
    // logicRT = new G4LogicalVolume(RTShape,Silicon, "RT");
    // new G4PVPlacement(0, RTpos, logicRT, "RT", logicEnv, false, 3,checkOverlaps); 

    //______ SciDet ______
    pCTSciDetConstructor* fSciDetConstructor = new pCTSciDetConstructor("SciDet");
    G4String nameSciDet = fSciDetConstructor->GetName();
    fSciDetConstructor->SetNLayers(20);
    fSciDetConstructor->SetNBars(25);
    fSciDetConstructor->SetBarX(2*mm);
    fSciDetConstructor->SetBarY(50*mm);
    fSciDetConstructor->SetBarZ(2*mm);
    logicSciDet = fSciDetConstructor->GetPiece(); 
    new G4PVPlacement(0,RTpos,logicSciDet,nameSciDet,logicEnv,false,0,checkOverlaps);
    //_____________________

    return physWorld;
}

// this function is looked for in newer versions of geant4 to work with the SD
void pCTDetectorConstruction::ConstructSDandField()
{
    G4cout << "Constructing SDs" << G4endl;
    epiLogic->SetSensitiveDetector(fCMOSSD);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Material *pCTDetectorConstruction::FindMaterial(G4String name)
{
    G4Material *material = G4Material::GetMaterial(name, true);
    return material;
}

void pCTDetectorConstruction::DefineMaterials()
{
    G4double density,temperature, pressure,fractionmass;
    G4String name;
    G4int nel;

    G4NistManager *nistMan = G4NistManager::Instance();

    G4Element *elH = nistMan->FindOrBuildElement(1);
    G4Element *elC = nistMan->FindOrBuildElement(6);
    G4Element *elN = nistMan->FindOrBuildElement(7);
    G4Element *elO = nistMan->FindOrBuildElement(8);

    //Air
    density = 1.29 * CLHEP::mg / CLHEP::cm3;
    pressure = 1 * CLHEP::atmosphere;
    temperature = 293.15 * CLHEP::kelvin;
    G4Material *air = new G4Material(name = "Air", density,
                                     nel = 2, kStateGas, temperature,
                                     pressure);
    air->AddElement(elN, fractionmass = 70 * CLHEP::perCent);
    air->AddElement(elO, fractionmass = 30 * CLHEP::perCent);

    //PlasticScintillator
    density = 1.032 * CLHEP::g / CLHEP::cm3;
    G4Material *plasticScintillator = new G4Material(name = "PlasticScintillator", density, nel = 2);
    plasticScintillator->AddElement(elC, 8);
    plasticScintillator->AddElement(elH, 8);
}
