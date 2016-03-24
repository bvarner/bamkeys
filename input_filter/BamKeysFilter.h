#ifndef BAMKEY_FILTER_H
#define BAMKEY_FILTER_H

#include <add-ons/input_server/InputServerFilter.h>

#include <List.h>
#include <OS.h>
#include <Message.h>
#include <Rect.h>
#include <Region.h>
#include <Screen.h>
#include <InterfaceDefs.h>

#include "PrivateWindow.h"
#include "MessagePort.h"
#include "BamKeysConstants.h"
#include "Prefs.h"

#include "BKFilterState.h"
#include "InfoWindow.h"

class BamKeysFilter : public BInputServerFilter, BHandler {
	public:
		BamKeysFilter();
		virtual ~BamKeysFilter();
		
		virtual filter_result Filter(BMessage *message, BList *outList);
		
		virtual void MessageReceived(BMessage *message);
	private:
		MessagePort *fMsgPort;
		bool fDoFilter;
		
		int32 fColumn;
		int32 fRow;
		int32 fMatches;
		BRegion* fRegion;
		BRect fScreenFrame;
		
		BamKeysState *fState;
		InfoWindow* fInfoDisplay;
		
		bool fTabTarget;
		bool isTargetable(window_info *window, int32 currentWorkspace);
		
		// window_info->w_type constants imported from opentracker/deskbar/WindowMenu.cpp
		static const uint32 kWindowScreen = 1026;
		static const uint32 kNormalWindow = 0;
		
		void ReadPrefs(BMessage *msg);
};

#endif /* BAMKEY_FILTER_H */