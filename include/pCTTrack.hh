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

class pCTTrack : public TObject {

private:
    std::map <G4int, std::vector< CMOSPixel* >>     fPixelsMap;
    std::vector< SciDetHit* >                       fSciDetHits;
    double                                          fRecoEnergy;
    double                                          fRecoMeas[5]; 
    double                                          fStraightness;

    // array of observables associated to the track
    // 0 : Reco Calorimetry 
    // 1 : Rng [ last layer Z - first layer Z ]
    // 2 : Rng [ Eucl (first point XYZ, last point XYZ)]
    // 3 : Rng [ Sum_i Eucl(XYZ_i,XYZ_i+1)]
    //

    std::vector<TVector3>                           f3DHits;

public:

    pCTTrack(){Init();};

    virtual ~pCTTrack(){
    };

    void Init(){
        memset( fRecoMeas, 0, 5*sizeof(double) );
    };

    //-----Setters------

    void SetPixelHitsMap(std::map<G4int, std::vector< CMOSPixel* >> pxls)  {fPixelsMap       = pxls;};
    void SetSciDetHits(std::vector< SciDetHit* > hits)                     {fSciDetHits      = hits;};
    void SetRecoEnergy(double E)                                           {fRecoEnergy      = E;};
    void SetRecoMeas(int idx, double mes)                                  {fRecoMeas[idx]   = mes;};
    void SetStraightness(double strn)                                      {fStraightness    = strn;};
    void Set3DHits   (std::vector<TVector3> hits)                          {f3DHits          = hits;};

    //------------------

    //-----Getters------

    std::map<G4int, std::vector< CMOSPixel* >>  GetPixelHitsMap()       {return fPixelsMap;};
    std::vector< SciDetHit* >                   GetSciDetHits()         {return fSciDetHits;};
    double                                      GetRecoEnergy()         {return fRecoEnergy;};
    double                                      GetRecoMeas(int idx)    {return fRecoMeas[idx];};
    double                                      GetStraightness()       {return fStraightness;};
    std::vector<TVector3>                       Get3DHits()             {return f3DHits;};

    //------------------

    ClassDef(pCTTrack,1) 
};

#endif
