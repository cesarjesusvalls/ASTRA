//
// C. Jesús Valls 
// cjesus@ifae.es
//
// M. Granado-González
// MMG998@student.bham.ac.uk
//

#ifndef _PCT_TrackingManager_H_
#define _PCT_TrackingManager_H_ 1

#include <TObject.h>
#include <TVector3.h>
#include "pCTEvent.hh"
#include "pCTXML.hh"
#include "AstraHit.hh"
#include "DMAPSPixel.hh"
#include <TH1.h>
#include <TGraph.h>
#include <TH2.h>

class pCTTrackingManager: public TObject {

private:

    pCTEvent* fevent;                                   // the event we are managing
    pCTXML*   fconfig;                                  // the configuration used to generate the event.

    //--------------
    std::vector< std::vector<DMAPSPixel*>> fcmosTracks;  // list of tracks provided by the DMAPS tracking algorithm.       
    int NrecoTracks;                                    // number of tracks which are succesfully reconstructed.
    std::map<int, bool> isReco;                         // true if the track is reco.
    std::map<int, bool> isDMAPSReco;                     // true if the track is reco.
    std::vector< pCTTrack* > fRecoTracks;               // list of pCTTracks
    //--------------

public:

    pCTTrackingManager(pCTEvent* evt, pCTXML* cnf);
    pCTTrackingManager(pCTEvent* evt, pCTXML* cnf, std::vector<TVector3> pnts, std::vector<TVector3> vecs);

    pCTTrackingManager(){
        Init();
    };

    virtual ~pCTTrackingManager(){
    };

    void Init(){
        NrecoTracks = 0;
    };

    //-----Methods------

    std::map<int, bool>   GetIsReco() {return isReco;}
    void SetIsReco(int i, bool b)     {isReco[i] = i;}
    void DrawGeometry();
    std::vector< pCTTrack* > GetRecoTracks() {return fRecoTracks;};

    //------------------

    double TrackOptimality(vector <DMAPSPixel*> inputPixels, pCTXML* config);

    void DoRTTracking();
    void DoDMAPSTracking();
    void DoDMAPSChi2Tracking();
    void phantomPositions();
    TVector3 GetSpacePoint (DMAPSPixel* pixel, int plane);
};

#endif
