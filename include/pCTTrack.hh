//
// C. Jesús Valls 
// cjesus@ifae.es
//
// M. Granado-González
// MMG998@student.bham.ac.uk
//

#ifndef _PCT_Track_H_
#define _PCT_Track_H_ 1

#include <TObject.h>
#include <TVector3.h>
#include "pCTXML.hh"
#include "SciDetHit.hh"
#include "CMOSPixel.hh"
#include "CMOSHit.hh"
#include "CMOSSD.hh"

class pCTTrack : public TObject {

private:
    std::map <G4int, std::vector< CMOSPixel* >>     fPixelsMap;
    std::vector< SciDetHit* >                       fSciDetHits;
    double                                          fRecoEnergy;
    double                                          fTrueEnergy;
    // array of observables associated to the track
    // 0 : Reco Calorimetry 
    // 1 : Rng [ last layer Z - first layer Z ]
    // 2 : Rng [ Eucl (first point XYZ, last point XYZ)]
    // 3 : Rng [ Sum_i Eucl(XYZ_i,XYZ_i+1)]
    double                                          fRecoMeas[5]; 
    double                                          fStraightness;
    std::vector<TVector3>                           f3DHits;
    std::vector<int>                                fBarIDs;
    int                                             fID;
    std::pair<std::pair<double,double>,std::pair<double,double>> fPhantomInfo;

public:

    pCTTrack(){Init();};

    virtual ~pCTTrack(){
    };

    void Init(){
        memset( fRecoMeas, 0, 5*sizeof(double) );
        for (int i(0); i<5; i++) fRecoMeas[i] = -999; 
        fRecoEnergy     = -999;
        fTrueEnergy     = -999;
        fStraightness   = -999;
        fID             = -999;
    };

    //-----Setters------
    void SetPixelHitsMap(std::map<G4int, std::vector< CMOSPixel* >> pxls)  {fPixelsMap       = pxls;};
    void SetSciDetHits(std::vector< SciDetHit* > hits)                     {fSciDetHits      = hits;};
    void SetRecoEnergy(double E)                                           {fRecoEnergy      = E;};
    void SetTrueEnergy(double E)                                           {fTrueEnergy      = E;};
    void SetRecoMeas(int idx, double mes)                                  {fRecoMeas[idx]   = mes;};
    void SetStraightness(double strn)                                      {fStraightness    = strn;};
    void Set3DHits      (std::vector<TVector3> hits)                       {f3DHits          = hits;}; 
    void SetBarIDs      (std::vector<int> bids)                            {fBarIDs          = bids;};    
    void SetID          (int  id)                                          {fID              = id;};
    void SetPhantomInfo (std::pair<std::pair<double,double>,std::pair<double,double>>  ph) {fPhantomInfo = ph;};                   
    //------------------

    //-----Getters------
    std::map<G4int, std::vector< CMOSPixel* >>  GetPixelHitsMap()       {return fPixelsMap;};
    std::vector< SciDetHit* >                   GetSciDetHits()         {return fSciDetHits;};
    double                                      GetRecoEnergy()         {return fRecoEnergy;};
    double                                      GetTrueEnergy()         {return fTrueEnergy;};
    double                                      GetRecoMeas(int idx)    {return fRecoMeas[idx];};
    double                                      GetStraightness()       {return fStraightness;};
    std::vector<TVector3>                       Get3DHits()             {return f3DHits;};
    std::vector<int>                            GetBarIDs()             {return fBarIDs;};
    int                                         GetID()                 {return fID;};
    std::pair<std::pair<double,double>,std::pair<double,double>>    GetPhantomInfo() {return fPhantomInfo;};
    double GetPhantomProjX () {return fPhantomInfo.second.first;};
    double GetPhantomProjY () {return fPhantomInfo.second.second;};

    //------------------

    ClassDef(pCTTrack,1) 
};

#endif
