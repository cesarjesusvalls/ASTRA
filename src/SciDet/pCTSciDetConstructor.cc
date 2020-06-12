//
// C. Jesús Valls 
// cjesus@ifae.es
//

#include <globals.hh>
#include <G4Box.hh>
#include <G4Material.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4PVReplica.hh>
#include <G4PVPlacement.hh>
#include <G4SubtractionSolid.hh>

#include <G4Tubs.hh>
#include "pCTSciDetConstructor.hh"

#include "G4SDManager.hh"
// used to keep a list of SD logical volumes
#include "G4RegionStore.hh"
#include <G4Region.hh> 
#include "CMOSSD.hh"

pCTSciDetConstructor::~pCTSciDetConstructor() {;}

void pCTSciDetConstructor::Init(void) {
    
    // Number layers 
    fNLayers = 1;

    // Number of bars in a single layer 
    fNBars = 1;

    // Default bar dimensions
    fBarX  = 1*CLHEP::mm;
    fBarZ  = 1*CLHEP::mm;
    fBarY = 1*CLHEP::mm;

    fCoatingThickness = 0.1*CLHEP::mm;

    // Set total size of the SciDet
    double TotWidth  = fBarX * fNBars; 
    double TotHeight = fBarY; 
    double TotLength = fBarZ * fNLayers; 

    SetDetX(TotWidth);
    SetDetY(TotHeight);
    SetDetZ(TotLength);

    // Position of the center of the Scintillator Detector (this is set externally at the time of the G4Placement)
    fPosX = 0.;
    fPosY = 0.;
    fPosZ = 0.;

    SetBarName(GetName()+"Bar");
    //AddConstructor(new pCTSciBarConstructor(GetBarName(), this)); 
}

G4Material* pCTSciDetConstructor::FindMaterial(G4String name) {
    G4Material* material = G4Material::GetMaterial(name,true);
    return material;
}

G4LogicalVolume *pCTSciDetConstructor::GetPiece(void) {

  pCTRootPersistencyManager* InputPersistencyManager
    = pCTRootPersistencyManager::GetInstance();
  fpCTXMLInput = InputPersistencyManager->GetXMLInput();

  // Set total size of the SciDet
  double TotWidth  = fBarX * fNBars; 
  double TotHeight = fBarY; 
  double TotLength = fBarZ * fNLayers; 

  SetDetX(TotWidth);
  SetDetY(TotHeight);
  SetDetZ(TotLength);

  // create the SciDet logical volume
  G4VSolid* SciDet_solidV = new G4Box(GetName(), GetDetX()/ 2., GetDetY()/ 2., GetDetZ()/ 2.);
  G4LogicalVolume* SciDet_logicalV = new G4LogicalVolume(SciDet_solidV,FindMaterial("Air"),GetName());
  //SciDet_logicalV->SetVisAttributes(G4Colour(1.0, 0.0, 0.0));
  SciDet_logicalV->SetVisAttributes(G4VisAttributes::Invisible);

  G4SDManager* SDman      = G4SDManager::GetSDMpointer();
  CMOSSD*      aTrackerSD = (CMOSSD*)SDman->FindSensitiveDetector("CMOS");

  for(int nlayer=0; nlayer<GetNLayers(); nlayer++){
      for(int nbar=0; nbar<GetNBars(); nbar++){
          TString barName = "/Layer";
          barName += nlayer;
          barName += "/Bar";
          barName += nbar;
          G4cout << barName << G4endl;

          G4VSolid* barSolid               = new G4Box(GetName()+barName.Data()+"/Scint",GetBarX()/2,GetBarY()/2,GetBarZ()/2);
          G4VSolid* scintSolid             = new G4Box(GetName()+barName.Data()+"/Coating",GetBarX()/2-fCoatingThickness,GetBarY()/2-fCoatingThickness,GetBarZ()/2-fCoatingThickness);
          G4SubtractionSolid* coatingSolid = new G4SubtractionSolid("Coating", barSolid, scintSolid);

          G4LogicalVolume *scintVolume   = new G4LogicalVolume(scintSolid,  FindMaterial("PlasticScintillator"),GetName()+barName.Data()+"/Scint");
          G4LogicalVolume *coatingVolume = new G4LogicalVolume(coatingSolid,FindMaterial("PlasticScintillator"),GetName()+barName.Data()+"/Coating");

          scintVolume->SetSensitiveDetector( aTrackerSD ); 
          coatingVolume->SetSensitiveDetector( aTrackerSD ); 
          scintVolume->SetVisAttributes(G4Color(0.0,0.8,0.8,0.));
          //coatingVolume->SetVisAttributes(G4Color(0.0,0.0,0.8,0.));
          coatingVolume->SetVisAttributes(G4VisAttributes::Invisible);

          G4RotationMatrix rotM;
          if(nlayer%2==0){
              rotM.rotateZ(90.0 * CLHEP::deg);
              new G4PVPlacement(G4Transform3D(rotM, G4ThreeVector(0, (GetNBars()-nbar*2-1)*fBarX/2, (GetNLayers()-nlayer*2-1)*fBarZ/2)),scintVolume,  GetName()+barName.Data()+"/Scint",SciDet_logicalV,false,nlayer*GetNBars()+nbar);
              new G4PVPlacement(G4Transform3D(rotM, G4ThreeVector(0, (GetNBars()-nbar*2-1)*fBarX/2, (GetNLayers()-nlayer*2-1)*fBarZ/2)),coatingVolume,GetName()+barName.Data()+"/Coating",SciDet_logicalV,false,nlayer*GetNBars()+nbar);
          }
          else{
              new G4PVPlacement(G4Transform3D(rotM, G4ThreeVector((GetNBars()-nbar*2-1)*fBarX/2, 0, (GetNLayers()-nlayer*2-1)*fBarZ/2)),scintVolume,  GetName()+barName.Data()+"/Scint"  ,SciDet_logicalV,false,nlayer*GetNBars()+nbar);
              new G4PVPlacement(G4Transform3D(rotM, G4ThreeVector((GetNBars()-nbar*2-1)*fBarX/2, 0, (GetNLayers()-nlayer*2-1)*fBarZ/2)),coatingVolume,GetName()+barName.Data()+"/Coating",SciDet_logicalV,false,nlayer*GetNBars()+nbar);
          }
      }
  }

  return SciDet_logicalV;
}
