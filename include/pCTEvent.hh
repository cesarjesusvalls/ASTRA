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

class pCTEvent : public TObject {

private:
    int fEvtId;

public:

    pCTEvent(){};

    virtual ~pCTEvent(){
        this->ResetEvent();
    };

    //-----Setters------

    void SetEvtId(int evtid) {fEvtId = evtid;};

    //------------------

    //-----Getters------

    int GetEvtId() {return fEvtId;};

    //------------------

    void ResetEvent(Option_t* /*option*/="")
    {
        fEvtId = -999;
    } 
};

#endif
