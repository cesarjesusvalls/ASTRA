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

#include <G4Tubs.hh>

#include "pCTSciDetConstructor.hh"
#include "pCTSciBarConstructor.hh"

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

    // change this... somehow
    G4String nameRepXYZ = "RepY"; // replica of single bar along Z
    G4String nameRepXZ  = nameRepXYZ+"/RepX"; // replica of single row  along X
    G4String nameRepZ   = nameRepXZ +"/RepZ"; // replica of single layer along Y
    G4String nameCube   = nameRepZ+"/CubeScint";
    
    SetNameRepXYZ(nameRepXYZ);
    SetNameRepXZ(nameRepXZ);
    SetNameRepZ(nameRepZ);
    SetNameBar(nameCube);

    AddConstructor(new pCTSciBarConstructor(GetNameBar(), this)); 
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
  SciDet_logicalV->SetVisAttributes(G4Colour(1.0, 0.0, 0.0));

  // create a single bar
  pCTSciBarConstructor& bar
    = Get<pCTSciBarConstructor>(GetNameBar());
  
  bar.SetBarX(fBarX);
  bar.SetBarY(fBarY);
  bar.SetBarZ(fBarZ);
  bar.SetCoatingThickness(0.1*CLHEP::mm);
  bar.SetGap(0.0*CLHEP::mm);

  // create the logic volume of a bar
  G4LogicalVolume* bar_logical = bar.GetPiece();

  // create the logic volume of a layer of bars
  G4VSolid* XY_plane = new G4Box(GetName()+"/"+GetNameRepZ(), GetDetX()/2, bar.GetBarY()/2, GetBarZ()/2); 
  G4LogicalVolume* XY_plane_logical = new G4LogicalVolume(XY_plane, FindMaterial("Air"), GetName()+"/"+GetNameRepZ());
  XY_plane_logical->SetVisAttributes(G4VisAttributes::Invisible);

  new G4PVReplica(GetName(),                          // its name
      bar_logical,                                    // its logical volume
      XY_plane_logical,                               // its mother volume
      kXAxis,                                         // axis along replication applied
      fNBars,                                         // number of replicated volumes
      bar.GetBarX()                                   // width of single replica along axis 
      );

  for(int layer=0; layer<GetNLayers(); layer++){
      TString layerName = "/Plane";
      layerName += layer;
      G4cout << layerName << G4endl;

      G4RotationMatrix rotM;
      if(layer%2==0) rotM.rotateZ(90.0 * CLHEP::deg);
    
      G4cout << (GetNLayers()-layer*2-1)*bar.GetBarZ()/2 << G4endl;
      new G4PVPlacement(G4Transform3D(rotM, G4ThreeVector(0, 0, (GetNLayers()-layer*2-1)*bar.GetBarZ()/2 )),XY_plane_logical,GetName()+layerName.Data(),SciDet_logicalV,false,0);
  }

  return SciDet_logicalV;
}
