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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "G4SystemOfUnits.hh" // NEW GLOBAL

using namespace std;

class pCTXML: public TObject{
public :

    pCTXML();
    pCTXML(const G4String inxmlname);
    ~pCTXML();
      
    G4String GetXMLFileName()   {return fXMLFile;};
    G4String GetXMLExample()    {return fExample;};
    G4String GetBeamType()      {return fBeamType;};
    
    G4int GetSciDetNBars()      {return fSciDetNBars;};
    G4int GetSciDetNLayers()    {return fSciDetNLayers;};
    
    G4double GetSciDetBarX()       {return fSciDetBarX;};
    G4double GetSciDetBarY()       {return fSciDetBarY;};
    G4double GetSciDetBarZ()       {return fSciDetBarZ;};

    G4double GetBarCoatThick()      {return fBarCoat;};

    G4double GetSciDetX()          {return fSciDetX;};
    G4double GetSciDetY()          {return fSciDetY;};
    G4double GetSciDetZ()          {return fSciDetZ;};

    G4double GetIniPGUNEnergy()    {return fIniPGUNEnergy;}
    G4int    GetNProtons()         {return fNProtons; }

    G4double GetPosX()               {return fPosX;};
    G4double GetPosY()               {return fPosY;};
    G4double GetPosZ0()              {return fPosZ0;};
    G4double GetPosZ1()              {return fPosZ1;};
    G4double GetPosZ2()              {return fPosZ2;};
    G4double GetPosZ3()              {return fPosZ3;};
    G4double GetPlaneColumns()       {return fPlaneColumns;}
    G4double GetPlaneRows()          {return fPlaneRows;}
    G4double GetEpiThickness()       {return fEpiThickness;}
    G4double GetSubThickness()       {return fSubThickness;}
    
    G4bool GetUseSciDet()          {return fUseSciDet;};
    G4bool GetUseCMOS()            {return fUseCMOS;};
    G4bool GetUse4thCMOS()         {return fUse4thCMOS;};
    G4bool GetUsePhantom()          {return fUsePhantom;};
  
    G4bool UseEnergyWide()    {return fUseEnergyWide;};

    void SetXMLFileName(G4String val)       {fXMLFile           = val;}
    void SetXMLExample(G4String val)        {fExample           = val;}
    void SetBeamType(G4String val)          {fBeamType          = val;}
    void SetSciDetNBars(G4int val)          {fSciDetNBars       = val;}
    void SetSciDetNLayers(G4int val)        {fSciDetNLayers     = val;}
    void SetSciDetBarX(G4double val)        {fSciDetBarX        = val;}
    void SetSciDetBarY(G4double val)        {fSciDetBarY        = val;}
    void SetSciDetBarZ(G4double val)        {fSciDetBarZ        = val;}
    void SetBarCoatThick(G4double val)      {fBarCoat           = val;}
    void SetSciDetX(G4double val)           {fSciDetX           = val;}
    void SetSciDetY(G4double val)           {fSciDetY           = val;}
    void SetSciDetZ(G4double val)           {fSciDetZ           = val;}
    void SetIniPGUNEnergy(G4double val)     {fIniPGUNEnergy     = val;}
    void SetNProtons(G4int val)             {fNProtons          = val;}
    void SetPosX(G4double val)              {fPosX              = val;}
    void SetPosY(G4double val)              {fPosY              = val;}
    void SetPosZ0(G4double val)             {fPosZ0             = val;}
    void SetPosZ1(G4double val)             {fPosZ1             = val;}
    void SetPosZ2(G4double val)             {fPosZ2             = val;}
    void SetPosZ3(G4double val)             {fPosZ3             = val;}
    void SetPlaneColumns(G4double val)      {fPlaneColumns      = val;}
    void SetPlaneRows(G4double val)         {fPlaneRows         = val;}
    void SetEpiThickness(G4double val)      {fEpiThickness      = val;}
    void SetSubThickness(G4double val)      {fSubThickness      = val;}
    void SetUseSciDet(G4bool val)           {fUseSciDet         = val;}
    void SetUseCMOS(G4bool val)             {fUseCMOS           = val;}
    void SetUse4thCMOS(G4bool val)          {fUse4thCMOS        = val;}
    void SetUsePhantom(G4bool val)           {fUsePhantom        = val;}
    void SetUseEnergyWide(G4bool val)          {fUseEnergyWide     = val;}

    void SetInputs();

private :

    G4String fXMLFile;
    G4String fExample;
    G4String fBeamType;

    G4int    fSciDetNBars;
    G4int    fSciDetNLayers;
    G4int    fNProtons;

    G4double fIniPGUNEnergy;
    G4double fSciDetBarX;
    G4double fSciDetBarY;
    G4double fSciDetBarZ;

    G4double fSciDetX;
    G4double fSciDetY;
    G4double fSciDetZ;
    G4double fBarCoat;

    G4double fPosX;
    G4double fPosY;
    G4double fPosZ0;
    G4double fPosZ1;
    G4double fPosZ2;
    G4double fPosZ3;
    G4double fPlaneColumns;
    G4double fPlaneRows;
    G4double fEpiThickness;
    G4double fSubThickness;
  
    G4bool   fUseSciDet;
    G4bool   fUseCMOS;
    G4bool   fUse4thCMOS;
    G4bool   fUsePhantom;
    G4bool   fUseEnergyWide;

    void XML2String(string tmp_app,string &dest);
    void XML2Double(string tmp_app,double &dest);
    void XML2Int   (string tmp_app,int &dest);
    void XML2Bool  (string tmp_app,bool &dest);
    string TrimSpaces(string input);
    
    ClassDef(pCTXML,1) 
};

#endif
