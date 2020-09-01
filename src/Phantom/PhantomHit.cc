#include "PhantomHit.hh"
#include "G4SystemOfUnits.hh"

// -- one more nasty trick for new and delete operator overloading:
G4ThreadLocal G4Allocator<PhantomHit>* PhantomHitAllocator = 0;


PhantomHit::PhantomHit(G4ThreeVector pos, G4bool isPhantom, G4double edep,G4int particle_ID, G4int track_ID)
  :fPos(pos), fIsPhantom(isPhantom),fEdep(edep),fParticleID(particle_ID),fTrackID(track_ID){;};

PhantomHit::~PhantomHit(){}


