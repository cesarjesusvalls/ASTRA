//
// C. Jesús Valls 
// cjesus@ifae.es
//
// M. Granado-González
// MMG998@student.bham.ac.uk
//

#ifndef pCTXML_h
#define pCTXML_h 1

#include "G4RunManager.hh"
#include "G4Event.hh"

#include <TTree.h>
#include <TFile.h>
#include <TBits.h>
#include <TObjString.h>
#include <vector>
#include <TXMLEngine.h>

//#include "G4SystemOfUnits.hh" // NEW GLOBAL

using namespace std;

class pCTXML
{
public :

  pCTXML(const G4String inxmlname="");
  ~pCTXML();
    
  G4String GetXMLFileName()     {return fXMLFile;};
  G4String GetXMLExample()     {return fExample;};

  void SetInputs();

private :

  G4String fXMLFile;
  G4String fExample;

  void XML2String(string tmp_app,string &dest);
  void XML2Double(string tmp_app,double &dest);
  void XML2Int   (string tmp_app,int &dest);
  void XML2Bool  (string tmp_app,bool &dest);
  string TrimSpaces(string input);
  
};

#endif
