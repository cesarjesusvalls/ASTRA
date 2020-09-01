
#include "CMOSHit.hh"
#include "G4SystemOfUnits.hh"

// -- one more nasty trick for new and delete operator overloading:
G4ThreadLocal G4Allocator<CMOSHit>* CMOSHitAllocator = 0;

CMOSHit::CMOSHit(const G4int plane, G4ThreeVector position, G4int particle_ID, G4int track_ID)
  : planeNumber(plane), localPosition(position), particleID(particle_ID), trackID(track_ID) // <<-- note BTW this is the only way to initialize a "const" member
{

	return;

}

CMOSHit::~CMOSHit()
{
}

void CMOSHit::Print()
{
    //Add one to the plane no. since they start at 0 but det. no starts at 1
	//G4cout << "Hit: Plane = " << planeNumber+1 << ", Strip = " << stripNumber << ", E = " << eDep/MeV << " MeV, t = "
   // << hit_time/s << " sec, isPrimary = " << (isPrimary?"true":"false") << ", track no: " << trackNumber << ", particle Z: " << ParticleZ << G4endl;
}
