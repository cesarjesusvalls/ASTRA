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
#include "SciDetHit.hh"

class pCTEvent : public TObject {

private:
    int fEvtId;
    std::map<G4int, std::vector< CMOSPixel* >>  fPixelsMap;
    std::vector< SciDetHit* >                   fSciDetHits;

public:

    pCTEvent(){Init();};

    virtual ~pCTEvent(){
        this->ResetEvent();
    };

    void Init(){
    };

    //-----Setters------

    void SetEvtId(int evtid)                                                {fEvtId      = evtid;};
    void SetPixelHitsMap(std::map<G4int, std::vector< CMOSPixel* >> fpxls)  {fPixelsMap  = fpxls;};
    void SetSciDetHits(std::vector< SciDetHit* > fhits)                     {fSciDetHits = fhits;};

    //------------------

    //-----Getters------

    int GetEvtId() {return fEvtId;};
    std::map<G4int, std::vector< CMOSPixel* >>  GetPixelHitsMap()    {return fPixelsMap;};
    std::vector< SciDetHit* >                   GetSciDetHits()      {return fSciDetHits;};

    //------------------

    void ResetEvent(Option_t* /*option*/="")
    {
        fEvtId = -999;
        fPixelsMap.clear();
        fSciDetHits.clear();
    }

    ClassDef(pCTEvent,1) 
};

#endif
