#include "InfoWindow.h"

#include <Application.h>
#include <Rect.h>
#include "BKKeyView.h"

InfoWindow::InfoWindow(BamKeysState *filter) 
		: BWindow(BRect(0, 0, 0, 0), "BamKeys Grid Window", 
				B_NO_BORDER_WINDOW_LOOK, B_FLOATING_ALL_WINDOW_FEEL,
				B_NOT_MOVABLE | B_NOT_CLOSABLE | B_NOT_ZOOMABLE | 
				B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | 
				B_NO_WORKSPACE_ACTIVATION, B_ALL_WORKSPACES)
{
	fFilter = filter;
	fView = new DrawView();
	
	Lock();
	AddChild(fView);
	Unlock();
	
	// Start the Window's looper, so we don't get hung up if we try to quit and never show();
	BWindow::Hide();
	BWindow::Show();

	BScreen *screen = new BScreen();
	BRect screenRect = screen->Frame();
	ResizeTo(screenRect.Width(), screenRect.Height());
	fScreenshot = NULL;
	delete screen;
}

InfoWindow::~InfoWindow() {
	RemoveChild(fView);
	if (fScreenshot != NULL) {
		delete fScreenshot;
	}
	delete fView;
}

void InfoWindow::Show(info_mode type) {
	fCurrentMode = type;
	
	if (IsHidden() && Lock()) {
		ResizeTo(fFilter->GetScreenSize().Width(), fFilter->GetScreenSize().Height());
		
		BScreen *screen = new BScreen(this);
		if (fScreenshot == NULL) {
			screen->GetBitmap(&fScreenshot, true, NULL);
		} else {
			screen->ReadBitmap(fScreenshot, true, NULL);
		}
		fView->SetViewBitmap(fScreenshot);
		delete screen;
			
		Unlock();
	}
	
	while(IsHidden()) {
		BWindow::Show();
	}
	
	be_app->HideCursor();
}

void InfoWindow::Hide() {
	while(! IsHidden()) {
		BWindow::Hide();
	}
	
	be_app->ShowCursor();
}

/**
 * When the active workspace changes, if we're not hidden we need to update.
 */
void InfoWindow::WorkspaceActivated(int32 workspace, bool active){
	if (fScreenshot != NULL && fFilter->GetScreenSize() != BScreen().Frame()) {
		delete fScreenshot;
		fScreenshot = NULL;
	}
	
	fFilter->WorkspaceActivated(workspace, active);
	
	if (!IsHidden() && active) {
		Hide();
		// Sleep 1/2 a second to give the app_server time to draw the windows
		// that may be on the screen. KLUDGE!!!
		snooze(50000);
		Show(fCurrentMode);
	}
}


