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

#include <TObject.h>
#include <TTree.h>
#include <TFile.h>
#include <TBits.h>
#include <TObjString.h>
#include <vector>
#include <TXMLEngine.h>

//#include "G4SystemOfUnits.hh" // NEW GLOBAL

using namespace std;

class pCTXML: public TObject{
public :

    pCTXML();
    pCTXML(const G4String inxmlname);
    ~pCTXML();
      
    G4String GetXMLFileName()   {return fXMLFile;};
    G4String GetXMLExample()    {return fExample;};
    
    G4int GetSciDetNBars()      {return fSciDetNBars;};
    G4int GetSciDetNLayers()    {return fSciDetNLayers;};
    
    G4double GetSciDetBarX()       {return fSciDetBarX;};
    G4double GetSciDetBarY()       {return fSciDetBarY;};
    G4double GetSciDetBarZ()       {return fSciDetBarZ;};

    G4double GetSciDetX()          {return fSciDetX;};
    G4double GetSciDetY()          {return fSciDetY;};
    G4double GetSciDetZ()          {return fSciDetZ;};

    G4bool GetUseSciDet()          {return fUseSciDet;};
    G4bool GetUseCMOS()            {return fUseCMOS;};
    G4bool GetUsePhatom()          {return fUsePhantom;};

    void SetInputs();

private :

    G4String fXMLFile;
    G4String fExample;

    G4int    fSciDetNBars;
    G4int    fSciDetNLayers;

    G4double fSciDetBarX;
    G4double fSciDetBarY;
    G4double fSciDetBarZ;

    G4double fSciDetX;
    G4double fSciDetY;
    G4double fSciDetZ;

    G4bool   fUseSciDet;
    G4bool   fUseCMOS;
    G4bool   fUsePhantom;

    void XML2String(string tmp_app,string &dest);
    void XML2Double(string tmp_app,double &dest);
    void XML2Int   (string tmp_app,int &dest);
    void XML2Bool  (string tmp_app,bool &dest);
    string TrimSpaces(string input);
    
    ClassDef(pCTXML,1) 
};

#endif
