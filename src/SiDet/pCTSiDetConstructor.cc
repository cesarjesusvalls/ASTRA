//
// C. Jesús Valls 
// cjesus@ifae.es
//

#include <globals.hh>
#include <G4Box.hh>
#include <G4Material.hh>
#include "G4NistManager.hh"
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4PVReplica.hh>
#include <G4PVPlacement.hh>
#include <G4Tubs.hh>

#include "pCTSiDetConstructor.hh"
#include "G4SDManager.hh"
#include "pCTXML.hh"
#include "pCTRootPersistencyManager.hh"

pCTSiDetConstructor::~pCTSiDetConstructor() {;}

void pCTSiDetConstructor::Init(void) {
  
    // Number planes
    //numPlanes = 1;


    SetPlaneWidth(fPlaneWidth);
    SetPlaneHeight(fPlaneHeight);
    //    SetThickness(fThickness);
    

    // Position of the center of the Scintillator Detector
    fPosX = 0.;
    fPosY = 0.;
    fPosZ0 = 0.;
    fPosZ1 = 0.;
    fPosZ2 = 0.;
  

    G4String namePlane = "Plane";
    G4String nameSub  = namePlane+"/Sub";
    G4String nameEpi   = namePlane+"/Epi";
    SetSubName(nameSub);
    SetEpiName(nameEpi);
    SetPlaneName(namePlane);
    //    AddConstructor(new pCTSiDetConstructor(GetNamePlane(), this)); 
}



G4LogicalVolume *pCTSiDetConstructor::GetPiece(void) {

  pCTRootPersistencyManager* InputPersistencyManager
    = pCTRootPersistencyManager::GetInstance();
  fpCTXMLInput = InputPersistencyManager->GetXMLInput();


  //Define the materials
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* Silicon = nist->FindOrBuildMaterial("G4_Si");
  G4Material* Air = nist->FindOrBuildMaterial("G4_AIR");

  fEpiThick =  fpCTXMLInput->GetEpiThickness();
  fSubThick =  fpCTXMLInput->GetSubThickness();

  
  // Set total size of the SiDet

  double width  = fPlaneWidth; 
  double height = fPlaneHeight; 
  double PlaneThickness = fEpiThick + fSubThick; 
  G4ThreeVector posEpi = G4ThreeVector(0*cm, 0*cm, -12.5*um);
  G4ThreeVector posSub = G4ThreeVector(0*cm, 0*cm, 37.5*um);

  SetPlaneWidth(width);
  SetPlaneHeight(height);
  SetPlaneThickness(PlaneThickness);

  G4bool checkOverlaps = true;
  //Don't know what this is:
  //  G4LogicalVolume* epi_logical = plane.GetPiece();

  // Make the logical volumes for the replicas
  // Build row of cubes along Z (replica of single cube along Z)

G4Box* planeShape =
    new G4Box(GetPlaneName(),
              0.5*GetPlaneWidth()*mm,
              0.5*GetPlaneHeight()*mm,
              2*0.5*GetPlaneThickness()*um);
 
  G4LogicalVolume *planeLogic
    = new G4LogicalVolume(planeShape,
			  Air,
			  GetNamePlane());

G4Box* epiShape =
    new G4Box(GetEpiName(),
	      0.5*GetPlaneWidth()*mm,
	      0.5*GetPlaneHeight()*mm,
	      0.5*GetEpiThickness()*um);//Maybe GetEpiThickness()
 epiLogic = //defined in hh to pass it to CMOSSD
    new G4LogicalVolume(epiShape, Silicon,"epi");  
 new G4PVPlacement(0, posEpi, epiLogic, GetEpiName(), planeLogic, false,0, checkOverlaps);
 
G4Box* subShape =
    new G4Box(GetSubName(),
              0.5*GetPlaneWidth()*mm,
              0.5*GetPlaneHeight()*mm,
              0.5*GetSubThickness()*um);//Maybe GetEpiThickness()                                                                                                                      
G4LogicalVolume* subLogic = new G4LogicalVolume(subShape,Silicon, "sub");
 new G4PVPlacement(0, posSub, subLogic, "sub", planeLogic, false, GetCopyNum(),checkOverlaps);




G4SDManager* SDman      = G4SDManager::GetSDMpointer();
CMOSSD*      aTrackerSD = (CMOSSD*)SDman->FindSensitiveDetector("CMOS");

epiLogic->SetSensitiveDetector( aTrackerSD );
planeLogic->SetVisAttributes(G4VisAttributes::Invisible);

 

return planeLogic;
}
