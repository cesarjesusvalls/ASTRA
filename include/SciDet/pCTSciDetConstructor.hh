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

    G4Material* FindMaterial(G4String m);

    /// Construct and return a G4 volume for the object.
    virtual G4LogicalVolume* GetPiece(void);

    // Set size of cube edges  
    void SetEdge(double w) {fEdge = w;}
    // Get size of cube edge 
    double GetEdge(void) {return fEdge;}

    /// Set the width of the SciDet detector bounding box "along" the X axis
    void SetWidth(double w) {fWidth = w;}
    /// Get the width of the SciDet detector
    double GetWidth(void) {return fWidth;}

    /// Set the height of the SciDet detector bounding box "along" the Y axis  
    void SetHeight(double w) {fHeight = w;}
    /// Get the height of the SciDet detector
    double GetHeight(void) {return fHeight;}

    /// Set the length of the SciDet detector bounding box "along" the beam axis (the
    /// Z axis).  
    void SetLength(double w) {fLength = w;}
    /// Get the length of the SciDet detector
    double GetLength(void) {return fLength;}

    // Set number of cubes along X
    void SetCubeNumX(int num) {fCubeNumX = num;}
    int GetCubeNumX() {return fCubeNumX;}

    // Set number of cubes along Y
    void SetCubeNumY(int num) {fCubeNumY = num;}
    int GetCubeNumY() {return fCubeNumY;}

    // Set number of cubes along Z
    void SetCubeNumZ(int num) {fCubeNumZ = num;}
    int GetCubeNumZ() {return fCubeNumZ;}

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
    virtual void SetNameCube  (G4String name){fNameCube  =name;};

    virtual G4String GetNameRepXYZ(){return fNameRepXYZ;};
    virtual G4String GetNameRepXZ (){return fNameRepXZ;};
    virtual G4String GetNameRepZ  (){return fNameRepZ;};
    virtual G4String GetNameCube  (){return fNameCube;};

    pCTXML * GetpCTXML() const {return fpCTXMLInput;};

private:
    // The name of the SciBar.
    G4String fName;

    // Size of the SciDet
    double fWidth;
    double fHeight;
    double fLength;

    /// The edge of a cube of the SciDet detector 
    double fEdge;

    // Number of cubes along each axis 
    int fCubeNumX;
    int fCubeNumY;
    int fCubeNumZ;

    // Position of the center of the SciDet detector
    double fPosX;
    double fPosY;
    double fPosZ;

    // Detector names
    G4String fNameRepXYZ; // Replica of layers along Y
    G4String fNameRepXZ;  // Replica of rows along X
    G4String fNameRepZ;   // Replica of cubes along Z
    G4String fNameCube;   // Single cube

    void Init(void);

    pCTXML *fpCTXMLInput;
};

#endif
