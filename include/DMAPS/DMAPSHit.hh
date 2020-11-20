#ifndef DMAPSHit_h
#define DMAPSHit_h 1

/*
 * Define user class DMAPSHit.
 *
 * We define "our" hit format : it is caracterized by its plane and strip
 * numbers, and an energy value, the accumulated energy in this strip
 */

#include "G4VHit.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4THitsCollection.hh"
#include "G4Types.hh"
#include "G4SystemOfUnits.hh"

/*
 * This class stores information of a hit.
 *
 * It contains
 *  - strip and plane number
 *  - deposited energy
 *  - position information
 */

class DMAPSHit : public G4VHit {
public:
  /// Constructor
  DMAPSHit(const G4int plane, G4ThreeVector position, G4int particle_ID, G4int track_ID);
  /// Destructor
  ~DMAPSHit();
  //! Print on screen a Hit
  void Print();
  
public:
  //The new and delete operators are overloaded for performances reasons:
  // -- Tricky business here... :-(, but provided for you below
  inline void *operator    new(size_t);
  inline void  operator delete(void *aHit);

public:
  //simple set and get methods

  void          SetLocalPosition(const G4ThreeVector & pos)  { localPosition = pos; }
  void    SetPlaneNumber(const G4int plane) {planeNumber = plane; }
  void          SetParticleID(const G4int particle_ID){particleID = particle_ID;}
  void          SetTrackID(const G4int track_ID){trackID = track_ID;}
  void    SetEnergyDeposited(const G4double E) {edep = E; electrons=(edep/eV)/3.6;}
  void    SetElectronsLiberated(const G4double e) {electrons = e; edep=e*3.6; } // NOTE check the units that this needs
  
  G4ThreeVector GetLocalPosition()    const  { return localPosition; }
  G4int         GetPlaneNumber() const  { return planeNumber; }
  G4int         GetParticleID () const  { return particleID;}
  G4int         GetTrackID () const  { return trackID;}
  G4double  GetEnergyDeposited() const { return edep; };
  G4double  GetElectronsLiberated() const { return electrons; };
    
private:
  G4int   planeNumber;
  G4int   particleID;
  G4int   trackID;
  G4ThreeVector localPosition;
  G4double edep;
  G4double electrons;
    
};

// Define the "hit collection" using the template class G4THitsCollection:
typedef G4THitsCollection<DMAPSHit> DMAPSHitCollection;


// -- new and delete overloaded operators:
extern G4ThreadLocal G4Allocator<DMAPSHit>* DMAPSHitAllocator;

inline void* DMAPSHit::operator new(size_t)
{
  //void *aHit;
  //aHit = (void *) DMAPSHitAllocator.MallocSingle();
  //return aHit;
  if(!DMAPSHitAllocator) DMAPSHitAllocator = new G4Allocator<DMAPSHit>;
  return (void *) DMAPSHitAllocator->MallocSingle();
}
inline void DMAPSHit::operator delete(void *aHit)
{
  //DMAPSHitAllocator.FreeSingle((DMAPSHit*) aHit);
  DMAPSHitAllocator->FreeSingle((DMAPSHit*) aHit);
}

#endif
