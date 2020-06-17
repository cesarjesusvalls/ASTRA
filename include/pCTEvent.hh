//
// C. Jesús Valls 
// cjesus@ifae.es
//
// M. Granado-González
// MMG998@student.bham.ac.uk
//

#ifndef _PCT_EVENT_H_
#define _PCT_EVENT_H_ 1

#include <TObject.h>
#include "CMOSPixel.hh"
#include "pCTXML.hh"
#include "SciDetHit.hh"

class pCTEvent : public TObject {

private:
    int fEvtId;
    std::map <G4int, std::vector< CMOSPixel* >>     fPixelsMap;
    std::vector< SciDetHit* >                       fSciDetHits;
    std::map <int, double >                         fGunEnergy;   // map from track ID to pgun true kin energy.

public:

    pCTEvent(){Init();};

    virtual ~pCTEvent(){
        this->ResetEvent();
    };

    void Init(){
    };

    void DrawSciDetHits (pCTXML* config);
    void DrawCMOSHits   (pCTXML* config);

    //-----Setters------

    void SetEvtId(int evtid)                                                {fEvtId      = evtid;};
    void SetPixelHitsMap(std::map<G4int, std::vector< CMOSPixel* >> fpxls)  {fPixelsMap  = fpxls;};
    void SetSciDetHits(std::vector< SciDetHit* > fhits)                     {fSciDetHits = fhits;};
    void SetGunEnergyMap(std::map <int, double > fgun)                      {fGunEnergy = fgun;};

    //------------------

    //-----Getters------

    int GetEvtId() {return fEvtId;};
    std::map<G4int, std::vector< CMOSPixel* >>  GetPixelHitsMap()    {return fPixelsMap;};
    std::vector< SciDetHit* >                   GetSciDetHits()      {return fSciDetHits;};
    std::map <int, double >                     GetGunEnergyMap()    {return fGunEnergy;};

    //------------------

    void ResetEvent(Option_t* /*option*/="")
    {
        fEvtId = -999;
        fPixelsMap.clear();
        fSciDetHits.clear();
        fGunEnergy.clear();
    }

    ClassDef(pCTEvent,1) 
};

#endif
