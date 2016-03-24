#ifndef BAMKEY_PREF_APP
#define BAMKEY_PREF_APP

#include <Application.h>
#include <Message.h>

#include "BKPrefsWindow.h"
#include "BamKeysConstants.h"

class BamKeysApp: public BApplication {
	public:
		BamKeysApp();
		~BamKeysApp();
		
		virtual void AboutRequested();
		virtual void ReadyToRun();
	private:
		BamKeysPrefsWindow *fSetWindow;
};

#endif /* BAMKEY_PREF_APP */