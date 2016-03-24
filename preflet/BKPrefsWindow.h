#ifndef BAMKEY_WINDOW
#define BAMKEY_WINDOW

#include <Window.h>
#include <Rect.h>
#include <Message.h>
#include <View.h>
#include <Box.h>
#include <Button.h>
#include <CheckBox.h>
#include <Message.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <PopUpMenu.h>
#include <StringView.h>

#include "Prefs.h"
#include "BKGridControl.h"
#include "BKKeyView.h"

class BamKeysPrefsWindow : public BWindow {
	public:
		BamKeysPrefsWindow();
		~BamKeysPrefsWindow();
		
		virtual void MessageReceived(BMessage *msg);
		virtual bool QuitRequested();
		
		virtual void WorkspaceActivated(int32 workspace, bool active);
		
		virtual void FrameResized(float width, float height);
	private:
		BBox	*fRootView;
		
		BBox	*fScreenBox;
		BamKeysGridControl *fGrid;
		
		BBox	*fSettingBox;
		
		BButton *fBtnRevert;
		BButton *fBtnDefaults;
		Prefs	*fPrefs;
		
		BMenuField *fMnuFldRows;
		BMenuField *fMnuFldColumns;
		BCheckBox  *fChkEnabled;
		
		BamKeysKeyView *fLeftCommand;
		BamKeysKeyView *fRightCommand;
		BamKeysKeyView *fLeftControl;
		BamKeysKeyView *fRightControl;
		BamKeysKeyView *fLeftOption;
		BamKeysKeyView *fRightOption;
		BamKeysKeyView *fLeftShift;
		BamKeysKeyView *fRightShift;
		BamKeysKeyView *fCapsLock;
		BamKeysKeyView *fNumLock;
		BamKeysKeyView *fScrlLock;
		
		BStringView *fStrHotKey;
		
		bool fModified;
};



#endif /* BAMKEY_WINDOW */
