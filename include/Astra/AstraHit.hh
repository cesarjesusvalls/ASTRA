
#ifndef AstraHit_h
#define AstraHit_h 1

#include "G4VHit.hh"
#include "G4Allocator.hh"
//#include "G4ThreeVector.hh"
#include "G4THitsCollection.hh"
#include "G4Types.hh"
#include "G4SystemOfUnits.hh"

class AstraHit : public G4VHit {
public:
    /// Constructor
    AstraHit(){;};
    AstraHit( const G4int copyID,  G4int layerID, G4int barID,
               G4int orientation,  G4bool isScint,
               G4double edep,  G4int particle_ID, G4int trackID);
    /// Destructor
    ~AstraHit();
    //! Print on screen a Hit
    void Print();
    
public:
    //The new and delete operators are overloaded for performances reasons:
    inline void *operator    new(size_t);
    inline void  operator delete(void *aHit);

public:
    //void SetLocalPosition(const G4ThreeVector & pos)    {fPos               = pos;          }
    void SetCopyID(const  G4int ID)                     {fCopyID            = ID;           }
    void SetLayerID(const G4int ID)                     {fLayerID           = ID;           }
    void SetBarID(const   G4int ID)                     {fBarID             = ID;           }
    void SetOrientation( const G4int X)                 {fOrientation       = X;            }
    void SetScint(const G4bool B)                       {fIsScint           = B;            }
    void SetParticleID (const G4int ID)                 {fParticleID        = ID;           }
    void SetTrackID    (const G4int ID)                 {fTrackID           = ID;           }
    void SetEnergyDeposited(const G4double E)           {fEdep              = E;            }
    void AddTrackID(int ID)                             {fListOfTrackID.push_back(ID);      }
    void AddParticleID(int ID)                          {fListOfParticleID.push_back(ID);   }

    //G4ThreeVector GetLocalPosition()        const  { return fPos;               }
    G4int         GetCopyID()               const  { return fCopyID;            }
    G4int         GetLayerID()              const  { return fLayerID;           }
    G4int         GetBarID()                const  { return fBarID;             }
    G4int         GetOrientation()          const  { return fOrientation;       }
    G4bool        GIsScint()                const  { return fIsScint;           }
    G4int         GetParticleID ()          const  { return fParticleID;        }
    G4int         GetTrackID ()             const  { return fTrackID;           }
    G4double      GetEnergyDeposited()      const  { return fEdep;              }
    std::vector<G4int> GetListOfTrackID()     const  { return fListOfTrackID;     }
    std::vector<G4int> GetListOfParticleID()  const  { return fListOfParticleID;  }

private:
    G4int         fCopyID;
    G4int         fLayerID;
    G4int         fBarID;
    G4int         fOrientation;
    G4bool        fIsScint;
    //G4ThreeVector fPos;
    G4double      fEdep;  
    G4int         fParticleID;
    G4int         fTrackID;
    std::vector<G4int> fListOfTrackID;
    std::vector<G4int> fListOfParticleID;
};

// Define the "hit collection" using the template class G4THitsCollection:
typedef G4THitsCollection<AstraHit> AstraHitCollection;

// -- new and delete overloaded operators:
extern G4ThreadLocal G4Allocator<AstraHit>* AstraHitAllocator;

inline void* AstraHit::operator new(size_t)
{
    if(!AstraHitAllocator) AstraHitAllocator = new G4Allocator<AstraHit>;
    return (void *) AstraHitAllocator->MallocSingle();
}
inline void AstraHit::operator delete(void *aHit)
{
    AstraHitAllocator->FreeSingle((AstraHit*) aHit);
}

#endif
