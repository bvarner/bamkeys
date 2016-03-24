#include "BKGridControl.h"
#include "BKKeyView.h"
#include <Alert.h>
#include <String.h>

BamKeysGridControl::BamKeysGridControl(BRect frame, const char* name, const char* label, BMessage* msg)
	: BControl(frame, name, label, msg, B_FOLLOW_ALL, B_FRAME_EVENTS | B_WILL_DRAW | B_NAVIGABLE),
	  BMessageFilter(B_ANY_DELIVERY, B_ANY_SOURCE)
{
	fPrefs = new Prefs(BAMKEY_PREFERENCES_NAME);
	fGridCombo = new KeyCombo(0, 0);
	fSelRow = -1;
	fSelCol = -1;
	
	fRows = 1;
	fColumns = 1;
	
	fScreenColor.red = 160;
	fScreenColor.green = 160;
	fScreenColor.blue = 160;
	
	fBlackColor.red = 0;
	fBlackColor.green = 0;
	fBlackColor.blue = 0;
	
	fPwrLightColor.red = 228;
	fPwrLightColor.green = 0;
	fPwrLightColor.blue = 0;
	
	fAcceleratorList = new BList();
	SetSize(3, 3);
	
	fMsgPort = new MessagePort(BAMKEY_PORT_NAME, false, 0, NULL);
	if (!fMsgPort->InitCheck()) {
		BString errStr("");
		errStr << "The BamKeys preferences application is unable to talk to";
		errStr << " the Input Filter add-on.\n";
		errStr << "\n";
		errStr << "Please make sure it's installed properly.\n";
		errStr << "\n";
		errStr << "You will still be able to make changes and save your settings.\n";
		       
		BAlert *errorAlert = new BAlert("BamKeys Failure", errStr.String(), "Ok", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT);
		errorAlert->Go();
	}
	ReadPrefs(fPrefs);
}

BamKeysGridControl::~BamKeysGridControl() {
	KeyCombo *accel;
	for (int32 i = 0; (accel = (KeyCombo*)fAcceleratorList->ItemAt(i)); i++) {
		delete accel;
	}
	delete fAcceleratorList;
	delete fMsgPort;
}

status_t BamKeysGridControl::InitCheck() {
	return fPrefs->InitCheck();
}


void BamKeysGridControl::ReadPrefs(BMessage *prefs) {
	KeyCombo *accel;
	for (int32 i = 0; (accel = (KeyCombo*)fAcceleratorList->ItemAt(i)); i++) {
		delete accel;
	}
	delete fAcceleratorList;
	fAcceleratorList = new BList();
	
	if (prefs->FindInt32("rows", &fRows) != B_OK) {
		fRows = 1;
	}
	if (prefs->FindInt32("columns", &fColumns) != B_OK) {
		fColumns = 1;
	}
	
	fAcceleratorList->AddItem(new KeyCombo(0, 0), 0);
	int32 key;
	int32 modifier;
	for (int32 i = 1; i <= fRows * fColumns; i++) {
		if (prefs->FindInt32("key", i - 1, &key) == B_OK &&
			prefs->FindInt32("modifier", i - 1, &modifier) == B_OK)
		{
			fAcceleratorList->AddItem(new KeyCombo(modifier, key), i);
		} else {
			fAcceleratorList->AddItem(new KeyCombo(0, 0), i);
		}
	}
	
	if (prefs->FindInt32("grid_key", &key) == B_OK &&
		prefs->FindInt32("grid_modifier", &modifier) == B_OK)
	{
		fGridCombo->fKey = key;
		fGridCombo->fMods = modifier;
	}
	
	if (prefs->FindBool("enabled", &fFiltered) != B_OK) {
		fFiltered = true;
	}
	
	SetSelected(-1, -1);
}

void BamKeysGridControl::Revert() {
	ReadPrefs(fPrefs);
}

void BamKeysGridControl::SaveSettings() {
	fPrefs->SetBool("enabled", fFiltered);
	
	fPrefs->SetInt32("rows", fRows);
	fPrefs->SetInt32("columns", fColumns);
	
	// Delete the existing arrays of 'key' and 'modifier'
	fPrefs->RemoveName("key");
	fPrefs->RemoveName("modifier");
	
	// Create the new arrays.
	KeyCombo *combo;
	for(int32 i = 1; i < fAcceleratorList->CountItems(); i++) {
		combo = (KeyCombo*)fAcceleratorList->ItemAt(i);
		fPrefs->AddInt32("key", combo->fKey);
		fPrefs->AddInt32("modifier", combo->fMods);
	}
	
	// Set the 'show grid' keys.
	fPrefs->SetInt32("grid_key", fGridCombo->fKey);
	fPrefs->SetInt32("grid_modifier", fGridCombo->fMods);
	
	fPrefs->Save();
	
	// Send the settings to the input_filter
	fMsgPort->WriteMessage(fPrefs);
}

