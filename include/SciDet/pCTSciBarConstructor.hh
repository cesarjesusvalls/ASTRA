//
// C. Jesús Valls 
// cjesus@ifae.es
//

#ifndef SCIDET_CUBESCINT_CONSTRUCTOR_HH_SEEN
#define SCIDET_CUBESCINT_CONSTRUCTOR_HH_SEEN

#include <G4ThreeVector.hh>

class G4LogicalVolume;
                  
#include "pCTRootPersistencyManager.hh"
#include "pCTXML.hh"
#include "pCTDetectorConstruction.hh"  
#include "pCTConstructor.hh"   

class pCTSciBarConstructor : public pCTConstructor {

public:
    pCTSciBarConstructor(G4String n, pCTDetectorConstruction* c)
        : pCTConstructor(n,c) {Init();};
    pCTSciBarConstructor(G4String n, pCTConstructor* p)
        : pCTConstructor(n,p) {Init();};
    virtual ~pCTSciBarConstructor();

    G4String GetName(void) {return fName;}

    /// Construct and return a G4 volume for the object.
    virtual G4LogicalVolume* GetPiece(void);

    /// Get the length of the bar to be constructed.
    virtual double GetLength(void) {return fLength;};

    /// Set the length of the bar to be constructed.
    virtual void SetLength(double len) {fLength = len;};

    /// Get the base width of the extruded bar.
    virtual double GetBase(void) {return fBase;};

    /// Set the base width of the extruded bar.
    virtual void SetBase(double base);

    /// Get the thickness of the reflective coating.
    virtual double GetCoatingThickness(void) {return fCoatingThickness;}
  
    /// Set the thickness of the reflective coating.
    virtual void SetCoatingThickness(double t) {fCoatingThickness=t;}

    /// Set the gap between the bars which corresponds to the space taken by
    /// whatever material is around the extrusion.  E.G. This is the space
    /// taken by the glue that is holding the bars together.
    virtual void SetGap(double g) {fGap = g;};

    /// Get the gap between the bars which corresponds to the space taken by
    /// whatever material is around the extrusion.  
    virtual double GetGap(void) {return fGap;};

    /// Get the height of the extruded bar.  The default value will be by a
    /// call to SetBase().
    virtual double GetHeight(void) {return fHeight;};

    /// Set the height of the extruded bar.
    virtual void SetHeight(double height);

    /// @{ The name of the material to use for the scintillator coating.
    /// [Default: ScintillatorCoating]
    virtual void SetCoatingMaterial(const char* mat) {fCoatingMaterial = mat;}
    const char* GetCoatingMaterial() const {return fCoatingMaterial.c_str();}
    /// @}

    /// @{ The name of the material to use for the scintillator. 
    /// [Default: Scintillator]
    virtual void SetScintillatorMaterial(const char* mat) {
        fScintillatorMaterial = mat;}
    const char* GetScintillatorMaterial() const {
        return fScintillatorMaterial.c_str();}
    /// @}

    virtual void SetVisibility(bool vis) {fIsVisible = vis;}
    virtual bool GetVisibility(void) {return fIsVisible;}

    pCTXML * GetpCTXML() const {return pCTXMLInput;};
  
private:
    void Init(void);

    // The name of the SciBar.
    G4String fName;

    /// The length of the scintillator bar.
    double fLength;

    /// The width of the base of the scintillator.  The base is along the X
    /// axis.
    double fBase;
    
    /// The height of the scintillator.  If the bar is triangular this will be
    /// calculated when the base is set, but may be overridden.  
    double fHeight;

    /// The radius of the hole.
    double fHoleRadius;
  
    /// The thickness of the material that is wrapping the scintillator (this
    /// may be extruded).
    double fCoatingThickness;

    /// Space outside of the bar to allow for glueing and mounting.
    double fGap;

    /// Flag if the created bar should be set to visible for G4 visualization. 
    bool fIsVisible;

    /// The name of the material to use for the coating.
    std::string fCoatingMaterial;

    /// The name of the material to use for the scintillator.
    std::string fScintillatorMaterial;
  
    pCTXML *pCTXMLInput;
};

#endif
