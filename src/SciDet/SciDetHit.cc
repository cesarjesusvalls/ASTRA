
#include "SciDetHit.hh"
#include "G4SystemOfUnits.hh"

// -- one more nasty trick for new and delete operator overloading:
G4ThreadLocal G4Allocator<SciDetHit>* SciDetHitAllocator = 0;

SciDetHit::SciDetHit( const G4int copyID,  G4int layerID, G4int barID,
              		  G4int orientation,  G4bool isScint,  G4ThreeVector position,
              		  G4bool edep,  G4int particle_ID)
	: fCopyID(copyID), fLayerID(layerID), fBarID(barID),fOrientation(orientation), fIsScint(isScint), fParticleID(particle_ID) {;};


SciDetHit::~SciDetHit()
{
}

void SciDetHit::Print()
{
    //Add one to the plane no. since they start at 0 but det. no starts at 1
	//G4cout << "Hit: Plane = " << planeNumber+1 << ", Strip = " << stripNumber << ", E = " << eDep/MeV << " MeV, t = "
   // << hit_time/s << " sec, isPrimary = " << (isPrimary?"true":"false") << ", track no: " << trackNumber << ", particle Z: " << ParticleZ << G4endl;
}
