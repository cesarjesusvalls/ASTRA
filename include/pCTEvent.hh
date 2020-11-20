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
#include "DMAPSPixel.hh"
#include "pCTXML.hh"
#include "AstraHit.hh"
#include "pCTTrack.hh"
#include "TVector3.h"

class pCTEvent : public TObject {

private:
    int fEvtId;
    std::map <G4int, std::vector< DMAPSPixel* >>     fPixelsMap;
    std::vector< AstraHit* >                       fAstraHits;
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

    void DrawAstraHits (pCTXML* config);
    void DrawRecoTracks (pCTXML* config, std::vector< pCTTrack* > trks);
    void DrawRecoTracks3D (pCTXML* config, std::vector< pCTTrack* > trks);
    void DrawDMAPSHits   (pCTXML* config);

    //-----Setters------

    void SetEvtId(int evtid)                                                {fEvtId       = evtid;};
    void SetPixelHitsMap(std::map<G4int, std::vector< DMAPSPixel* >> pxls)   {fPixelsMap   = pxls;};
    void SetAstraHits(std::vector< AstraHit* > hits)                      {fAstraHits  = hits;};
    void SetGunEnergyMap(std::map <int, double > gun)                       {fGunEnergy   = gun;};
    void SetRecoProtons(std::vector< pCTTrack* > trks)                      {fRecoProtons = trks;};
    void SetPhantomHits(std::map<G4int,std::vector<std::pair<TVector3,double>>> phHits) {fPhantomHits = phHits;};
    void SetPhantomMidPos(TVector3 phMidPos){fphMidPos = phMidPos;};

  //------------------

    //-----Getters------

    int GetEvtId() {return fEvtId;};
    std::map<G4int, std::vector< DMAPSPixel* >>  GetPixelHitsMap()    {return fPixelsMap;};
    std::vector< AstraHit* >                   GetAstraHits()      {return fAstraHits;};
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
	    fAstraHits.clear();
        fGunEnergy.clear();
        for (auto prot:fRecoProtons) delete prot;
        fRecoProtons.clear();
    }

    ClassDef(pCTEvent,1) 
};

#endif
