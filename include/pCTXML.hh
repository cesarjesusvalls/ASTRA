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
    
    int GetSciDetNBars()      {return fSciDetNBars;};
    int GetSciDetNLayers()    {return fSciDetNLayers;};
    
    double GetSciDetBarX()       {return fSciDetBarX;};
    double GetSciDetBarY()       {return fSciDetBarY;};
    double GetSciDetBarZ()       {return fSciDetBarZ;};

    double GetBarCoatThick()      {return fBarCoat;};

    double GetSciDetX()          {return fSciDetX;};
    double GetSciDetY()          {return fSciDetY;};
    double GetSciDetZ()          {return fSciDetZ;};

    double GetIniPGUNEnergy()    {return fIniPGUNEnergy;}
    int    GetNProtons()         {return fNProtons; }

    double GetPosX()               {return fPosX;};
    double GetPosY()               {return fPosY;};
    double GetPosZ0()              {return fPosZ0;};
    double GetPosZ1()              {return fPosZ1;};
    double GetPosZ2()              {return fPosZ2;};
    double GetPosZ3()              {return fPosZ3;};
    double GetPlaneColumns()       {return fPlaneColumns;}
    double GetPlaneRows()          {return fPlaneRows;}
    double GetEpiThickness()       {return fEpiThickness;}
    double GetSubThickness()       {return fSubThickness;}

    double GetPosZi(int i){
        if      (i==0) return GetPosZ0();
        else if (i==1) return GetPosZ1();
        else if (i==2) return GetPosZ2();
        else if (i==3) return GetPosZ3();
        else {std::cout << "Error in GetPosZi: i Out Of Range!!" << std::endl; exit(0);}
        return -999;
    }
    
    G4bool GetUseSciDet()          {return fUseSciDet;};
    G4bool GetUseCMOS()            {return fUseCMOS;};
    G4bool GetUse4thCMOS()         {return fUse4thCMOS;};
    G4bool GetUsePhantom()          {return fUsePhantom;};

    G4bool UseEnergyWide()    {return fUseEnergyWide;};

    void SetXMLFileName(G4String val)       {fXMLFile           = val;}
    void SetXMLExample(G4String val)        {fExample           = val;}
    void SetBeamType(G4String val)          {fBeamType          = val;}
    void SetSciDetNBars(int val)          {fSciDetNBars       = val;}
    void SetSciDetNLayers(int val)        {fSciDetNLayers     = val;}
    void SetSciDetBarX(double val)        {fSciDetBarX        = val;}
    void SetSciDetBarY(double val)        {fSciDetBarY        = val;}
    void SetSciDetBarZ(double val)        {fSciDetBarZ        = val;}
    void SetBarCoatThick(double val)      {fBarCoat           = val;}
    void SetSciDetX(double val)           {fSciDetX           = val;}
    void SetSciDetY(double val)           {fSciDetY           = val;}
    void SetSciDetZ(double val)           {fSciDetZ           = val;}
    void SetIniPGUNEnergy(double val)     {fIniPGUNEnergy     = val;}
    void SetNProtons(int val)             {fNProtons          = val;}
    void SetPosX(double val)              {fPosX              = val;}
    void SetPosY(double val)              {fPosY              = val;}
    void SetPosZ0(double val)             {fPosZ0             = val;}
    void SetPosZ1(double val)             {fPosZ1             = val;}
    void SetPosZ2(double val)             {fPosZ2             = val;}
    void SetPosZ3(double val)             {fPosZ3             = val;}
    void SetPlaneColumns(double val)      {fPlaneColumns      = val;}
    void SetPlaneRows(double val)         {fPlaneRows         = val;}
    void SetEpiThickness(double val)      {fEpiThickness      = val;}
    void SetSubThickness(double val)      {fSubThickness      = val;}
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

    int    fSciDetNBars;
    int    fSciDetNLayers;
    int    fNProtons;

    double fIniPGUNEnergy;
    double fSciDetBarX;
    double fSciDetBarY;
    double fSciDetBarZ;

    double fSciDetX;
    double fSciDetY;
    double fSciDetZ;
    double fBarCoat;

    double fPosX;
    double fPosY;
    double fPosZ0;
    double fPosZ1;
    double fPosZ2;
    double fPosZ3;
    double fPlaneColumns;
    double fPlaneRows;
    double fEpiThickness;
    double fSubThickness;
  
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
