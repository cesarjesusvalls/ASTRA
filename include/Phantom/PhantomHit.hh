#ifndef PhantomHit_h
#define PhantomHit_h 1

#include "G4VHit.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4THitsCollection.hh"
#include "G4Types.hh"
#include "G4SystemOfUnits.hh"

class PhantomHit : public G4VHit {
  public:
  PhantomHit(){;};
  PhantomHit(G4ThreeVector pos ,G4bool isPhantom,G4double edep,G4int particle_ID, G4int trackID);
  ~PhantomHit();



public:
  //The new and delete operators are overloaded for performances reasons:
  inline void *operator    new(size_t);
  inline void  operator delete(void *aHit);


  public:
  void SetLocalPosition(const G4ThreeVector & pos)    {fPos               = pos;          }
  void SetPhantom(const G4bool B)                       {fIsPhantom           = B;            }
  void SetParticleID (const G4int ID)                 {fParticleID        = ID;           }
  void SetTrackID    (const G4int ID)                 {fTrackID           = ID;           }
  void SetEnergyDeposited(const G4double E)           {fEdep              = E;            }
  void AddTrackID(int ID)                             {fListOfTrackID.push_back(ID);      }
  void AddParticleID(int ID)                          {fListOfParticleID.push_back(ID);   }


  G4ThreeVector GetLocalPosition()        const  { return fPos;               }
  G4bool GetPhantom()                     const  { return fIsPhantom;         }
  G4int         GetParticleID ()          const  { return fParticleID;        }
  G4int         GetTrackID ()             const  { return fTrackID;           }
  G4double      GetEnergyDeposited()      const  { return fEdep;              }
  std::vector<G4int> GetListOfTrackID()     const  { return fListOfTrackID;     }
  std::vector<G4int> GetListOfParticleID()  const  { return fListOfParticleID;  }

  private:

  G4ThreeVector fPos;
  G4bool        fIsPhantom;
  G4double      fEdep;
  G4int         fParticleID;
  G4int         fTrackID;
  std::vector<G4int> fListOfTrackID;
  std::vector<G4int> fListOfParticleID;

  };


//Define the "hit collection" using the template class G4THitsCollection:      
typedef G4THitsCollection<PhantomHit> PhantomHitCollection;

// -- new and delete overloaded operators:

extern G4ThreadLocal G4Allocator<PhantomHit>* PhantomHitAllocator;


inline void* PhantomHit::operator new(size_t)
{
if(!PhantomHitAllocator) PhantomHitAllocator = new G4Allocator<PhantomHit>;
return (void *)PhantomHitAllocator->MallocSingle();
}


inline void PhantomHit::operator delete(void *aHit)
{
  PhantomHitAllocator->FreeSingle((PhantomHit*) aHit);
}


#endif
