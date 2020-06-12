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

class pCTSciDetConstructor{

public:
    pCTSciDetConstructor(G4String n)
      : fName(n){Init();};

    virtual ~pCTSciDetConstructor();

    G4String GetName(void) {return fName;}

    /// Construct and return a G4 volume for the object.
    G4LogicalVolume* GetPiece(void);

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

    G4Material* FindMaterial(G4String name);

    /// Set bar name
    void SetBarName(G4String name) {fBarName = name;}
    // Get bar name
    G4String GetBarName(void) {return fBarName;}

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

    double fCoatingThickness;

    // Name of bar logical volume
    G4String fBarName;

    void Init(void);

    pCTXML *fpCTXMLInput;
};

#endif