void BamKeysGridControl::Filter(bool enable) {
	fFiltered = enable;
}

bool BamKeysGridControl::IsFiltering() {
	return fFiltered;
}


/**
 * Filters for KeyDown / KeyUp messages. This way we have direct access to the key codes.
 */
filter_result BamKeysGridControl::Filter(BMessage *message, BHandler **target) {
	if (fFocused) {
		if (message->what == B_KEY_DOWN) {
			int32 key;
			int32 modifiers;
			if (message->FindInt32("modifiers", &modifiers) == B_OK &&
				message->FindInt32("key", &key) == B_OK)
			{
				int32 currentRow, currentColumn;
				GetSelected(&currentRow, &currentColumn);
				
				switch (key) {
					case 0x5e: // Spacebar selects / deselects
						if (IsSelected()) {
							SetSelected(-1, -1);
						} else {
							SetSelected(0, 0);
						}
						break;
					case 0x57: // up arrow
						if (currentRow > 0) {
							SetSelected(fSelRow - 1, fSelCol);
						}
						break;
					case 0x62: // down arrow
						if (currentRow < fRows - 1) {
							SetSelected(fSelRow + 1, fSelCol);
						}
						break;
					case 0x61: // left arrow
						if (currentColumn > 0) {
							SetSelected(fSelRow, fSelCol - 1);
						}
						break;
					case 0x63: // right arrow
						if (currentColumn < fColumns - 1) {
							SetSelected(fSelRow, fSelCol + 1);
						}
						break;
					default:
						if (IsSelected()) {
							SetShortcut(fSelRow, fSelCol, modifiers, key);
							Invalidate();
							Invoke();
						}
						break;
				}
			}
		}
	}
	return B_DISPATCH_MESSAGE;
}

/**
 * Whenever we're attached, we start to filter for the window we're a part of.
 */
void BamKeysGridControl::AttachedToWindow() {
	BControl::AttachedToWindow();
	if (Parent()) {
		SetViewColor(Parent()->ViewColor());
	}
	Window()->AddCommonFilter(this);
}

/**
 * When we're detached, we quit filtering.
 */
void BamKeysGridControl::DetachedFromWindow() {
	BControl::DetachedFromWindow();
	Window()->RemoveCommonFilter(this);
}

/**
 * Places the selected row and column into row and column.
 */
void BamKeysGridControl::GetSelected(int32 *row, int32 *column) const {
	*row = fSelRow;
	*column = fSelCol;
}

/**
 * Determines if there is a grid selected or not.
 */
bool BamKeysGridControl::IsSelected() {
	return fSelRow >=0 && fSelCol >= 0;
}

/**
 * Sets the selected row and column
 */
void BamKeysGridControl::SetSelected(int32 row, int32 column) {
	// If we're unselecting, invoke before we update the member variables.
	if ((row < 0 || column < 0) && fFiltered) {
		// Tell the filter to start listening again...
		BMessage *msg = new BMessage(BAMKEY_START_FILTER);
		fMsgPort->WriteMessage(msg);
		delete msg;
		
		Invoke();
	}
	fSelRow = row;
	fSelCol = column;
	
	// If we're selecting, Invoke after setting member variables.
	if ((row >= 0 && column >= 0) && fFiltered) {
		// Tell the filter to stop intercepting...
		BMessage *msg = new BMessage(BAMKEY_STOP_FILTER);
		fMsgPort->WriteMessage(msg);
		delete msg;
		
		Invoke();
	}
	
	Invalidate();
}

/**
 * Robustly adds the row, column fields to the message when Invoke() is called.
 */
status_t BamKeysGridControl::Invoke(BMessage *msg) {
	if (msg == NULL) {
		msg = Message();
	}
	
	int32 holder;
	if (msg->FindInt32("row", &holder) == B_OK) {
		msg->ReplaceInt32("row", fSelRow);
	} else {
		msg->AddInt32("row", fSelRow);
	}
	
	if (msg->FindInt32("column", &holder) == B_OK) {
		msg->ReplaceInt32("column", fSelCol);
	} else {
		msg->AddInt32("column", fSelCol);
	}

	return BControl::Invoke(msg);
}

/**
 * Get the current size of the grid into rows and columns
 */
void BamKeysGridControl::GetSize(int32 *rows, int32 *columns) const {
	*rows = fRows;
	*columns = fColumns;
}

