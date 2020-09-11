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
#include "SciDetSD.hh"
#include "phantom.hh"
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

#include "pCTRootGeometryManager.hh"

#include "pCTRootPersistencyManager.hh"
#include "pCTSciDetConstructor.hh"
#include "pCTSiDetConstructor.hh"

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
    SDman->AddNewDetector(new SciDetSD("SciDetSensDet"));
    SDman->AddNewDetector(new Phantom("PhantomSD"));
    pCTRootPersistencyManager *InputPersistencyManager = pCTRootPersistencyManager::GetInstance();
    pCTXMLInput = InputPersistencyManager->GetXMLInput();

    // Get nist material manager
    G4NistManager* nist = G4NistManager::Instance();
    // Option to switch on/off checking of volumes overlaps
    G4bool checkOverlaps = true;

    // Envelope parameters
    G4double env_sizeXY = 20*cm, env_sizeZ = 100*cm;
    G4Material* env_mat = nist->FindOrBuildMaterial("G4_AIR");

    // World
    G4double world_sizeXY         = 1.2*env_sizeXY;
    G4double world_sizeZ          = 1.2*env_sizeZ;
    G4Material* world_mat         = nist->FindOrBuildMaterial("G4_AIR");
    G4Box* solidWorld             = new G4Box("World",0.5*world_sizeXY,0.5*world_sizeXY,0.5*world_sizeZ);
    G4LogicalVolume* logicWorld   = new G4LogicalVolume(solidWorld,world_mat,"World");                  
    G4VPhysicalVolume* physWorld  = new G4PVPlacement(0,G4ThreeVector(),logicWorld,"World",0,false,0,checkOverlaps);
     
    //_____ ENVELOPE _____
    G4Box* solidEnv               = new G4Box("Envelope",0.5*env_sizeXY,0.5*env_sizeXY,0.5*env_sizeZ);
    G4LogicalVolume* logicEnv     = new G4LogicalVolume(solidEnv,env_mat,"Envelope");
    new G4PVPlacement(0,G4ThreeVector(),logicEnv,"Envelope",logicWorld,false,0,checkOverlaps);
    //___________________

    //_____ PHANTOM _____
    G4ThreeVector phantomPos      = G4ThreeVector(0,0,-8.5*cm);            // TODO: get from XML.
    //G4Material*   phantom_mat     = nist->FindOrBuildMaterial("G4_Cu");
    G4Material*   phantom_mat     = FindMaterial("PlasticScintillator");
    
    // G4Ellipsoid*  solidEllipsoid  = new G4Ellipsoid ("ellipsoid", 1.5*mm, 2.5*mm , 0.6*mm,0,0); 
    // G4Tubs*       solidCylinder   = new G4Tubs("cylinder", 0, 4.*mm, 0.6*mm, 0,2*M_PI);    
    // G4Box*        solidBox        = new G4Box("box",4,4,0.35);
    // G4VSolid* solidUnion_EC       = new G4UnionSolid("ellipsoid+cylinder",  solidEllipsoid,solidCylinder,0,G4ThreeVector(0,0.,0.7*mm));
    // G4VSolid* solidIntersec_BC    = new G4IntersectionSolid("boc+cylinder", solidCylinder,solidBox,0,G4ThreeVector(4*mm,2.*mm,0.05*mm));
    // G4VSolid* solidUnion          = new G4UnionSolid("phantomShape",        solidUnion_EC,solidIntersec_BC,0,G4ThreeVector(0,0.,1.25*mm));

    // G4Ellipsoid*  solidEllipsoid  = new G4Ellipsoid ("ellipsoid", 1.5*mm, 2.5*mm , 6.4*mm,0,0); 
    // G4Tubs*       solidCylinder   = new G4Tubs("cylinder", 0, 4.*mm, 4.6*mm, 0,2*M_PI);    
    // G4Box*        solidBox        = new G4Box("box",10,10,0.25);
    // G4VSolid* solidUnion_EC       = new G4UnionSolid("ellipsoid+cylinder",  solidEllipsoid,solidCylinder,0,G4ThreeVector(0,0.,0.*mm));
    // G4VSolid* solidIntersec_BC    = new G4IntersectionSolid("boc+cylinder", solidCylinder,solidBox,0,G4ThreeVector(4.*mm,4.*mm,4.*mm));
    // G4VSolid* solidUnion          = new G4UnionSolid("phantomShape",        solidUnion_EC,solidIntersec_BC,0,G4ThreeVector(0,0.,1.25*mm));
    // G4LogicalVolume* logicPhantom = new G4LogicalVolume(solidUnion, phantom_mat,"Phantom"); 


    //if(pCTXMLInput->GetUsePhantom()){
    //   for(int star(0); star<9; star++){
    //        // G4Box*        solidBox1        = new G4Box("box",2*mm,   0.5*mm,  1*(star+1)*mm);
    //       // G4Box*        solidBox2        = new G4Box("box",0.5*mm, 2*mm, 1*(star+1)*mm);
    //        // G4VSolid*     crossSolid       = new G4UnionSolid("boc+cylinder", solidBox1,solidBox2,0,G4ThreeVector(0.*mm,0.*mm,0.*mm));
    //      G4Tubs*       crossSolid   = new G4Tubs("cylinder", 0, 4.*mm, 1*(star+1)*mm, 0,2*M_PI);   
    //      G4LogicalVolume* logicPhantom  = new G4LogicalVolume(crossSolid, phantom_mat,"Phantom"); 
    //      logicPhantom->SetVisAttributes(G4Colour(0.6, 0.6, 0.0));
    //        int xCoor = star%3;
    //        int yCoor = ((int) (star/3))%3;
    //        new G4PVPlacement(0,G4ThreeVector((1-xCoor)*1.5*cm,(1-yCoor)*1.5*cm, -2.5*cm-2*(star+1)*mm)  ,logicPhantom,"phantom",logicEnv,false,0,checkOverlaps);
    //    }
    /*
    G4Box*        solidBox1        = new G4Box("box",1.2*mm,0.3*mm,5*mm);
    G4Box*        solidBox2        = new G4Box("box",0.3*mm, 1.2*mm,5*mm);
    G4VSolid*     crossSolid       = new G4UnionSolid("crossBox", solidBox1,solidBox2,0,G4ThreeVector(0.*mm,0.*mm,0.*mm));
    */
    G4Box* solidBox = new G4Box("box", 100*mm,100*mm,2*cm);
    G4LogicalVolume* logicPhantom  = new G4LogicalVolume(solidBox, phantom_mat,"Phantom"); 
    Phantom*      PhantomSD = (Phantom*)SDman->FindSensitiveDetector("PhantomSD");
    logicPhantom->SetSensitiveDetector(PhantomSD);
    logicPhantom->SetVisAttributes(G4Colour(0.6, 0.6, 0.0));
    if(pCTXMLInput->GetUsePhantom()){
      /*
        new G4PVPlacement(0,G4ThreeVector(0.2*cm,0.2*cm,-3*cm)  ,logicPhantom,"phantom",logicEnv,false,0,checkOverlaps);
        new G4PVPlacement(0,G4ThreeVector(-0.2*cm,0.2*cm,-3*cm) ,logicPhantom,"phantom",logicEnv,false,0,checkOverlaps);
        new G4PVPlacement(0,G4ThreeVector(0.2*cm,-0.2*cm,-3*cm) ,logicPhantom,"phantom",logicEnv,false,0,checkOverlaps);
        new G4PVPlacement(0,G4ThreeVector(-0.2*cm,-0.2*cm,-3*cm),logicPhantom,"phantom",logicEnv,false,0,checkOverlaps);
        new G4PVPlacement(0,G4ThreeVector(0.*cm,0.*cm,-3*cm)    ,logicPhantom,"phantom",logicEnv,false,0,checkOverlaps);
      
      */
      new G4PVPlacement(0,phantomPos    ,logicPhantom,"phantom",logicEnv,false,0,checkOverlaps);
    }
    //___________________

    pCTSiDetConstructor* fSiDetConstructor = new pCTSiDetConstructor("SiliconDet");
    G4String nameSiliconDet = fSiDetConstructor->GetPlaneName();
    fSiDetConstructor->SetPlaneColumns(pCTXMLInput->GetPlaneColumns()*0.04);
    fSiDetConstructor->SetPlaneRows(pCTXMLInput->GetPlaneRows()*0.036);
    fSiDetConstructor->SetEpiThickness(pCTXMLInput->GetEpiThickness());
    fSiDetConstructor->SetSubThickness(pCTXMLInput->GetSubThickness());
    logicPlane = fSiDetConstructor->GetPiece();

    G4ThreeVector pos0   = G4ThreeVector((pCTXMLInput->GetPosX())*cm, (pCTXMLInput->GetPosY())*cm, (pCTXMLInput->GetPosZ0())*cm);
    G4ThreeVector pos1   = G4ThreeVector((pCTXMLInput->GetPosX())*cm, (pCTXMLInput->GetPosY())*cm, (pCTXMLInput->GetPosZ1())*cm);
    G4ThreeVector pos2   = G4ThreeVector((pCTXMLInput->GetPosX())*cm, (pCTXMLInput->GetPosY())*cm, (pCTXMLInput->GetPosZ2())*cm);
    G4ThreeVector pos3   = G4ThreeVector((pCTXMLInput->GetPosX())*cm, (pCTXMLInput->GetPosY())*cm, (pCTXMLInput->GetPosZ3())*cm);

    G4ThreeVector SciDet;
    if(pCTXMLInput->GetUse4thCMOS()) SciDet = G4ThreeVector(0, 0, pos3.getZ() + 0.5*cm + pCTXMLInput->GetSciDetNLayers()*pCTXMLInput->GetSciDetBarZ()*mm/2);
    else                             SciDet = G4ThreeVector(0, 0, pos2.getZ() + 0.5*cm + pCTXMLInput->GetSciDetNLayers()*pCTXMLInput->GetSciDetBarZ()*mm/2);

   if(pCTXMLInput->GetUseCMOS()){
		new G4PVPlacement(0,pos0,logicPlane,nameSiliconDet,logicEnv,false,0,checkOverlaps);
		new G4PVPlacement(0,pos1,logicPlane,nameSiliconDet,logicEnv,false,1,checkOverlaps);
		new G4PVPlacement(0,pos2,logicPlane,nameSiliconDet,logicEnv,false,2,checkOverlaps);
   		if(pCTXMLInput->GetUse4thCMOS()) new G4PVPlacement(0,pos3,logicPlane,nameSiliconDet,logicEnv,false,3,checkOverlaps);
   }

    //______ SciDet ______
    pCTSciDetConstructor* fSciDetConstructor = new pCTSciDetConstructor("SciDet");
    G4String nameSciDet = fSciDetConstructor->GetName();
    fSciDetConstructor->SetNLayers(pCTXMLInput->GetSciDetNLayers());
    fSciDetConstructor->SetNBars(pCTXMLInput->GetSciDetNBars());
    fSciDetConstructor->SetBarX(pCTXMLInput->GetSciDetBarX()*mm);
    fSciDetConstructor->SetBarY(pCTXMLInput->GetSciDetBarY()*mm);
    fSciDetConstructor->SetBarZ(pCTXMLInput->GetSciDetBarZ()*mm);
    logicSciDet = fSciDetConstructor->GetPiece(); 
    if(pCTXMLInput->GetUseSciDet()){
        new G4PVPlacement(0,SciDet,logicSciDet,nameSciDet,logicEnv,false,0,checkOverlaps);
    }

    pCTRootGeometryManager::Get()->Update(physWorld, true);
    pCTRootGeometryManager::Get()->Export("geometry.root");
    //_____________________

    return physWorld;
}

// this function is looked for in newer versions of geant4 to work with the SD
void pCTDetectorConstruction::ConstructSDandField()
{

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
