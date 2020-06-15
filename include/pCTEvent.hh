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
#include "SciDetBar.hh"

class pCTEvent : public TObject {

private:
    int fEvtId;
    std::map<G4int, std::vector< CMOSPixel* >> fPixelsMap;
    std::map<G4int, std::vector< SciDetBar* >> fBarsMap;

public:

    pCTEvent(){Init();};

    virtual ~pCTEvent(){
        this->ResetEvent();
    };

    void Init(){
    };

    //-----Setters------

    void SetEvtId(int evtid)                                               {fEvtId     = evtid;};
    void SetPixelHitsMap(std::map<G4int, std::vector< CMOSPixel* >> fpxls) {fPixelsMap = fpxls;};
    void SetBarHitsMap(std::map<G4int, std::vector< SciDetBar* >> fbars)   {fBarsMap   = fbars;};

    //------------------

    //-----Getters------

    int GetEvtId() {return fEvtId;};
    std::map<G4int, std::vector< CMOSPixel* >> GetPixelHitsMap()    {return fPixelsMap;};
    std::map<G4int, std::vector< SciDetBar* >> GetBarHitsMap()      {return fBarsMap;};

    //------------------

    void ResetEvent(Option_t* /*option*/="")
    {
        fEvtId = -999;
        fPixelsMap.clear();
        fBarsMap.clear();
    }

    ClassDef(pCTEvent,1) 
};

#endif
