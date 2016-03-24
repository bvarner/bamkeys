#include "BKFilterState.h"
#include <Screen.h>

BamKeysState::BamKeysState(int32 rows, int32 columns) {
	fShowGridKey = 0;
	fShowGridModKeys = 0;
	
	fKeys = new int32[0];
	fModKeys = new int32[0];
	
	fScreenFrame = BScreen().Frame();
	
	SetSize(rows, columns);
}

BamKeysState::~BamKeysState() {
	delete[] fKeys;
	delete[] fModKeys;
	
	fKeys = 0;
	fModKeys = 0;
}

void BamKeysState::SetSize(int32 rows, int32 columns) {
	delete[] fKeys;
	delete[] fModKeys;
	
	fTotalRows = rows;
	fTotalColumns = columns;
	fTotalKeys = rows * columns;
	
	fKeys = new int32[fTotalKeys];
	fModKeys = new int32[fTotalKeys];
}

void BamKeysState::GetSize(int32 *rows, int32 *columns) const {
	*rows = fTotalRows;
	*columns = fTotalColumns;
}
	
void BamKeysState::SetGridCombo(int32 key, int32 modifiers) {
	fShowGridKey = key;
	fShowGridModKeys = modifiers;
}

void BamKeysState::GetGridCombo(int32 *key, int32 *modifiers) const {
	*key = fShowGridKey;
	*modifiers = fShowGridModKeys;
}

void BamKeysState::GetCombo(int32 row, int32 column, int32 *key, int32 *modifiers) const {
	*key = fKeys[(row * fTotalColumns) + column];
	*modifiers = fModKeys[(row * fTotalColumns) + column];
}

void BamKeysState::WorkspaceActivated(int32 workspace, bool active) {
	fScreenFrame = BScreen().Frame();
}

BRect BamKeysState::GetScreenSize() {
	return fScreenFrame;
}
