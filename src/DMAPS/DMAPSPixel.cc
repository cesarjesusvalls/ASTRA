
#include "DMAPSPixel.hh"
#include "G4SystemOfUnits.hh"

// -- one more nasty trick for new and delete operator overloading:
G4ThreadLocal G4Allocator<DMAPSPixel>* DMAPSPixelAllocator = 0;

DMAPSPixel::DMAPSPixel()
{
	planeNumber = -999;
	Pixel = std::make_pair(-999,-999);
	electrons = -999;
  	trackID = -999;
	return;
}

DMAPSPixel::~DMAPSPixel()
{
}

void DMAPSPixel::Print()
{
   G4cout << "DMAPSPixel::Print()" << G4endl;
   G4cout << "Plane = " << planeNumber << " PixelIndex = (" << Pixel.first << ", " << Pixel.second << ") electrons = " << electrons << G4endl;
}