DrawView::DrawView()
	: BView(BRect(0, 0, 10, 10), "InfoBackground", B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
{
	SetViewColor(255, 255, 255, 0);
	SetLowColor(255, 255, 255, 0);
}

DrawView::~DrawView() {
}


void DrawView::Draw(BRect rect) {
	BView::Draw(rect);
	
	InfoWindow* window = (InfoWindow*)Window();
	
	if (window->fCurrentMode == BAMKEY_SHOW_GRID) {
		DrawGrid(window);
	} else if (window->fCurrentMode == BAMKEY_ERROR) {
		window->Hide();
	}
}

void DrawView::DrawGrid(InfoWindow *window) {
	SetDrawingMode(B_OP_ALPHA);
	BPoint ps = BPoint(0, 0);
	BPoint pe = BPoint(0, 0);
	BPoint max = BPoint(Frame().RightBottom());
	
	// Dark background
	SetPenSize(5.0);
	SetHighColor(0, 0, 0, 128);
	int32 fRows, fColumns;
	window->fFilter->GetSize(&fRows, &fColumns);
	
	BeginLineArray(fRows * fColumns);
	for (int32 column = 1; column < fColumns; column++) {
		ps.Set((max.x / fColumns) * column, 0);
		pe.Set(ps.x, max.y);
		
		AddLine(ps, pe, HighColor());
	}
	
	for (int32 row = 1; row < fRows; row++) {
		ps.Set(0, (max.y / fRows) * row);
		pe.Set(max.x, ps.y);
		
		AddLine(ps, pe, HighColor());
	}
	EndLineArray();
	
	// Lightened inside
	SetPenSize(3.0);
	SetHighColor(255, 255, 255, 128);
	BeginLineArray(fRows * fColumns);
	for (int32 column = 1; column < fColumns; column++) {
		ps.Set((max.x / fColumns) * column, 0);
		pe.Set(ps.x, max.y);
		
		AddLine(ps, pe, HighColor());
	}
	
	for (int32 row = 1; row < fRows; row++) {
		ps.Set(0, (max.y / fRows) * row);
		pe.Set(max.x, ps.y);
		
		AddLine(ps, pe, HighColor());
	}
	EndLineArray();
	
	SetPenSize(1.0);
	SetHighColor(255, 255, 255, 96);
	BRect zone;
	for (int32 row = 0; row < fRows; row++) {
		for (int32 column = 0; column < fColumns; column++) {
			zone.Set((max.x / fColumns) * column, (max.y / fRows) * row,
					(max.x / fColumns) * (column + 1), (max.y / fRows) * (row + 1));
			zone.InsetBy(3, 3);
			FillRect(zone, B_SOLID_HIGH);
			
			// Display the keys that need to be pressed for each zone.
			uint8 keyAlpha = 255;
			zone.InsetBy(5, 5);
			int32 key, modifiers;
			window->fFilter->GetCombo(row, column, &key, &modifiers);
			
			if (key != 0) {
				BPoint nextKey;
				BRect keySize;
				nextKey = zone.LeftTop();
				
				if (modifiers & B_NUM_LOCK) {
					BamKeysKeyView::GetIdealSize(&keySize, "Num Lock");
					keySize.OffsetTo(nextKey);
					if (! zone.Contains(keySize)) {
						keySize.OffsetTo(zone.left, keySize.LeftBottom().y + 5);
						nextKey.y = keySize.top;
					}
					BamKeysKeyView::DrawTo(this, keySize, key, true, "Num Lock", keyAlpha);
					nextKey.x = keySize.RightTop().x + 5;
				}
				
				if (modifiers & B_CAPS_LOCK) {
					BamKeysKeyView::GetIdealSize(&keySize, "Caps Lock");
					keySize.OffsetTo(nextKey);
					if (! zone.Contains(keySize)) {
						keySize.OffsetTo(zone.left, keySize.LeftBottom().y + 5);
						nextKey.y = keySize.top;
					}
					BamKeysKeyView::DrawTo(this, keySize, key, true, "Caps Lock", keyAlpha);
					nextKey.x = keySize.RightTop().x + 5;
				}
				
				if (modifiers & B_SCROLL_LOCK) {
					BamKeysKeyView::GetIdealSize(&keySize, "Scrl Lock");
					keySize.OffsetTo(nextKey);
					if (! zone.Contains(keySize)) {
						keySize.OffsetTo(zone.left, keySize.LeftBottom().y + 5);
						nextKey.y = keySize.top;
					}
					BamKeysKeyView::DrawTo(this, keySize, key, true, "Scrl Lock", keyAlpha);
					nextKey.x = keySize.RightTop().x + 5;
				}
				
				if (modifiers & B_LEFT_CONTROL_KEY) {
					BamKeysKeyView::GetIdealSize(&keySize, "Left Ctrl");
					keySize.OffsetTo(nextKey);
					if (! zone.Contains(keySize)) {
						keySize.OffsetTo(zone.left, keySize.LeftBottom().y + 5);
						nextKey.y = keySize.top;
					}
					BamKeysKeyView::DrawTo(this, keySize, key, false, "Left Ctrl", keyAlpha);
					nextKey.x = keySize.RightTop().x + 5;
				}
				
				if (modifiers & B_LEFT_OPTION_KEY) {
					BamKeysKeyView::GetIdealSize(&keySize, "Left Optn");
					keySize.OffsetTo(nextKey);
					if (! zone.Contains(keySize)) {
						keySize.OffsetTo(zone.left, keySize.LeftBottom().y + 5);
						nextKey.y = keySize.top;
					}
					BamKeysKeyView::DrawTo(this, keySize, key, false, "Left Optn", keyAlpha);
					nextKey.x = keySize.RightTop().x + 5;
				}
				
				if (modifiers & B_LEFT_COMMAND_KEY) {
					BamKeysKeyView::GetIdealSize(&keySize, "Left Alt");
					keySize.OffsetTo(nextKey);
					if (! zone.Contains(keySize)) {
						keySize.OffsetTo(zone.left, keySize.LeftBottom().y + 5);
						nextKey.y = keySize.top;
					}
					BamKeysKeyView::DrawTo(this, keySize, key, false, "Left Alt", keyAlpha);
					nextKey.x = keySize.RightTop().x + 5;
				}
				
				if (modifiers & B_LEFT_SHIFT_KEY) {
					BamKeysKeyView::GetIdealSize(&keySize, "Left Shift");
					keySize.OffsetTo(nextKey);
					if (! zone.Contains(keySize)) {
						keySize.OffsetTo(zone.left, keySize.LeftBottom().y + 5);
						nextKey.y = keySize.top;
					}
					BamKeysKeyView::DrawTo(this, keySize, key, false, "Left Shift", keyAlpha);
					nextKey.x = keySize.RightTop().x + 5;
				}
				
				if (modifiers & B_RIGHT_CONTROL_KEY) {
					BamKeysKeyView::GetIdealSize(&keySize, "Right Ctrl");
					keySize.OffsetTo(nextKey);
					if (! zone.Contains(keySize)) {
						keySize.OffsetTo(zone.left, keySize.LeftBottom().y + 5);
						nextKey.y = keySize.top;
					}
					BamKeysKeyView::DrawTo(this, keySize, key, false, "Right Ctrl", keyAlpha);
					nextKey.x = keySize.RightTop().x + 5;
				}
				
				if (modifiers & B_RIGHT_OPTION_KEY) {
					BamKeysKeyView::GetIdealSize(&keySize, "Right Optn");
					keySize.OffsetTo(nextKey);
					if (! zone.Contains(keySize)) {
						keySize.OffsetTo(zone.left, keySize.LeftBottom().y + 5);
						nextKey.y = keySize.top;
					}
					BamKeysKeyView::DrawTo(this, keySize, key, false, "Right Optn", keyAlpha);
					nextKey.x = keySize.RightTop().x + 5;
				}
				
				if (modifiers & B_RIGHT_COMMAND_KEY) {
					BamKeysKeyView::GetIdealSize(&keySize, "Right Alt");
					keySize.OffsetTo(nextKey);
					if (! zone.Contains(keySize)) {
						keySize.OffsetTo(zone.left, keySize.LeftBottom().y + 5);
						nextKey.y = keySize.top;
					}
					BamKeysKeyView::DrawTo(this, keySize, key, false, "Right Alt", keyAlpha);
					nextKey.x = keySize.RightTop().x + 5;
				}
				
				if (modifiers & B_RIGHT_SHIFT_KEY) {
					BamKeysKeyView::GetIdealSize(&keySize, "Right Shift");
					keySize.OffsetTo(nextKey);
					if (! zone.Contains(keySize)) {
						keySize.OffsetTo(zone.left, keySize.LeftBottom().y + 5);
						nextKey.y = keySize.top;
					}
					BamKeysKeyView::DrawTo(this, keySize, key, false, "Right Shift", keyAlpha);
					nextKey.x = keySize.RightTop().x + 5;
				}
				
				BString *keyLabel = new BString("");
				BamKeysKeyView::GetKeyLabel(keyLabel, key, modifiers);
				BamKeysKeyView::GetIdealSize(&keySize, keyLabel->String());
				keySize.OffsetTo(nextKey);
				if (! zone.Contains(keySize)) {
					keySize.OffsetTo(zone.left, keySize.LeftBottom().y + 5);
					nextKey.y = keySize.top;
				}
				BamKeysKeyView::DrawTo(this, keySize, key, false, keyLabel->String(), 255);
				delete keyLabel;
			}
		}
	}
	
	
}

