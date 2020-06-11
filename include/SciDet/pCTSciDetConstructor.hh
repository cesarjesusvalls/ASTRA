//
// C. Jesús Valls 
// cjesus@ifae.es
//

#ifndef SCIDET_CONSTRUCTOR_HH_SEEN
#define SCIDET_CONSTRUCTOR_HH_SEEN

#include <G4ThreeVector.hh>
class G4LogicalVolume;

#include "pCTRootPersistencyManager.hh"
#include "pCTXML.hh"
#include "pCTDetectorConstruction.hh"   
#include "pCTConstructor.hh"   

class pCTSciDetConstructor : public pCTConstructor {

public:
    pCTSciDetConstructor(G4String n, pCTDetectorConstruction* c)
      : pCTConstructor(n,c) {Init();};
    pCTSciDetConstructor(G4String n, pCTConstructor* p)
      : pCTConstructor(n,p) {Init();};
    virtual ~pCTSciDetConstructor();

    G4String GetName(void) {return fName;}

    /// Construct and return a G4 volume for the object.
    virtual G4LogicalVolume* GetPiece(void);

    // Detector Total Size (X,Y,Z)
    void   SetDetX(double x) {fDetX = x;}
    double GetDetX(void)     {return fDetX;}
    void   SetDetY(double y) {fDetY = y;}
    double GetDetY(void)     {return fDetY;}
    void   SetDetZ(double z) {fDetZ = z;}
    double GetDetZ(void)     {return fDetZ;}

    // Sci Bars Total Size (X,Y,Z)
    void   SetBarX(double x) {fBarX = x;}
    double GetBarX(void)     {return fBarX;}
    void   SetBarY(double y) {fBarY = y;}
    double GetBarY(void)     {return fBarY;}
    void   SetBarZ(double z) {fBarZ = z;}
    double GetBarZ(void)     {return fBarZ;}

    /// Set the number N of layers along the SciDet  
    void SetNLayers(double N) {fNLayers = N;}
    /// Get the number N of layers along the SciDet  
    int GetNLayers(void) {return fNLayers;}

    /// Set the number N of bars in each layer  
    void SetNBars(double N) {fNBars = N;}
    /// Get the number N of bars in each layer   
    int GetNBars(void) {return fNBars;}

    /// Set the X position of the SciDet detector     
    void SetPosX(double pos) {fPosX = pos;}
    /// Get the X position of the SciDet detector  
    double GetPosX(void) {return fPosX;}

    /// Set the Y position of the SciDet detector       
    void SetPosY(double pos) {fPosX = pos;}
    /// Get the Y position of the SciDet detector   
    double GetPosY(void) {return fPosY;}

    /// Set the Z position of the SciDet detector 
    void SetPosZ(double pos) {fPosZ = pos;}
    // Get the Z position of the SciDet detector 
    double GetPosZ(void) {return fPosZ;}

    // Set and Get the detectors name
    virtual void SetNameRepXYZ(G4String name){fNameRepXYZ=name;};
    virtual void SetNameRepXZ (G4String name){fNameRepXZ =name;};
    virtual void SetNameRepZ  (G4String name){fNameRepZ  =name;};
    virtual void SetNameBar  (G4String name){fNameBar  =name;};

    virtual G4String GetNameRepXYZ(){return fNameRepXYZ;};
    virtual G4String GetNameRepXZ (){return fNameRepXZ;};
    virtual G4String GetNameRepZ  (){return fNameRepZ;};
    virtual G4String GetNameBar  (){return fNameBar;};

    pCTXML * GetpCTXML() const {return fpCTXMLInput;};

private:
    // The name of the SciDet.
    G4String fName;

    // Size of the SciDet
    double fDetX;
    double fDetY;
    double fDetZ;

    // Size of the SciBars
    double fBarX;
    double fBarY;
    double fBarZ;
    
    /// The number of layers
    int fNLayers;

    /// The number of bars per layer
    int fNBars;

    // Position of the center of the SciDet detector
    double fPosX;
    double fPosY;
    double fPosZ;

    // Detector names
    G4String fNameRepXYZ; // Replica of layers along Y
    G4String fNameRepXZ;  // Replica of rows along X
    G4String fNameRepZ;   // Replica of cubes along Z
    G4String fNameBar;   // Single cube

    void Init(void);

    pCTXML *fpCTXMLInput;
};

#endif
