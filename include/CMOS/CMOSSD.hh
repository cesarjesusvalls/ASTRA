
#ifndef CMOSSD_h
#define CMOSSD_h 1
#include "pCTXML.hh"
#include "G4VSensitiveDetector.hh"
///class DetectorConstruction;
///class RunAction;

#include "CMOSHit.hh"              // <<- the hit "format" we define
#include "pCTXML.hh"   
class G4HCofThisEvent;           // <<- means "H(it) C(ollections) of This Event"

//#include "HistoManager.hh"
#include "iostream"
#include "string"
/*
 * Defines sensitve part of detector geometry.
 *
 * Stores Hits with 
 *  * deposited energy
 *  * position
 * in <i>Hit Collections of This Event</i>
 *
 * ProcessHits()
 */
class CMOSSD : public G4VSensitiveDetector
{
public:
  /// Constructor
  CMOSSD(G4String SDname);
  /// Destructor
  ~CMOSSD();

public:
  /// methods from base class G4VSensitiveDetector
  /// Mandatory base class method : it must to be overloaded:
   G4bool ProcessHits(G4Step *step, G4TouchableHistory *ROhist);

  /// (optional) method of base class G4VSensitiveDetector
  void Initialize(G4HCofThisEvent* HCE);
  /// (optional) method of base class G4VSensitiveDetector
  void EndOfEvent(G4HCofThisEvent* HCE);
  
  void SetBinaryName(G4String name) { fname = name; };


private:
  CMOSHitCollection*      hitCollection;
  G4int                 HCID;       //JT
  G4int pEvtID;
	std::ofstream fout;
	G4String fname;
  pCTXML*                   pCTXMLInput;
  //HistoManager* fHistoManager;
};

#endif
