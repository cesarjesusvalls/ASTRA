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

class pCTEvent : public TObject {

private:
    int fEvtId;
    std::map<G4int, std::vector< CMOSPixel* >> fPixels;

public:

    pCTEvent(){};

    virtual ~pCTEvent(){
        this->ResetEvent();
    };

    //-----Setters------

    void SetEvtId(int evtid)                                            {fEvtId = evtid;};
    void SetPixelHits(std::map<G4int, std::vector< CMOSPixel* >> fpxls) {fPixels = fpxls;};

    //------------------

    //-----Getters------

    int GetEvtId() {return fEvtId;};
    std::map<G4int, std::vector< CMOSPixel* >> GetPixelHits() {return fPixels;};

    //------------------

    void ResetEvent(Option_t* /*option*/="")
    {
        fEvtId = -999;
    }

    ClassDef(pCTEvent,1) 
};

#endif
