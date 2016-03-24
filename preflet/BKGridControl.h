#ifndef BAMKEY_GRID_CONTROL
#define BAMKEY_GRID_CONTROL

#include <Control.h>
#include <MessageFilter.h>
#include <Handler.h>
#include <InterfaceDefs.h>
#include <List.h>
#include <Message.h>
#include <Point.h>
#include <Window.h>

#include "BamKeysConstants.h"
#include "MessagePort.h"
#include "Prefs.h"


class KeyCombo {
	public:
		KeyCombo(int32 modifiers, int32 key);
		~KeyCombo();
		
		int32 fMods;
		int32 fKey;
};

class BamKeysGridControl : public BControl, BMessageFilter {
	public:
		BamKeysGridControl(BRect frame, const char* name, const char* label, BMessage *msg);
		~BamKeysGridControl();
		
		void GetSize(int32* rows, int32* columns) const;
		void SetSize(int32 rows, int32 columns);
		void SetRows(int32 rows);
		void SetColumns(int32 columns);
		
		void GetSelected(int32* row, int32* column) const;
		void SetSelected(int32 row, int32 column);
		bool IsSelected();
		
		void SetBackgroundColor(rgb_color color);
		
		status_t GetShortcut(int32 row, int32 column, int32* modifiers, int32* key) const;
		status_t SetShortcut(int32 row, int32 column, int32 modifiers, int32 key);
		
		void GetShowGrid(int32* modifiers, int32* key) const;
		void SetShowGrid(int32 modifiers, int32 key);
		
		virtual void MouseDown(BPoint where);
		virtual void MakeFocus(bool focused);
		
		virtual void Draw(BRect updateRect);
		
		virtual void AttachedToWindow();
		virtual void DetachedFromWindow();
		virtual filter_result Filter(BMessage *message, BHandler **target);
		virtual	status_t Invoke(BMessage *msg = NULL);
		
		void SaveSettings();
		void Revert();
		
		void Filter(bool enable);
		bool IsFiltering();
		
		status_t InitCheck();
	private:
		void ReadPrefs(BMessage *prefs);

		int32 fSelRow;
		int32 fSelCol;
		int32 fRows, fColumns;
		
		bool fFocused;
		bool fFiltered;
		
		BRect fRectInner;
		
		rgb_color fScreenColor;
		rgb_color fBlackColor;
		rgb_color fPwrLightColor;
		
		Prefs* fPrefs;
		BList* fAcceleratorList;
		MessagePort* fMsgPort;
		
		KeyCombo *fGridCombo;
};

#endif /* BAMKEY_GRID_CONTROL */
		
		