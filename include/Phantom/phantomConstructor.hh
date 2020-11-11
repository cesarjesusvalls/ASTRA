#ifndef phantomConstructor_h
#define phantomConstructor_h 1

#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

#include "pCTDetectorConstruction.hh"

class phantomConstructor{

public:

phantomConstructor();

    virtual ~phantomConstructor();
  
    G4LogicalVolume* GetPhantom(void);  

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......      
#endif

