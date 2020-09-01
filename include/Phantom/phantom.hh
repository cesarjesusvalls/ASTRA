#ifndef PHANTOM_h
#define PHANTOM_h 1

#include "G4VSensitiveDetector.hh"
#include "PhantomHit.hh"
#include "pCTXML.hh"

class G4HCofThisEvent;
#include "iostream"
#include "string"


class Phantom : public G4VSensitiveDetector
{
public:
  //constructor
  Phantom(G4String SDname);
  //Destructor
  ~Phantom();

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

  PhantomHitCollection*      hitCollection;
  G4int                     HCID;

  G4int pEvtID;
  std::ofstream fout;
  G4String fname;
  pCTXML*                   pCTXMLInput;
};

#endif