/**
 * Resizes the internal data structure (BList) to contain the appropriate
 * number of elements to store a grid rows * columns. Retains data of 
 * existing columns / rows through the resize process.
 * 
 * This is acheived by first removing any whole rows, then columns,
 * then adding additional columns and rows.
 */
void BamKeysGridControl::SetSize(int32 rows, int32 columns) {
	// Create a new list
	BList *nList = new BList(rows * columns + 1);
	for (int i = 0; i <= rows * columns; i++) {
		nList->AddItem(new KeyCombo(0, 0), i);
	}
	
	// Copy the old grid into the new one.
	KeyCombo *combo;
	int32 r, c, modifiers, key;
	
	int32 copyRows = rows > fRows ? fRows : rows;
	int32 copyCols = columns > fColumns ? fColumns : columns;
	
	for (r = 0; r < copyRows; r++) {
		for (c = 0; c < copyCols; c++) {
			GetShortcut(r, c, &modifiers, &key);
			combo = (KeyCombo*)nList->ItemAt(r * fColumns + c + 1);
			if (combo != NULL) {
				combo->fMods = modifiers;
				combo->fKey = key;
			}
		}
	}
	
	// Empty the old list.
	for (int32 i = 0; (combo = (KeyCombo*)fAcceleratorList->ItemAt(i)); i++) {
		delete combo;
	}
	
	delete fAcceleratorList;
	fAcceleratorList = nList;
	
	fRows = rows;
	fColumns = columns;
	
	Invalidate();
}

/**
 * Set how many rows we have.
 */
void BamKeysGridControl::SetRows(int32 rows) {
	SetSize(rows, fColumns);
}

/**
 * Set how many columns we have.
 */
void BamKeysGridControl::SetColumns(int32 columns) {
	SetSize(fRows, columns);
}

/**
 * Get the shortcut for the given row and column.
 */
status_t BamKeysGridControl::GetShortcut(int32 row, int32 column, int32* modifiers, int32* key) const {
	KeyCombo *combo = (KeyCombo*)fAcceleratorList->ItemAt(row * fColumns + column + 1);
	if (combo != NULL) {
		*modifiers = combo->fMods;
		*key = combo->fKey;
		return B_OK;
	} else {
		*modifiers = 0;
		*key = 0;
		return B_ERROR;
	}
}

/**
 * Sets a shortcut for a given row and column
 * Does NOT Invoke();
 */
status_t BamKeysGridControl::SetShortcut(int32 row, int32 column, int32 modifiers, int32 key) {
	KeyCombo *combo = (KeyCombo*)fAcceleratorList->ItemAt(row * fColumns + column + 1);
	if (combo != NULL) {
		combo->fMods = modifiers;
		combo->fKey = key;
		return B_OK;
	} else {
		return B_ERROR;
	}
}

/**
 * Gets the shortcut key to show the grid.
 */
void BamKeysGridControl::GetShowGrid(int32* modifiers, int32* key) const {
	*modifiers = fGridCombo->fMods;
	*key = fGridCombo->fKey;
}

/**
 * Shortcut for displaying the Grid with the input_filter.
 * 
 * We store this at index 0 for now.
 */
void BamKeysGridControl::SetShowGrid(int32 modifiers, int32 key) {
	fGridCombo->fMods = modifiers;
	fGridCombo->fKey = key;
}

/**
 * Handle MouseDowns on us. We may need to select a region of the grid.
 */
void BamKeysGridControl::MouseDown(BPoint where) {
	int32 sc = -1;
	int32 sr = -1;
	for (int32 c = 1; c <= fColumns; c++) {
		float left = ((fRectInner.right - fRectInner.left) / fColumns) * (c - 1) + fRectInner.left;
		float right = ((fRectInner.right - fRectInner.left) / fColumns) * (c) + fRectInner.left;
		if (where.x >= left && where.x <= right) {
			sc = c - 1;
		}
	}
	
	for (int32 r = 1; r <= fRows; r++) {
		float top = ((fRectInner.bottom - fRectInner.top) / fRows) * (r - 1) + fRectInner.top;
		float bottom = ((fRectInner.bottom - fRectInner.top) / fRows) * r + fRectInner.top;
		if (where.y >= top && where.y <= bottom) {
			sr = r - 1;
		}
	}
	
	if (sr != fSelRow || sc != fSelCol) {
		SetSelected(sr, sc);
	} else if (sr == fSelRow && sc == fSelCol) {
		SetSelected(-1, -1);
	}
	MakeFocus(true);
}

/**
 * Override the MakeFocus to redraw us.
 */
void BamKeysGridControl::MakeFocus(bool focused) {
	fFocused = focused;
	BControl::MakeFocus(focused);
	Invalidate();
}

/**
 * Draw our control, and current state.
 */
