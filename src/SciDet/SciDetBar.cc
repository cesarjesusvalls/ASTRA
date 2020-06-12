
#include "SciDetBar.hh"
#include "G4SystemOfUnits.hh"

// -- one more nasty trick for new and delete operator overloading:
G4ThreadLocal G4Allocator<SciDetBar>* SciDetBarAllocator = 0;

SciDetBar::SciDetBar()
{
	return;
}

SciDetBar::~SciDetBar()
{
}

void SciDetBar::Print()
{
   G4cout << "SciDetBar::Print()" << G4endl;
   G4cout << "Plane = " << planeNumber << " BarIndex = (" << Bar.first << ", " << Bar.second << ") electrons = " << electrons << G4endl;
}
