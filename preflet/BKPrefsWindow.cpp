#include "BKPrefsWindow.h"

#include <Rect.h>
#include <String.h>
#include <InterfaceDefs.h>
#include <Application.h>

#include <stdio.h>

#define WIDTH 518
#define HEIGHT 350
#define MAX_DIVIDE 6


BamKeysPrefsWindow::BamKeysPrefsWindow()
	: BWindow(BRect(40, 30, 40 + WIDTH, 30 + HEIGHT), "BamKeys",
			B_TITLED_WINDOW, B_NOT_ZOOMABLE)// | B_NOT_RESIZABLE)
{
	fModified = false;
	fRootView = new BBox(BRect(0, 0, WIDTH, HEIGHT), 
						"root_view", B_FOLLOW_ALL, B_WILL_DRAW, B_PLAIN_BORDER);
	
	fRootView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	AddChild(fRootView);
	
	// Screen area selection box
	fScreenBox = new BBox(BRect(10, 10, ((WIDTH / 3) * 2) - 5, (HEIGHT - 50)), 
						"screen_box", B_FOLLOW_ALL, B_WILL_DRAW, B_FANCY_BORDER);
	BRect gridRect = fScreenBox->Frame();
	gridRect.OffsetTo(0, 0);
	gridRect.InsetBy(5, 5);
	fGrid = new BamKeysGridControl(gridRect, "screen_grid", "", new BMessage('grid'));
	fScreenBox->AddChild(fGrid);
	
	// Settings Box
	fSettingBox = new BBox(BRect(((WIDTH / 3) * 2) + 5, 10, WIDTH - 10, (HEIGHT - 50)),
						"settings_box", B_FOLLOW_RIGHT | B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW, B_FANCY_BORDER);
	
	BRect itmRect = fSettingBox->Frame();
	itmRect.Set(5, 10, itmRect.Width() - 5, (itmRect.Height() - 10) / 8);
	
	// Filter Enabled
	fChkEnabled = new BCheckBox(itmRect, "chkGrid", "Enable BamKeys", new BMessage('fltr'), B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	fChkEnabled->SetValue(fGrid->IsFiltering() ? B_CONTROL_ON : B_CONTROL_OFF);
	fSettingBox->AddChild(fChkEnabled);
	
	itmRect.OffsetBy(0, itmRect.Height());

	// Rows and Columns
	int32 rows;
	int32 columns;
	fGrid->GetSize(&rows, &columns);
	
	BPopUpMenu *mnuRows = new BPopUpMenu("");
	for (int i = 1; i <= MAX_DIVIDE; i++) {
		BString numb("");
		numb << i;
		BMenuItem *mnuItem = new BMenuItem(numb.String(), new BMessage('rowC'));
		mnuRows->AddItem(mnuItem);
		if (i == rows) {
			mnuItem->SetMarked(true);
		}
	}
	
	fMnuFldRows = new BMenuField(itmRect, "mnuRows", "Rows:", mnuRows, B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	fSettingBox->AddChild(fMnuFldRows);
	
	
	itmRect.OffsetBy(0, itmRect.Height());
	
	BPopUpMenu *mnuColumns = new BPopUpMenu("");
	for (int i = 1; i <= MAX_DIVIDE; i++) {
		BString numb("");
		numb << i;
		BMenuItem *mnuItem = new BMenuItem(numb.String(), new BMessage('colC'));
		mnuColumns->AddItem(mnuItem);
		if (i == columns) {
			mnuItem->SetMarked(true);
		}
	}
	
	fMnuFldColumns = new BMenuField(itmRect, "mnuCols", "Columns:", mnuColumns, B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	fSettingBox->AddChild(fMnuFldColumns);
	
	itmRect.OffsetBy(0, itmRect.Height());
	// Hotkey
	BRect stringRect(itmRect);
	stringRect.OffsetBy(0, -(itmRect.Height() / 3));
	fStrHotKey = new BStringView(stringRect, "lblModifiers", "Modifiers:", B_FOLLOW_TOP | B_FOLLOW_LEFT);
	fSettingBox->AddChild(fStrHotKey);
	itmRect.OffsetBy(0, itmRect.Height());

	// Caps Lock
	fCapsLock = new BamKeysKeyView(itmRect, "capsLock", B_CAPS_LOCK, B_FOLLOW_TOP | B_FOLLOW_LEFT);
	fSettingBox->AddChild(fCapsLock);
	itmRect.OffsetBy(0, itmRect.Height());
	
	// Num Lock
	fNumLock = new BamKeysKeyView(itmRect, "numLock", B_NUM_LOCK, B_FOLLOW_TOP | B_FOLLOW_LEFT);
	fSettingBox->AddChild(fNumLock);
	itmRect.OffsetBy(0, itmRect.Height());

	// Scroll Lock
	fScrlLock = new BamKeysKeyView(itmRect, "scrollLock", B_SCROLL_LOCK, B_FOLLOW_TOP | B_FOLLOW_LEFT);
	fSettingBox->AddChild(fScrlLock);
	itmRect.OffsetBy(0, itmRect.Height());
	
	BPoint half = itmRect.RightBottom();
	
	half.x = half.x / 2;
	itmRect.SetRightBottom(half);
	
	// Command Keys
	fLeftCommand = new BamKeysKeyView(itmRect, "leftCommand", B_LEFT_COMMAND_KEY, B_FOLLOW_TOP | B_FOLLOW_LEFT);
	fSettingBox->AddChild(fLeftCommand);
	itmRect.OffsetBy((itmRect.Width() + 5), 0);
	
	fRightCommand = new BamKeysKeyView(itmRect, "rightCommand", B_RIGHT_COMMAND_KEY, B_FOLLOW_TOP | B_FOLLOW_LEFT);
	fSettingBox->AddChild(fRightCommand);
	itmRect.OffsetBy(-(itmRect.Width() + 5), itmRect.Height());
	
	// Ctrl Keys
	fLeftControl = new BamKeysKeyView(itmRect, "leftControl", B_LEFT_CONTROL_KEY, B_FOLLOW_TOP | B_FOLLOW_LEFT);
	fSettingBox->AddChild(fLeftControl);
	itmRect.OffsetBy((itmRect.Width() + 5), 0);
	
	fRightControl = new BamKeysKeyView(itmRect, "rightControl", B_RIGHT_CONTROL_KEY, B_FOLLOW_TOP | B_FOLLOW_LEFT);
	fSettingBox->AddChild(fRightControl);
	itmRect.OffsetBy(-(itmRect.Width() + 5), itmRect.Height());
	
	// Option Keys
	fLeftOption = new BamKeysKeyView(itmRect, "leftOption", B_LEFT_OPTION_KEY, B_FOLLOW_TOP | B_FOLLOW_LEFT);
	fSettingBox->AddChild(fLeftOption);
	itmRect.OffsetBy((itmRect.Width() + 5), 0);
	
	fRightOption = new BamKeysKeyView(itmRect, "rightOption", B_RIGHT_OPTION_KEY, B_FOLLOW_TOP | B_FOLLOW_LEFT);
	fSettingBox->AddChild(fRightOption);
	itmRect.OffsetBy(-(itmRect.Width() + 5), itmRect.Height());
	
	// Shift Keys
	fLeftShift = new BamKeysKeyView(itmRect, "leftShift", B_LEFT_SHIFT_KEY, B_FOLLOW_TOP | B_FOLLOW_LEFT);
	fSettingBox->AddChild(fLeftShift);
	itmRect.OffsetBy((itmRect.Width() + 5), 0);
	
	fRightShift = new BamKeysKeyView(itmRect, "rightShift", B_RIGHT_SHIFT_KEY, B_FOLLOW_TOP | B_FOLLOW_LEFT);
	fSettingBox->AddChild(fRightShift);
	itmRect.OffsetBy(-(itmRect.Width() + 5), itmRect.Height());
	
	
	// Buttons that do things!
	fBtnDefaults = new BButton(BRect(10, HEIGHT - 35, 85, HEIGHT - 15), 
							"btn_defaults", "Defaults", new BMessage('deft'),
							B_FOLLOW_BOTTOM | B_FOLLOW_LEFT);
	fBtnRevert = new BButton(BRect(95, HEIGHT - 35, 170, HEIGHT - 15),
							"btn_revert", "Revert", new BMessage('revt'),
							B_FOLLOW_BOTTOM | B_FOLLOW_LEFT);
	
	fRootView->AddChild(fScreenBox);
	fRootView->AddChild(fSettingBox, fScreenBox);
	fRootView->AddChild(fBtnDefaults, fSettingBox);
	fRootView->AddChild(fBtnRevert, fBtnDefaults);
	
	// If the fGrid couldn't load it settings, force our applications' defaults.
	if (fGrid->InitCheck() != B_OK) {
		BMessage *defaults = new BMessage('deft');
		MessageReceived(defaults);
		fGrid->SaveSettings();
		delete defaults;
		fModified = false;
	}
	fBtnRevert->SetEnabled(fModified);
}

BamKeysPrefsWindow::~BamKeysPrefsWindow() {
	if (Lock() == B_OK) {
		delete fRootView;
		delete fScreenBox;
		delete fGrid;
		delete fSettingBox;
		delete fBtnDefaults;
		delete fBtnRevert;
		delete fChkEnabled;
		
		delete fLeftCommand;
		delete fRightCommand;
		delete fLeftControl;
		delete fRightControl;
		delete fLeftOption;
		delete fRightOption;
		delete fLeftShift;
		delete fRightShift;
		delete fCapsLock;
		delete fScrlLock;
		delete fNumLock;
		
		delete fStrHotKey;
		
		Unlock();
	}
}

void BamKeysPrefsWindow::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		case 'fltr': {
			fGrid->Filter(fChkEnabled->Value() == B_CONTROL_ON);
			fModified = true;
			break;
		}
		case 'colC': {
			int32 index;
			if (msg->FindInt32("index", &index) == B_OK) {
				fGrid->SetColumns(index + 1);
			}
			fModified = true;
			break;	
		}
		case 'rowC': {
			int32 index;
			if (msg->FindInt32("index", &index) == B_OK) {
				fGrid->SetRows(index + 1);
			}
			fModified = true;
			break;
		}
		case 'grid': {
			int32 row, column, modifiers, key;
			if (msg->FindInt32("row", &row) == B_OK &&
				msg->FindInt32("column", &column) == B_OK &&
				row >= 0 && column >= 0)
			{
				fGrid->GetShortcut(row, column, &modifiers, &key);
				if (!fGrid->IsSelected()) {
					fModified = true;
					// deselect them all
					fLeftCommand->Update(0);
					fRightCommand->Update(0);
					fLeftControl->Update(0);
					fRightControl->Update(0);
					fLeftOption->Update(0);
					fRightOption->Update(0);
					fLeftShift->Update(0);
					fRightShift->Update(0);
					fCapsLock->Update(0);
					fScrlLock->Update(0);
					fNumLock->Update(0);
				} else {
					fLeftCommand->Update(modifiers);
					fRightCommand->Update(modifiers);
					fLeftControl->Update(modifiers);
					fRightControl->Update(modifiers);
					fLeftOption->Update(modifiers);
					fRightOption->Update(modifiers);
					fLeftShift->Update(modifiers);
					fRightShift->Update(modifiers);
					fCapsLock->Update(modifiers);
					fScrlLock->Update(modifiers);
					fNumLock->Update(modifiers);
				}
			}
			break;
		}
		case 'revt': {
			// Reload the Prefs without saving!
			fGrid->Revert();
			
			fModified = false;
			break;
		}
		case 'deft': {
			// Load the defaults....
			if (Lock()) {
				fGrid->SetSize(3, 3);
				
				fGrid->SetShortcut(0, 0, B_COMMAND_KEY | B_LEFT_COMMAND_KEY | B_NUM_LOCK, 0x37);
				fGrid->SetShortcut(0, 1, B_COMMAND_KEY | B_LEFT_COMMAND_KEY | B_NUM_LOCK, 0x38);
				fGrid->SetShortcut(0, 2, B_COMMAND_KEY | B_LEFT_COMMAND_KEY | B_NUM_LOCK, 0x39);
				
				fGrid->SetShortcut(1, 0, B_COMMAND_KEY | B_LEFT_COMMAND_KEY | B_NUM_LOCK, 0x48);
				fGrid->SetShortcut(1, 1, B_COMMAND_KEY | B_LEFT_COMMAND_KEY | B_NUM_LOCK, 0x49);
				fGrid->SetShortcut(1, 2, B_COMMAND_KEY | B_LEFT_COMMAND_KEY | B_NUM_LOCK, 0x4a);
				
				fGrid->SetShortcut(2, 0, B_COMMAND_KEY | B_LEFT_COMMAND_KEY | B_NUM_LOCK, 0x58);
				fGrid->SetShortcut(2, 1, B_COMMAND_KEY | B_LEFT_COMMAND_KEY | B_NUM_LOCK, 0x59);
				fGrid->SetShortcut(2, 2, B_COMMAND_KEY | B_LEFT_COMMAND_KEY | B_NUM_LOCK, 0x5a);
				
				fGrid->SetShowGrid(B_COMMAND_KEY | B_LEFT_COMMAND_KEY | B_NUM_LOCK, 0x5b);
				
				fGrid->Filter(true);
				
				Unlock();
			}
			
			fModified = true;
			break;
		}
		default:
			BWindow::MessageReceived(msg);
	}
	
	// Update the UI states.
	
	BString strCol("");
	BString strRow("");
	int32 rows, columns;
	
	fGrid->GetSize(&rows, &columns);
	strRow << rows;
	strCol << columns;
	
	fMnuFldRows->Menu()->FindItem(strRow.String())->SetMarked(true);
	fMnuFldColumns->Menu()->FindItem(strCol.String())->SetMarked(true);

	fChkEnabled->SetValue(fGrid->IsFiltering() ? B_CONTROL_ON : B_CONTROL_OFF);
	
	fBtnRevert->SetEnabled(fModified);
}

bool BamKeysPrefsWindow::QuitRequested() {
	fGrid->SaveSettings();
	be_app->PostMessage(new BMessage(B_QUIT_REQUESTED));
	return true;
}

void BamKeysPrefsWindow::WorkspaceActivated(int32 workspace, bool active) {
	fGrid->Invalidate();
}

void BamKeysPrefsWindow::FrameResized(float width, float height) {
	fRootView->Invalidate();
	fSettingBox->Invalidate();
	fScreenBox->Invalidate();
	fGrid->Invalidate();
}