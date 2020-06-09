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

#include <G4Tubs.hh>

#include "pCTSciDetConstructor.hh"
#include "pCTSciBarConstructor.hh"

pCTSciDetConstructor::~pCTSciDetConstructor() {;}

void pCTSciDetConstructor::Init(void) {
    /// The edge of the bar
    fEdge = 1*CLHEP::cm;

    // Number of bars in a single layer 
    fCubeNumX = 1;
    fCubeNumY = 1;
    fCubeNumZ = 1;

    SetWidth( fCubeNumX * fEdge);
    SetHeight(fCubeNumY * fEdge);
    SetLength(fCubeNumZ * fEdge);

    // Position of the center of the Scintillator Detector
    fPosX = 0.;
    fPosY = 0.;
    fPosZ = 0.;

    G4String nameRepXYZ = "RepY"; // replica of single cube along Z
    G4String nameRepXZ  = nameRepXYZ+"/RepX"; // replica of single row  along X
    G4String nameRepZ   = nameRepXZ +"/RepZ"; // replica of single layer along Y
    G4String nameCube   = nameRepZ+"/CubeScint";
    
    SetNameRepXYZ(nameRepXYZ);
    SetNameRepXZ(nameRepXZ);
    SetNameRepZ(nameRepZ);
    SetNameCube(nameCube);
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

  double TotWidth  = fCubeNumX * fEdge; 
  double TotHeight = fCubeNumY * fEdge; 
  double TotLength = fCubeNumZ * fEdge; 

  SetWidth(TotWidth);
  SetLength(TotLength);
  SetHeight(TotHeight);

  // Build the plastic scintillator cube
  pCTSciBarConstructor& cube
    = Get<pCTSciBarConstructor>(GetNameCube());
  
  cube.SetBase(fEdge);
  cube.SetLength(fEdge);
  cube.SetHeight(fEdge);
  cube.SetCoatingThickness(0.1*CLHEP::mm);
  cube.SetGap(0.0*CLHEP::mm);

  G4LogicalVolume* cube_logical = cube.GetPiece();

  // Make the logical volumes for the replicas
  // Build row of cubes along Z (replica of single cube along Z)

  G4VSolid *repZ_solid 
    = new G4Box(GetName()+"/"+GetNameRepZ(), 
		cube.GetBase()/2, 
		cube.GetHeight()/2, 
		GetLength()/2); 
  G4LogicalVolume *repZ_logical 
    = new G4LogicalVolume(repZ_solid,
			  FindMaterial("Air"),
			  GetName()+"/"+GetNameRepZ());
  repZ_logical->SetVisAttributes(G4VisAttributes::Invisible);  
     
  // Build layer of cubes XZ (replica of single row of cubes along X)  

  G4VSolid *repXZ_solid 
    = new G4Box(GetName()+"/"+GetNameRepXZ(), 
		GetWidth()/2, 
		cube.GetHeight()/2, 
		GetLength()/2);   
  G4LogicalVolume *repXZ_logical 
    = new G4LogicalVolume(repXZ_solid,
			  FindMaterial("Air"),
			  GetName()+"/"+GetNameRepXZ());
  repXZ_logical->SetVisAttributes(G4VisAttributes::Invisible);  

  // Build box XYZ of cubes

  G4VSolid *repXYZ_solid 
    = new G4Box(GetName()+"/"+GetNameRepXYZ(), 
		GetWidth()/2, 
		GetHeight()/2, 
		GetLength()/2); 
  G4LogicalVolume *repXYZ_logical 
    = new G4LogicalVolume(repXYZ_solid,
			  FindMaterial("Air"),
			  GetName()+"/"+GetNameRepXYZ());
  repXYZ_logical->SetVisAttributes(G4VisAttributes::Invisible);  

  //
  // Place the cubes:
  // 1) replica of cubes along Z --> single row
  // 2) replica of rows along X --> single layer
  // 3) replica of layers along Y --> SciDet
  //
  
  new G4PVReplica(GetName()+"/"+GetNameRepZ(),        // its name
		  cube_logical,    // its logical volume
		  repZ_logical, // its mother volume
		  kZAxis,          // axis along replication applied
		  fCubeNumZ,       // number of replicated volumes
		  cube.GetLength() // width of single replica along axis 
		  );

  new G4PVReplica(GetName()+"/"+GetNameRepXZ(),        // its name
		  repZ_logical,  // its logical volume
		  repXZ_logical, // its mother volume
		  kXAxis,           // axis along replication applied
		  fCubeNumX,        // number of replicated volumes
		  cube.GetBase()    // width of single replica along axis 
		  );

  new G4PVReplica(GetName()+"/"+GetNameRepXYZ(),        // its name
		  repXZ_logical,  // its logical volume
		  repXYZ_logical, // its mother volume
		  kYAxis,            // axis along replication applied
		  fCubeNumY,         // number of replicated volumes
		  cube.GetHeight()   // width of single replica along axis 
		  );

  return repXYZ_logical;
}
