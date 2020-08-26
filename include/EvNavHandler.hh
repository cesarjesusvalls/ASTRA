
#ifndef _PCT_EvNavHandler_H_
#define _PCT_EvNavHandler_H_ 1

#include <TEveManager.h>
#include <TObject.h>
#include <TTree.h>

#include <TEveGeoNode.h>
#include <TGeoNode.h>

#include <TVector3.h>
#include "pCTTrackingManager.hh"
#include "pCTEvent.hh"
#include "pCTXML.hh"
#include "pCTTrack.hh"
    // int nCMOS = 4;
    // if (!config->GetUse4thCMOS()) nCMOS = 3;

    // TGeoNode* CMOS[nCMOS];
    // TGeoNode* CMOSepi[nCMOS];
    // TGeoNode* CMOSsub[nCMOS];
    // for (int it(0); it<nCMOS; ++it){
    //     CMOS[it] = topNode->GetDaughter(it);
    //     CMOSepi[it] = CMOS[it]->GetDaughter(0);
    //     CMOSsub[it] = CMOS[it]->GetDaughter(1);
    // }
    // TGeoNode* SciDet = topNode->GetDaughter(nCMOS);

    // int nbars(config->GetSciDetNBars());
    // int nlayers(config->GetSciDetNLayers());

    // TGeoNode* SciDetbar[nbars*nlayers*2];
    // for (int it(0); it<nbars*nlayers*2; ++it){
    //     SciDetbar[it] = SciDet->GetDaughter(it);
    //     TEveGeoNode* tmpEve = new TEveGeoNode(SciDetbar[it]);
    //     tmpEve->SetMainColor(1);
    //     tmpEve->SetMainTransparency (99);
    // }


class EvNavHandler : public TObject {
public:

    int* event_id;
    TTree* data;
    pCTEvent* event;
    pCTXML* config;
    TGeoNode* SciDet;

    EvNavHandler(TTree* treedat, int* evt_id, pCTEvent* evt, pCTXML* cfg, TGeoNode* node) :
    data(treedat), event_id(evt_id), event(evt), config(cfg), SciDet(node) {};

    EvNavHandler(){};
    virtual ~EvNavHandler(){};

    void load_event(TTree* data, int event_id)
    {
        printf("Loading event %d.\n", event_id);
        data->GetEntry(event_id);
        
        std::vector<TVector3> points;
        std::vector<TVector3> vecs;
        printf("Yes\n");
        std::map<int, std::vector< CMOSPixel* > > Counter = event->GetPixelHitsMap();
        printf("No\n");
        std::map<int, std::vector< CMOSPixel*> >::iterator it;
        for(it=Counter.begin(); it!=Counter.end(); it++){
            ushort Plane = (*it).first;
            if (Plane != 3) continue;
            ushort nHitsInPlane = (*it).second.size();
            for(ushort index(0); index<nHitsInPlane; index++)
            {
                points.push_back(TVector3((*it).second.at(index)->GetX(),(*it).second.at(index)->GetY(),0));
                vecs.push_back(TVector3(0,0,0));
            }
        }
        std::cout << "#seeds: " << points.size() << std::endl;
        if(points.size())
            std::cout <<  points.back().X() << "," << points.back().Y() << "," << points.back().Z() << std::endl;
        pCTTrackingManager* trkMan = new pCTTrackingManager(event,config,points,vecs);
        auto recoTracks = trkMan->DoTracking();


        int nbars(config->GetSciDetNBars());
        int nlayers(config->GetSciDetNLayers());
        for (int it(0); it<nbars*nlayers; ++it){
            TEveGeoNode* tmpEve = new TEveGeoNode(SciDet->GetDaughter(it));
            tmpEve->SetRnrSelf(0);
            tmpEve->SetMainColor(event_id);
            //delete tmpEve;
        }

        event->DrawSciDetHits(config);

        int trkCnt = 0;
        for(auto trk=recoTracks.begin(); trk != recoTracks.end(); trk++){
            auto barIds = (*trk)->GetBarIDs();
            for(auto id=barIds.begin(); id!=barIds.end(); id++){
                std::cout << "id: " << (*id) << std::endl;
                TEveGeoNode* tmpEve = new TEveGeoNode(SciDet->GetDaughter((*id)));
                tmpEve->SetRnrSelf(1);
                tmpEve->SetMainColor(trkCnt);
                delete tmpEve;
            }
            trkCnt++;
        }
        


        // for (int it(0); it<30; ++it){
        //     TEveGeoNode* tmpEve = new TEveGeoNode(SciDet->GetDaughter(it));
        //     if (it%100==0){
        //         tmpEve->SetMainColor(2);
        //         tmpEve->SetMainTransparency (10);
        //     }
        //     else{
        //         tmpEve->SetMainColor(1);
        //         tmpEve->SetMainTransparency (99);
        //     }
        //     delete tmpEve;
        // }

        delete trkMan;
        gEve->FullRedraw3D(kFALSE, kTRUE);
    }

    void Fwd()
    {     
       if ((*event_id) < data->GetEntries() - 1) {
          ++(*event_id);
          load_event(data, (*event_id));
       } else {
          printf("Already at last event.\n");
       }
    }
    void Bck()
    {
       if ((*event_id) > 0) {
          --(*event_id);
          load_event(data, (*event_id));
       } else {
          printf("Already at first event.\n");
       }
    }
    ClassDef(EvNavHandler,1)
};

#endif
