#ifndef SiDetConstructor_h
#define SiDetrConstructor_h 1

#include "globals.hh"
#include <G4ThreeVector.hh>
class G4VPhysicalVolume;
class G4LogicalVolume;


#include "pCTRootPersistencyManager.hh"
#include "pCTXML.hh"
#include "pCTDetectorConstruction.hh"

class pCTSiDetConstructor{

public:
  pCTSiDetConstructor(G4String n)
      : fName(n){Init();};
  
    virtual ~pCTSiDetConstructor();

  G4String GetNamePlane(void) {return fName;}
  
   /// Construct and return a G4 volume for the object.                                                                                                                             
    G4LogicalVolume* GetPiece(void);


  // Plane Total Sizes                                                                                                                                                   
  void   SetPlaneColumns(double columns) {fPlaneColumns = columns;}
  double GetPlaneColumns(void)     {return fPlaneColumns;}
   
  void   SetPlaneRows(double rows) {fPlaneRows = rows;}
  double GetPlaneRows(void)     {return fPlaneRows;}

  void   SetPlaneThickness(double thick) {fPlaneThickness = thick;}
  double GetPlaneThickness(void)     {return fPlaneThickness;}

  
  // Epitaxial layer thickness
  void   SetEpiThickness(double thick) {fEpiThick = thick;}
  double GetEpiThickness(void)     {return fEpiThick;}
  // Substrate layer thickness
  void   SetSubThickness(double thick) {fSubThick = thick;}
  double GetSubThickness(void)     {return fSubThick;}

  /// Set the X position of the SiDet detector
  void SetPosX(double pos) {fPosX = pos;}
  /// Get the X position of the SiDet detector
  double GetPosX(void) {return fPosX;}

  /// Set the Y position of the SiDet detector                                                                                                                                    
  void SetPosY(double pos) {fPosY = pos;}
  /// Get the Y position of the Astra detector
  double GetPosY(void) {return fPosY;}

  /// Set the Z0 position of the SiDet detector
  void SetPosZ0(double pos) {fPosZ0 = pos;}
  /// Get the Z0 position of the SiDet detector
  double GetPosZ0(void) {return fPosZ0;}

  /// Set the Z1 position of the SiDet detector
  void SetPosZ1(double pos) {fPosZ1 = pos;}
  /// Get the Z1 position of the SiDet detector 
  double GetPosZ1(void) {return fPosZ1;}

  /// Set the Z2 position of the SiDet detector 
  void SetPosZ2(double pos) {fPosZ2 = pos;}
  /// Get the Z2 position of the SiDet detector
  double GetPosZ2(void) {return fPosZ2;}

  /// Set the Z3 position of the SiDet detector
  void SetPosZ3(double pos) {fPosZ3 = pos;}
  /// Get the Z3 position of the SiDet detector
  double GetPosZ3(void) {return fPosZ3;}

  
  /// Set plane name
  void SetPlaneName(G4String name) {fPlaneName = name;}
  // Get plane name
  G4String GetPlaneName(void) {return fPlaneName;}

  /// Set Sub name
  void SetSubName(G4String name) {fSubName = name;}
  // Get Sub name
  G4String GetSubName(void) {return fSubName;}

  /// Set Epi name
  void SetEpiName(G4String name) {fEpiName = name;}
  // Get Epi name 
  G4String GetEpiName(void) {return fEpiName;}

  void SetCopyNum(int num) {fCopyNum = num;}
  int GetCopyNum(void) {return fCopyNum;}

  
   pCTXML * GetpCTXML() const {return fpCTXMLInput;};








  
  /*
    virtual G4VPhysicalVolume* Construct();
      void ConstructSDandField();
  */



  
  protected:

  
 // The name of the plane
  G4String fName;
  // Copy number
  int fCopyNum;
  //Size of the plane
  double fPlaneRows;
  double fPlaneColumns;
  double fPlaneThickness;

  //Layers thickness
  double fEpiThick;
  double fSubThick;


  //Plane positions:

  double fPosX;
  double fPosY;
  double fPosZ0;
  double fPosZ1;
  double fPosZ2;
  double fPosZ3;

  G4String fPlaneName;
  G4String fEpiName;
  G4String fSubName;
  G4LogicalVolume* epiLogic;

 void Init(void);

 pCTXML *fpCTXMLInput;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......      
#endif

