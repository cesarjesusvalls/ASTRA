
#ifndef DMAPSPixel_h
#define DMAPSPixel_h 1

/*
 * Define user class DMAPSPixel.
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

class DMAPSPixel : public G4VHit {
public:
  /// Constructor
  DMAPSPixel();
  /// Destructor
  ~DMAPSPixel();
  //! Print on screen a Hit
  void Print();
  
public:
  //The new and delete operators are overloaded for performances reasons:
  // -- Tricky business here... :-(, but provided for you below
  inline void *operator    new(size_t);
  inline void  operator delete(void *aHit);

public:
  //simple set and get methods

  void    SetPixelIndex(std::pair<G4int, G4int>  pix)  { Pixel = pix; }
  void		SetPlaneNumber(const G4int plane) {planeNumber = plane; }
  void		SetElectronsLiberated(const G4double e) {electrons = e;}
  void    SetTrackID(G4int tID){trackID = tID;}
  
  std::pair<G4int, G4int> GetPixelIndex()    const  { return Pixel; }
  G4int		                GetX() const { return Pixel.first; }
  G4int		                GetY() const { return Pixel.second; }
  G4int                   GetPlaneNumber() const  { return planeNumber; }
  G4double	              GetElectronsLiberated() const { return electrons; }
  G4int                   GetTrackID()   const    {return trackID;  };
    
private:
	G4int   planeNumber;
	std::pair<G4int, G4int> Pixel;
	G4double electrons;
  G4int trackID;
    
};


// -- new and delete overloaded operators:
extern G4ThreadLocal G4Allocator<DMAPSPixel>* DMAPSPixelAllocator;

inline void* DMAPSPixel::operator new(size_t)
{
  if(!DMAPSPixelAllocator) DMAPSPixelAllocator = new G4Allocator<DMAPSPixel>;
  return (void *) DMAPSPixelAllocator->MallocSingle();
}
inline void DMAPSPixel::operator delete(void *aHit)
{
  DMAPSPixelAllocator->FreeSingle((DMAPSPixel*) aHit);
}

#endif
