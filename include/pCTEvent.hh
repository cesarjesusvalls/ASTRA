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
#include <TRandom3.h>
#include "CMOSPixel.hh"
#include "pCTXML.hh"
#include "SciDetHit.hh"
#include "pCTTrack.hh"
#include "TVector3.h"
class pCTEvent : public TObject {

private:
    int fEvtId;
    std::map <G4int, std::vector< CMOSPixel* >>     fPixelsMap;
    std::vector< SciDetHit* >                       fSciDetHits;
    std::vector< pCTTrack* >                        fRecoProtons;
    std::map <int, double >                         fGunEnergy;   // map from track ID to pgun true kin energy.
    TRandom3 * fRndm;
    std::map <G4int,std::vector<std::pair<TVector3,double>>> fPhantomHits;
    TVector3 fphMidPos;
public:

    pCTEvent(){Init();};

    virtual ~pCTEvent(){
        this->ResetEvent();
    };

    std::vector< pCTTrack* >  Reconstruct(pCTXML* config);

    void Init(){
        fRndm = new TRandom3(0);
    };

    void DrawSciDetHits (pCTXML* config);
    void DrawCMOSHits   (pCTXML* config);

    //-----Setters------

    void SetEvtId(int evtid)                                                {fEvtId       = evtid;};
    void SetPixelHitsMap(std::map<G4int, std::vector< CMOSPixel* >> pxls)   {fPixelsMap   = pxls;};
    void SetSciDetHits(std::vector< SciDetHit* > hits)                      {fSciDetHits  = hits;};
    void SetGunEnergyMap(std::map <int, double > gun)                       {fGunEnergy   = gun;};
    void SetRecoProtons(std::vector< pCTTrack* > trks)                      {fRecoProtons = trks;};
    void SetPhantomHits(std::map<G4int,std::vector<std::pair<TVector3,double>>> phHits) {fPhantomHits = phHits;};
    void SetPhantomMidPos(TVector3 phMidPos){fphMidPos = phMidPos;};

  //------------------

    //-----Getters------

    int GetEvtId() {return fEvtId;};
    std::map<G4int, std::vector< CMOSPixel* >>  GetPixelHitsMap()    {return fPixelsMap;};
    std::vector< SciDetHit* >                   GetSciDetHits()      {return fSciDetHits;};
    std::map <int, double >                     GetGunEnergyMap()    {return fGunEnergy;};
    std::vector< pCTTrack* >                    GetRecoProtons()     {return fRecoProtons;};
    std::map<G4int,std::vector<std::pair<TVector3,double>>> GetPhantomHits() {return fPhantomHits;};
    TVector3 GetPhantomMidPos(){return fphMidPos;};
    
  //------------------

    void ResetEvent(Option_t* /*option*/="")
    {
        fEvtId = -999;
        fphMidPos.SetXYZ(-999,-999,-999);
        fPixelsMap.clear();
       	fPhantomHits.clear();
	    fSciDetHits.clear();
        fGunEnergy.clear();
        for (auto prot:fRecoProtons) delete prot;
        fRecoProtons.clear();
    }

    ClassDef(pCTEvent,1) 
};

#endif
