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

class pCTTrackingManager: public TObject {

private:

    pCTEvent* fevent;                                 // the event we are managing
    pCTXML*   fconfig;                                // the configuration used to generate the event.

    //--------------
    //
    // maps: the key is always a 'reco' trackID. There are always NCMOSTracks trackIDs.
    // 
    // info from CMOS tracker used to disambiguate the XY coordinate at the first SciDet layer:
    //
    std::vector<TVector3> seedPoints;              // list of 3D (x,y,z) points at the last CMOS plane (provided as input)
    std::vector<TVector3> seedVecs;                // list of 3D unitary vectors from CMOS tracker (provided as input)

    // info filled by the TrackingManager Class algorithms:
    //           
    int NrecoTracks;                                 // number of tracks which are succesfully reconstructed.
    std::map<int, bool> isReco;                      // true if the track is reco.
    std::map<int, std::vector<TVector3>> recoTracks; // list of 3D points reconstructed at the SciDet.
    //
    //------------

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

    std::vector< pCTTrack* > DoTracking();

    //double chi2(const double *par);

    std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>> SortTracksByChi2(
        std::vector< CMOSPixel*> Det1, 
        std::vector< CMOSPixel*> Det2, 
        std::vector< CMOSPixel*> Det3, 
        std::vector< CMOSPixel*> Det4);

    std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>> TrackSelector(
                            std::vector<std::pair<Double_t, std::vector<std::pair <int ,std::pair <int,int>>>>> sortedTracks,int nTracks);

    std::vector<TVector3> SpacePoint (std::vector<std::pair <int ,std::pair <int,int>>> pixelPoint);
    TVector3 Vd(TVector3 V1,TVector3 V2);

    //ClassDef(pCTTrackingManager,1) 
};

#endif
