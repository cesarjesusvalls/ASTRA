#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclasses;

#pragma link C++ class G4String+;
#pragma link C++ class pCTXML+;
#pragma link C++ class pCTEvent+;
#pragma link C++ class G4VHit+;

#pragma link C++ class DMAPSPixel+;
#pragma link C++ class std::vector< DMAPSPixel >+;
#pragma link C++ class std::map<G4int, std::vector< DMAPSPixel >>+;

#pragma link C++ class AstraHit+;
#pragma link C++ class std::vector< AstraHit >+;

#pragma link C++ class pCTTrack+;
#pragma link C++ class std::vector< pCTTrack >+;

#pragma link C++ class pCTEventHandler+;

#pragma link C++ class PhantomHit+;

#endif // __CINT__
