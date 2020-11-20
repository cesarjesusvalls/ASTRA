
#include "AstraBar.hh"
#include "G4SystemOfUnits.hh"

// -- one more nasty trick for new and delete operator overloading:
G4ThreadLocal G4Allocator<AstraBar>* AstraBarAllocator = 0;

AstraBar::AstraBar()
{
	return;
}

AstraBar::~AstraBar()
{
}

void AstraBar::Print()
{
   G4cout << "AstraBar::Print()" << G4endl;
   G4cout << "Plane = " << planeNumber << " BarIndex = (" << Bar.first << ", " << Bar.second << ") electrons = " << electrons << G4endl;
}
