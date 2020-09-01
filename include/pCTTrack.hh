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
#include "CMOSHit.hh"
#include "CMOSPixel.hh"
#include "CMOSSD.hh"
class pCTTrack : public TObject {

private:
    std::map <G4int, std::vector< CMOSPixel* >>     fPixelsMap;
    std::vector< SciDetHit* >                       fSciDetHits;
    double                                          fRecoEnergy;
    double                                          fRecoRange;
    double                                          fRngCorr;
    std::vector<TVector3>                          f3DHits;

public:

    pCTTrack(){Init();};

    virtual ~pCTTrack(){
    };

    void Init(){
    };

    //-----Setters------

    void SetPixelHitsMap(std::map<G4int, std::vector< CMOSPixel* >> pxls)  {fPixelsMap  = pxls;};
    void SetSciDetHits(std::vector< SciDetHit* > hits)                     {fSciDetHits = hits;};
    void SetRecoEnergy(double E)                                           {fRecoEnergy = E;};
    void SetRecoRange(double rng)                                          {fRecoRange  = rng;};
    void SetRngCorr  (double rng)                                          {fRngCorr    = rng;};
    void Set3DHits   (std::vector<TVector3> hits)                         {f3DHits     = hits;};

    //------------------

    //-----Getters------

    std::map<G4int, std::vector< CMOSPixel* >>  GetPixelHitsMap()       {return fPixelsMap;};
    std::vector< SciDetHit* >                   GetSciDetHits()         {return fSciDetHits;};
    double                                      GetRecoEnergy()         {return fRecoEnergy;};
    double                                      GetRecoRange()          {return fRecoRange;};
    double                                      GetRngCorr()            {return fRngCorr;};
    std::vector<TVector3>                       Get3DHits()             {return f3DHits;};

    //------------------

    ClassDef(pCTTrack,1) 
};

#endif
