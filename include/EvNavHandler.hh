
#ifndef _PCT_EvNavHandler_H_
#define _PCT_EvNavHandler_H_ 1

#include <TEveManager.h>
#include <TObject.h>
#include <TTree.h>


class EvNavHandler : public TObject {
public:

   	int* event_id;
   	TTree* data;

	EvNavHandler(TTree* treedat, int* evt) :
	data(treedat), event_id(evt){};

 	EvNavHandler(){};
    virtual ~EvNavHandler(){};

	void load_event(TTree* data, int event_id)
	{
	   printf("Loading event %d.\n", event_id);
	   data->GetEntry(event_id);
	   gEve->Redraw3D(kFALSE, kTRUE);
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
