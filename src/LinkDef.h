#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclasses;

#pragma link C++ class G4String+;
#pragma link C++ class pCTXML+;
#pragma link C++ class pCTEvent+;
#pragma link C++ class G4VHit+;

#pragma link C++ class CMOSPixel+;
#pragma link C++ class std::vector< CMOSPixel >+;
#pragma link C++ class std::map<G4int, std::vector< CMOSPixel >>+;

#pragma link C++ class SciDetHit+;
#pragma link C++ class std::vector< SciDetHit >+;

#endif // __CINT__