void BamKeysGridControl::Draw(BRect updateRect) {
	SetDrawingMode(B_OP_OVER);
	BRect outer = Frame();
	outer.OffsetTo(0, 0);
	outer.InsetBy(10, 10);
	
	fRectInner = outer.InsetByCopy(4, 4);
	
	SetHighColor(fScreenColor);
	FillRoundRect(outer, 3, 3);
	SetHighColor(fBlackColor);
	StrokeRoundRect(outer, 3, 3);
	
	SetHighColor(ui_color(B_DESKTOP_COLOR));
	FillRoundRect(fRectInner, 2, 2);
	SetHighColor(fBlackColor);
	StrokeRoundRect(fRectInner, 2, 2);
	
	SetHighColor(fPwrLightColor);
	BPoint pwrSpot = fRectInner.LeftBottom() + BPoint(0, 2);
	StrokeLine(pwrSpot, pwrSpot + BPoint(1, 0));
	
	// Draw the grid.
	fRectInner.InsetBy(1, 1);
	BPoint ps = fRectInner.LeftTop();
	BPoint pe = fRectInner.LeftTop();
	BRect selRect = BRect();
	
	SetDrawingMode(B_OP_ALPHA);
	SetHighColor(255, 255, 255, 128);
	BeginLineArray(fRows * fColumns);
	for (int32 c = 1; c <= fColumns; c++) {
		ps.Set(((fRectInner.right - fRectInner.left) / fColumns) * c + fRectInner.left, fRectInner.top);
		pe.Set(ps.x, fRectInner.bottom);
		
		if (c < fColumns) {
			AddLine(ps, pe, HighColor());
		}
		
		if (fSelCol + 1 == c) {
			selRect.left = ((fRectInner.right - fRectInner.left) / fColumns) * (c - 1) + fRectInner.left;
			selRect.right = ((fRectInner.right - fRectInner.left) / fColumns) * (c) + fRectInner.left;
		}
	}
	
	for (int32 r = 1; r <= fRows; r++) {
		ps.Set(fRectInner.left, ((fRectInner.bottom - fRectInner.top) / fRows) * r + fRectInner.top);
		pe.Set(fRectInner.right, ps.y);
		
		if (r < fRows) {
			AddLine(ps, pe, HighColor());
		}
		
		if (fSelRow + 1 == r) {
			selRect.top = ((fRectInner.bottom - fRectInner.top) / fRows) * (r - 1) + fRectInner.top;
			selRect.bottom = ((fRectInner.bottom - fRectInner.top) / fRows) * r + fRectInner.top;
		}
	}
	EndLineArray();	
	
	// Tint the selRect square.
	if (fSelRow >= 0 && fSelCol >= 0) {
		selRect.InsetBy(1, 1);
		StrokeRect(selRect);
		
		SetHighColor(tint_color(ui_color(B_DESKTOP_COLOR), B_HIGHLIGHT_BACKGROUND_TINT));
		selRect.InsetBy(1, 1);
		FillRect(selRect);
	}
	
	// Draw the individual key shortcut into the zones.
	BRect zone;
	BRect keySize;
	for (int32 r = 0; r < fRows; r++) {
		for (int32 c = 0; c < fColumns; c++) {
			zone.Set(((fRectInner.right - fRectInner.left) / fColumns) * c + fRectInner.left,
			         ((fRectInner.bottom - fRectInner.top) / fRows) * r + fRectInner.top,
			         ((fRectInner.right - fRectInner.left) / fColumns) * (c + 1) + fRectInner.left,
			         ((fRectInner.bottom - fRectInner.top) / fRows) * (r + 1) + fRectInner.top);
			zone.InsetBy(3, 3);
			
			int32 key, modifiers;
			GetShortcut(r, c, &modifiers, &key);
			if (key != 0) {
				BString *keyLabel = new BString("");
				BamKeysKeyView::GetKeyLabel(keyLabel, key, modifiers);
				BamKeysKeyView::GetIdealSize(&keySize, keyLabel->String());
				keySize.OffsetTo(zone.left + (zone.Width() / 2) - (keySize.Width() / 2),
				                 zone.top + (zone.Height() / 2) - (keySize.Height() / 2));
				BamKeysKeyView::DrawTo(this, keySize, key, false, keyLabel->String(), 255);
				delete keyLabel;
			}
		}
	}
	
	// Draw the focus Indicator
	if (fFocused) {
		outer.InsetBy(-5, -5);
		SetHighColor(ui_color(B_KEYBOARD_NAVIGATION_COLOR));
		StrokeRect(outer);
	}
}

KeyCombo::KeyCombo(int32 modifiers, int32 key) {
	fMods = modifiers;
	fKey = key;
}

KeyCombo::~KeyCombo() {
}