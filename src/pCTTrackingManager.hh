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
#include "SciDetHit.hh"
#include "CMOSPixel.hh"
#include <TH1.h>
#include <TGraph.h>
#include <TH2.h>

class pCTTrackingManager: public TObject {

private:

    pCTEvent* fevent;                                 // the event we are managing
    pCTXML*   fconfig;                                // the configuration used to generate the event.

    //--------------
    std::vector< std::vector<CMOSPixel*>> fcmosTracks;  // list of tracks provided by the CMOS tracking algorithm.       
    int NrecoTracks;                                    // number of tracks which are succesfully reconstructed.
    std::map<int, bool> isReco;                         // true if the track is reco.
    std::map<int, bool> isCMOSReco;                     // true if the track is reco.
    std::map<int, std::vector<TVector3>> recoTracks;    // list of 3D points reconstructed at the SciDet.
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

    //void DrawRecoEvent();
    void DrawGeometry();

    //------------------

    double TrackOptimality(vector <CMOSPixel*> inputPixels, pCTXML* config);

    std::vector< pCTTrack* > DoRTTracking();
    void DoCMOSTracking();
    void DoCMOSChi2Tracking();
    std::vector<std::pair<std::pair<double,double>,std::pair<double,double>>> phantomPositions();
    TVector3 GetSpacePoint (CMOSPixel* pixel, int plane);

    //ClassDef(pCTTrackingManager,1) 
};

#endif
