//
// C. Jesús Valls 
// cjesus@ifae.es
//
// M. Granado-González
// MMG998@student.bham.ac.uk
//

#ifndef pCT_CONSTRUCTOR_HH_SEEN
#define pCT_CONSTRUCTOR_HH_SEEN

#include <iostream>
#include <map>

#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWith3Vector.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>

#include "G4RotationMatrix.hh"
#include "pCTDetectorConstruction.hh"           


class pCTConstructor {
private:
    /// The short local name of the constructor.
    G4String fLocalName;

    /// The name of the constructor.
    G4String fName;

    /// The G4VUserDetectorConstruction class that this is cooperating with.
    //pCTUserDetectorConstruction* fConstruction;
    pCTDetectorConstruction* fConstruction;
    
    /// The parent of this constructor
    pCTConstructor* fParent;

    /// The user interface messenger that will provide values for this class. 
    G4UImessenger* fMessenger;

    /// If this is true, then draw constructed object.
    bool fVisible;

    /// The sub constructors that might be used in this class.
    std::map<G4String,pCTConstructor*> fSubConstructors;

protected:
    G4Material* FindMaterial(G4String m);

public:
    pCTConstructor(G4String n, pCTDetectorConstruction* c);
    pCTConstructor(G4String n, pCTConstructor* parent);
    virtual ~pCTConstructor();

    /// Construct and return a G4 volume for the object.  This is a pure
    /// virtual function, which means it must be implemented by the inheriting
    /// classes.  This returns an unplaced logical volume which faces along
    /// the Z axis.
    virtual G4LogicalVolume *GetPiece(void) = 0;

    /// Return the base name of the object that this builds.
    G4String GetName(void);

    /// Return the base name of the object that this builds.
    G4String GetLocalName(void) {return fLocalName;};

    /// Set the base name of the logical volume that this builds.
    void SetLocalName(const G4String& name);

    /// Set the visibility of the constructed object.
    void SetVisible(bool v);
    
    /// Set the visibility of the object daughters
    void SetVisibleDaughters(bool v);
    
    /// Get the visibility of the constructed object.
    bool GetVisible(void) const {return fVisible;}

    /// Return the detector construction that is building this piece.
    //pCTUserDetectorConstruction* GetConstruction(void) {
    pCTDetectorConstruction* GetConstruction(void) {
      return fConstruction;
    };
    
    /// Return the messenger for this constructor
    G4UImessenger* GetMessenger(void) {return fMessenger;};

    /// Set the messenger for this constructor.
    void SetMessenger(G4UImessenger* m) {
        fMessenger = m;
    };

    /// Add a new sub constructor to the current geometry constructor.  This
    /// should be done in the derived class ctor.  All geometry constructors
    /// that will be used by the derived class should be added using this
    /// method.
    void AddConstructor(pCTConstructor* c) {
        if (fSubConstructors.find(c->GetLocalName()) 
            != fSubConstructors.end()) {
            std::cout << "Multiply defined constructor name " 
                      << c->GetName()
                      << std::endl;
            //G4Exception("pCTConstructor::AddConstructor():"
	    //" Multiple defines");
	    G4ExceptionDescription msg;
	    msg << "Multiple defines" << G4endl;
	    G4Exception("pCTConstructor::AddConstructor()",
			"MyCode0002",JustWarning, msg);
	}
        fSubConstructors[c->GetLocalName()] = c;
    }

    /// Get a sub constructor by name and do the dynamic cast.  This will
    /// abort with an error message if you request an undefined name.
    template <class T> T& Get(G4String n) {
        std::map<G4String,pCTConstructor*>::iterator p 
            = fSubConstructors.find(n);
        if (p==fSubConstructors.end()) {
            std::cout << "Error in " << GetName() << std::endl;
            std::cout << "   Undefined constructor requested "
                      << n << std::endl;
            std::cout << "   The choices are: " << std::endl;
            for (p = fSubConstructors.begin();
                 p != fSubConstructors.end();
                 ++p) {
                std::cout << "        \"" << (*p).first << "\""
                          << " for constructor: " << (*p).second->GetName()
                          << std::endl;
            }
            // G4Exception("pCTConstructor::Get<>:"
            //             " Undefined constructor");
       	    G4ExceptionDescription msg;
	    msg << "Undefined constructor" << G4endl;
	    G4Exception("pCTConstructor::Get<>",
			"MyCode0002",JustWarning, msg);

	}
        T* c = dynamic_cast<T*>((*p).second);
        if (!c) {
            std::cout << "Error in " << GetName() << std::endl;
            std::cout << "  Cannot type cast " << n 
                      << " to requested class" << std::endl;
            // G4Exception("pCTConstructor::Get<>:"
            //             " Bad typecast");
       	    G4ExceptionDescription msg;
	    msg << "Bad typecast" << G4endl;
	    G4Exception("pCTConstructor::Get<>",
			"MyCode0002",JustWarning, msg);
        }
        return *c;
    }

    /// Find a sub constructor by name and do the dynamic cast.  This returns
    /// a pointer that will be NULL if you request an undefined name.
    template <class T> T* Find(G4String n) {
        std::map<G4String,pCTConstructor*>::iterator p 
            = fSubConstructors.find(n);
        if (p==fSubConstructors.end()) return NULL;
        T* c = dynamic_cast<T*>((*p).second);
        return c;
    }

    /// stringToRotationMatrix() converts a string "X90,Y45" into a
    /// G4RotationMatrix.
    /// This is an active rotation, in that the object is first rotated
    /// around the parent's X axis by 90 degrees, then the object is
    /// further rotated around the parent's Y axis by 45 degrees.
    /// The return value points to a G4RotationMatrix on the heap, so
    /// it is persistent. Angles are in degrees, can have decimals,
    /// and can be negative. Axes are X, Y, Z.
    static G4RotationMatrix stringToRotationMatrix(G4String rotation);
};

#endif
