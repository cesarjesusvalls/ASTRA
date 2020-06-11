//
// C. Jesús Valls 
// cjesus@ifae.es
//
// M. Granado-González
// MMG998@student.bham.ac.uk
//

#ifndef pCTRootPersistencyManager_hh_seen
#define pCTRootPersistencyManager_hh_seen
 
#include <string>
#include <vector>

class TFile;
class TTree;

#include "G4VPersistencyManager.hh"
#include "pCTXML.hh"

#include "G4Navigator.hh"
#include "G4NavigationHistory.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "pCTEvent.hh"

//#include <TH2F.h> 

/// Provide a root output for the geant 4 events.
class pCTRootPersistencyManager : public G4VPersistencyManager {
public:
    virtual ~pCTRootPersistencyManager();

    /// stores anEvent and the associated objects into database.
    virtual G4bool Store(const G4Event* anEvent);
    virtual G4bool Store(const G4Run* aRun);
    virtual G4bool Store(const G4VPhysicalVolume* aWorld);

    virtual G4bool Retrieve(G4Event *&aEvent) {(void)aEvent; return false;}
    virtual G4bool Retrieve(G4Run*& aRun) {(void) aRun; return false;}
    virtual G4bool Retrieve(G4VPhysicalVolume*& aWorld) {(void) aWorld; return false;};

    /// If a persistency manager has not been created, create one.
    static pCTRootPersistencyManager* GetInstance(void);

    /// Return true if the ROOT output file is active.  This means that the
    /// output file is open and ready to accept data.
    bool IsOpen();

    /// Return a pointer to the current TFile.
    TFile* GetTFile() const {return fOutput;}

    /// Return a pointer to the TTree
    TTree* GetTTree() const {return fEventTree;}

    /// Return a pointer to the TTree
    pCTEvent* GetEvent() const {return fpCTEvent;}

    /// Interface with PersistencyMessenger (open and close the
    /// database).
    virtual G4bool Open(G4String dbname);
    virtual G4bool Close(void);

    // Open input XML configuration file
    void OpenXML(G4String filename);
    pCTXML *GetXMLInput(){return fpCTXMLInput;};

    void SetEventFirst(int first){fEventFirst=first;};
    void SetNEvents(int num){fNEvents=num;};

    int GetEventFirst(){return fEventFirst;};
    int GetNEvents(){return fNEvents;};  

private:
    /// The ROOT output file that events are saved into.
    TFile *fOutput;
    
    /// The event tree that contains the output events.
    TTree *fEventTree;
    /// The TTree branches
    pCTEvent *fpCTEvent; // The pCTEvent class

public:
    pCTXML *fpCTXMLInput;
    G4int fEventFirst;
    G4int fNEvents;
    int eraseme;

private:
    /// The filename of the output file.
    G4String fFilename;
    /// The number of events saved to the output file since the last write.
    int fEventsNotSaved;

public:
  
  
protected:
    /// use GetPersistencyManager() instead
    pCTRootPersistencyManager();

    /// Set the filename
    void SetFilename(G4String file) {
        fFilename = file;
    }

    G4String GetFilename(void) const {return fFilename;}

private:

  G4int fEventID; 
  
};
#endif
