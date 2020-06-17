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


  G4double GetPosX()               {return fPosX;};
  G4double GetPosY() {return fPosY;};
  G4double GetPosZ0() {return fPosZ0;};
  G4double GetPosZ1() {return fPosZ1;};
  G4double GetPosZ2() {return fPosZ2;};
  G4double GetPlaneWidth()     {return fPlaneWidth;}
  G4double GetPlaneHeight()     {return fPlaneHeight;}
  G4double GetEpiThickness()     {return fEpiThickness;}
  G4double GetSubThickness()     {return fSubThickness;}
  
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

  G4double fPosX;
  G4double fPosY;
  G4double fPosZ0;
  G4double fPosZ1;
  G4double fPosZ2;
  G4double fPlaneWidth;
  G4double fPlaneHeight;
  G4double fEpiThickness;
  G4double fSubThickness;

  
    G4bool   fUseSciDet;
    G4bool   fUseCMOS;
    G4bool   fUsePhantom;

    void XML2String(string tmp_app,string &dest);
    void XML2Double(string tmp_app,double &dest);
    void XML2Int   (string tmp_app,int &dest);
    void XML2Bool  (string tmp_app,bool &dest);
    string TrimSpaces(string input);
    
};

#endif
