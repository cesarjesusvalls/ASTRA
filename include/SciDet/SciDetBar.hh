
#ifndef SciDetBar_h
#define SciDetBar_h 1

#include "G4VHit.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4THitsCollection.hh"
#include "G4Types.hh"
#include "G4SystemOfUnits.hh"

class SciDetBar : public G4VHit {
public:
    /// Constructor
    SciDetBar();
    /// Destructor
    ~SciDetBar();
    //! Print on screen a Hit
    void Print();
  
public:
    //The new and delete operators are overloaded for performances reasons:
    inline void *operator    new(size_t);
    inline void  operator delete(void *aHit);

public:
    //simple set and get methods

    void    SetBarIndex(std::pair<G4int, G4int>  pix)  { Bar = pix; }
    void    SetPlaneNumber(const G4int plane) {planeNumber = plane; }
    
    std::pair<G4int, G4int> GetBarIndex()    const  { return Bar; }
    G4int                   GetX() const { return Bar.first; }
    G4int                   GetY() const { return Bar.second; }
    G4int                   GetPlaneNumber() const  { return planeNumber; }
      
private:
    G4int   planeNumber;
    std::pair<G4int, G4int> Bar;
    G4double electrons;
};


// -- new and delete overloaded operators:
extern G4ThreadLocal G4Allocator<SciDetBar>* SciDetBarAllocator;

inline void* SciDetBar::operator new(size_t)
{
    if(!SciDetBarAllocator) SciDetBarAllocator = new G4Allocator<SciDetBar>;
    return (void *) SciDetBarAllocator->MallocSingle();
}
inline void SciDetBar::operator delete(void *aHit)
{
    SciDetBarAllocator->FreeSingle((SciDetBar*) aHit);
}

#endif
