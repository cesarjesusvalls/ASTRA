//
// C. Jesús Valls 
// cjesus@ifae.es
//
// M. Granado-González
// MMG998@student.bham.ac.uk
//

#include <algorithm>
#include <functional>

#include <G4UnitsTable.hh>
#include "pCTConstructor.hh" 

pCTConstructor::pCTConstructor(G4String n, pCTDetectorConstruction* c) 
    : fLocalName(n), fName(n), fConstruction(c), fParent(NULL),
      fMessenger(NULL), fVisible(false) {
}

pCTConstructor::pCTConstructor(G4String n, pCTConstructor* p) 
    : fLocalName(n), fName(n), fConstruction(NULL), fParent(p), 
      fMessenger(NULL), fVisible(false) {
    fName = fParent->GetName() + "/" + fLocalName;
    fConstruction = fParent->GetConstruction();
}

pCTConstructor::~pCTConstructor() {;}

G4LogicalVolume *pCTConstructor::GetPiece(void) {
  //G4Exception("pCTConstructor::GetPiece(): Not implemented");
  G4ExceptionDescription msg;
  msg << "pCTConstructor::GetPiece(): Not implemented" << G4endl;
  G4Exception("pCTConstructor::GetPiece",
          "MyCode0002",FatalException, msg);
  return NULL;
}

G4Material* pCTConstructor::FindMaterial(G4String name) {
    G4Material* material = G4Material::GetMaterial(name,true);
    return material;
}

G4String pCTConstructor::GetName(void) {
    return fName;
}

/// Set the local name of the object to be built.  The local name of the
/// logical volume being set, and also sets the full name of the volume.
void pCTConstructor::SetLocalName(const G4String& name) {
    fLocalName = name;
    fName = name;
    if (fParent) fName = fParent->GetName() + "/" + fName;
}


/// Set the visibility of the constructed object.
void pCTConstructor::SetVisible(bool v) {
    if (fVisible != v) {
      //pCTVerbose("Set Visibility for " << GetName()
      //<< " from " << fVisible << " to " << v);   
      G4ExceptionDescription msg;
      msg << "Set Visibility for " << GetName() 
      << " from " << fVisible << " to " << v
      << G4endl;
      G4Exception("pCTConstructor::SetVisible",
          "MyCode0002",JustWarning, msg);
    }
    fVisible = v;
}

/// Set the visibility of the constructed object.
void pCTConstructor::SetVisibleDaughters(bool v) {
  if (fVisible != v) {
    //pCTVerbose("Set daughter visibility for " << GetName()
    //<< " to " << v);
    G4ExceptionDescription msg;
    msg << "Set daughter visibility for " 
    << GetName()
    << " to " << v
    << G4endl;
    G4Exception("pCTConstructor::SetVisibleDaughters",
        "MyCode0002",JustWarning, msg);
    
  }

    SetVisible(false);

    for (std::map<G4String,pCTConstructor*>::iterator p 
             = fSubConstructors.begin();
         p!=fSubConstructors.end();
         ++p) {
        (*p).second->SetVisible(v);
    }
}

G4RotationMatrix pCTConstructor::stringToRotationMatrix(G4String rotation)
{
  // We apply successive rotations OF THE OBJECT around the FIXED
  // axes of the parent's local coordinates; rotations are applied
  // left-to-right (rotation="r1,r2,r3" => r1 then r2 then r3).

  G4RotationMatrix rot;

  unsigned int place = 0;

  while (place < rotation.size()) {

        G4double angle;
        char* p(0);
        G4String current=rotation.substr(place+1);
        angle = strtod(current.c_str(),&p) * CLHEP::deg;

        if (!p || (*p != ',' && *p != '\0')) {
      //pCTError("Invalid rotation specification: "
      //<< rotation.c_str());
      G4ExceptionDescription msg;
      msg << "Invalid rotation specification: "
          << rotation.c_str()
          << G4endl;
      G4Exception("pCTConstructor::stringToRotationMatrix",
            "MyCode0002",JustWarning, msg);
    
       return rot;
        }
  
        G4RotationMatrix thisRotation;

        switch(rotation.substr(place,1).c_str()[0]) {
              case 'X': case 'x':
                thisRotation = G4RotationMatrix(CLHEP::HepRotationX(angle));
                break;
              case 'Y': case 'y':
                thisRotation = G4RotationMatrix(CLHEP::HepRotationY(angle));
                break;
              case 'Z': case 'z':
                thisRotation = G4RotationMatrix(CLHEP::HepRotationZ(angle));
                break;
              default:
                //pCTError(" Invalid rotation specification: "
        //<< rotation);   
        G4ExceptionDescription msg;
        msg << "Invalid rotation specification: "
            << rotation.c_str()
            << G4endl;
        G4Exception("pCTConstructor::stringToRotationMatrix",
            "MyCode0002",JustWarning, msg);
        
                return rot;
        }

       rot = thisRotation * rot;
       place = rotation.find(',',place);
       if (place > rotation.size()) break;
       ++place;
  }

  return rot;

}
