
#ifndef SCIDETSD_h
#define SCIDETSD_h 1

#include "G4VSensitiveDetector.hh"
#include "AstraHit.hh"   
#include "pCTXML.hh"          
class G4HCofThisEvent;

#include "iostream"
#include "string"
 
class AstraSD : public G4VSensitiveDetector
{
public:
  /// Constructor
  AstraSD(G4String SDname);
  /// Destructor
  ~AstraSD();

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
  AstraHitCollection*      hitCollection;
  G4int                     HCID;
  G4int pEvtID;
	std::ofstream fout;
	G4String fname;
  pCTXML*                   pCTXMLInput;
};

#endif
