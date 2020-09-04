
#include "CMOSPixel.hh"
#include "G4SystemOfUnits.hh"

// -- one more nasty trick for new and delete operator overloading:
G4ThreadLocal G4Allocator<CMOSPixel>* CMOSPixelAllocator = 0;

CMOSPixel::CMOSPixel()
{
	planeNumber = -999;
	Pixel = std::make_pair(-999,-999);
	electrons = -999;
  	trackID = -999;
	return;
}

CMOSPixel::~CMOSPixel()
{
}

void CMOSPixel::Print()
{
   G4cout << "CMOSPixel::Print()" << G4endl;
   G4cout << "Plane = " << planeNumber << " PixelIndex = (" << Pixel.first << ", " << Pixel.second << ") electrons = " << electrons << G4endl;
}
