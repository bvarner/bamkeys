#ifndef BAMKEY_FILTER_STATE
#define BAMKEY_FILTER_STATE

#include <OS.h>
#include <Rect.h>

class BamKeysState {
	public:
		BamKeysState(int32 rows, int32 columns);
		~BamKeysState();
		
		void SetSize(int32 rows, int32 columns);
		void GetSize(int32 *rows, int32 *columns) const;
		
		void SetGridCombo(int32 key, int32 modifiers);
		void GetGridCombo(int32 *key, int32 *modifiers) const;
		
		void GetCombo(int32 row, int32 column, int32 *key, int32 *modifiers) const;
		
		void WorkspaceActivated(int32 workspace, bool active);
		BRect GetScreenSize();
		
		int32* fKeys;
		int32* fModKeys;
	private:
		int32 fTotalKeys;
		int32 fTotalRows;
		int32 fTotalColumns;
		
		int32 fShowGridKey;
		int32 fShowGridModKeys;
		
		BRect fScreenFrame;
};

#endif /* BAMKEY_FILTER_STATE */