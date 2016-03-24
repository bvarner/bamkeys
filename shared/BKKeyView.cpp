#include "BKKeyView.h"
#include <Font.h>
#include <stdlib.h>
#include <OS.h>

BamKeysKeyView::BamKeysKeyView(BRect frame, const char* name, int32 key, uint32 resizeMode = B_FOLLOW_LEFT | B_FOLLOW_TOP, uint32 flags = B_WILL_DRAW) 
	: BView(frame, name, resizeMode, flags)
{
	fPressed = false;
	fKeyLabel = NULL;
	fTransparency = 255;
	
	SetKey(key);
	
	SetFont(be_fixed_font);
}

BamKeysKeyView::~BamKeysKeyView() {
	if (fKeyLabel != NULL) {
		delete fKeyLabel;
	}
}

void BamKeysKeyView::AttachedToWindow() {
	BView::AttachedToWindow();
	if (Parent()) {
		SetViewColor(Parent()->ViewColor());
	}
}

void BamKeysKeyView::SetKey(int32 key, int32 modifiers = 0) {
	if (fKeyLabel != NULL) {
		delete fKeyLabel;
	}
	fKey = key;
	fKeyLabel = new BString("");
	GetKeyLabel(fKeyLabel, fKey, modifiers);
	Invalidate();
}

void BamKeysKeyView::SetTransparency(uint8 alpha) {
	fTransparency = alpha;
}

void BamKeysKeyView::Update(int32 keys) {
	if (modifiers == 0) {
		fPressed = false;
	} else {
		fPressed = keys & fKey;
	}
	Invalidate();
}

void BamKeysKeyView::Draw(BRect update) {
	if (fKeyLabel != NULL) {
		DrawTo(this, this->Bounds(), fKey, fPressed, fKeyLabel->String(), fTransparency);
	} else {
		DrawTo(this, this->Bounds(), fKey, fPressed, NULL, fTransparency);
	}
}

void BamKeysKeyView::GetKeyLabel(BString *keyLabel, int32 keycode, int32 modifiers = 0) {
	switch (keycode) {
		case 0:
			break;
		case B_LEFT_SHIFT_KEY:
		case B_RIGHT_SHIFT_KEY:
			*keyLabel << "Shift";
			break;
		case B_LEFT_COMMAND_KEY:
		case B_RIGHT_COMMAND_KEY:
			*keyLabel << "Alt";
			break;
		case B_LEFT_CONTROL_KEY:
		case B_RIGHT_CONTROL_KEY:
			*keyLabel << "Ctrl";
			break;
		case B_LEFT_OPTION_KEY:
		case B_RIGHT_OPTION_KEY:
			*keyLabel << "Optn";
			break;
		case B_CAPS_LOCK:
			*keyLabel << "Caps Lock";
			break;
		case B_NUM_LOCK:
			*keyLabel << "Num Lock";
			break;
		case B_SCROLL_LOCK:
			*keyLabel << "Scrl Lock";
			break;
		// Numeric keys get special attention if numlock is on when mapping.
		case 0x64: // 0
		case 0x58: // 1
		case 0x59: // 2
		case 0x5a: // 3
		case 0x48: // 4
		case 0x49: // 5
		case 0x4a: // 6
		case 0x37: // 7
		case 0x38: // 8
		case 0x39: // 9
			if (modifiers & B_NUM_LOCK) {
				// Switch the state of the SHIFT_KEY to get the
				// proper mapping for numeric keypad keys.
				if (modifiers & B_SHIFT_KEY) {
					modifiers = modifiers ^ B_SHIFT_KEY;
				} else {
					modifiers = modifiers | B_SHIFT_KEY;
				}
			}
		default:
			// Lookup the key code to resolve it to a char.
			//debugger("getkey");
			key_map *keymap;
			char* chars;
			get_key_map(&keymap, &chars);
			
			int32 charindex;

// If you want to see the actual character created by pressing the combination, then
// use the following 'if' block.
/*			if (modifiers & B_CONTROL_KEY) {
				charindex = keymap->control_map[keycode];
			} else if (modifiers & B_OPTION_KEY &&
		               modifiers & B_CAPS_LOCK &&
		               modifiers & B_SHIFT_KEY) 
		    {
				charindex = keymap->option_caps_shift_map[keycode];
			} else if (modifiers & B_OPTION_KEY &&
			           modifiers & B_CAPS_LOCK)
			{
				charindex = keymap->option_caps_map[keycode];
			} else if (modifiers & B_OPTION_KEY &&
			           modifiers & B_SHIFT_KEY)
			{
				charindex = keymap->option_shift_map[keycode];
			} else if (modifiers & B_OPTION_KEY) {
				charindex = keymap->option_map[keycode];
			} else if (modifiers & B_CAPS_LOCK &&
			           modifiers & B_SHIFT_KEY)
			{
				charindex = keymap->caps_shift_map[keycode];
			} else if (modifiers & B_CAPS_LOCK) {
				charindex = keymap->caps_map[keycode];
			} else if (modifiers & B_SHIFT_KEY) {
				charindex = keymap->shift_map[keycode];
			} else { // I decided to cut the rest of the maps.
				charindex = keymap->normal_map[keycode];
			}
*/
// If you only want the key on the keyboard (the normal_map) then use this.
			charindex = keymap->normal_map[keycode];
			
			int charsize = chars[charindex++];
			switch(charsize) {
				case 0:
					// unmapped
					break;
				case 1:
					*keyLabel << chars[charindex];
					break;
				default:
					char *str = new char[charsize + 1];
					strncpy(str, &(chars[charindex]), charsize);
					*keyLabel << str;
					delete []str;
			}
			free(keymap);
			free(chars);
	}
}

void BamKeysKeyView::DrawTo(BView *view, BRect bounds, int32 key, bool pressed, const char* keyLabel = NULL, uint8 alpha = 255) {
	drawing_mode origMode = view->DrawingMode();
	view->SetDrawingMode(B_OP_ALPHA);
	rgb_color origHigh = view->HighColor();
	
	BRect outterBounds = bounds;
	
	BRect r = outterBounds;
	r.InsetBy(3, 3);
	
	rgb_color pen;
	MakeGrey(&pen, 0, alpha);
	view->SetHighColor(pen);
	view->StrokeRect(r, B_SOLID_HIGH);
	
	r.InsetBy(1, 1);
	if (! pressed) {
		view->BeginLineArray(19);
		
		MakeGrey(&pen, 64, alpha);
		view->SetHighColor(pen);
		view->AddLine(r.LeftBottom(), r.RightBottom(), pen);
		view->AddLine(r.RightBottom(), r.RightTop(), pen);
		view->AddLine(r.RightBottom(), r.RightBottom() - BPoint(1, 1), pen);
		
		// 200 level greys
		r.InsetBy(0.5, 0.5);
		r.OffsetBy(-0.5, -0.5);
		MakeGrey(&pen, 200, alpha);
		BPoint start = r.LeftTop();
		BPoint end = r.LeftBottom();
		view->AddLine(start, end, pen);
		
		end = start;
		end.x += 2;
		view->AddLine(start, end, pen);
		
		start = end;
		start.x += 1;
		start.y = r.LeftBottom().y - 1;
		end = r.RightBottom();
		end.x -= 2;
		end.y -= 1;
		view->AddLine(start, end, pen);
		
		
		start = end;
		start.x += 1;
		start.y -= 1;
		end = r.RightTop();
		end.y += 2;
		end.x -= 1;
		view->AddLine(start, end, pen);
		
		// 160 level greys (3 points)
		MakeGrey(&pen, 160, alpha);
		start = r.LeftBottom();
		start.x += 1;
		end = start;
		view->AddLine(start, end, pen);
		
		start = r.RightBottom();
		start.x -= 1;
		start.y -= 1;
		end = start;
		view->AddLine(start, end, pen);
		
		start = r.RightTop();
		start.y += 1;
		end = start;
		view->AddLine(start, end, pen);
		
		// 96 level greys
		MakeGrey(&pen, 96, alpha);
		
		start = r.LeftBottom();
		start.x += 2;
		end = r.RightBottom();
		end.x -= 1;
		view->AddLine(start, end, pen);
		
		start = r.RightBottom();
		start.y -= 1;
		end = r.RightTop();
		end.y += 2;
		view->AddLine(start, end, pen);
		
		// 184 level greys
		MakeGrey(&pen, 184, alpha);
		
		start = r.LeftTop();
		start.x += 3;
		end = start;
		end.x += 2;
		view->AddLine(start, end, pen);
		
		// 168 level greys
		MakeGrey(&pen, 168, alpha);
		
		start = end;
		start.x += 1;
		end = start;
		end.x += 2;
		view->AddLine(start, end, pen);
		
		// 152
		MakeGrey(&pen, 152, alpha);
		
		start = end;
		start.x += 1;
		end = r.RightTop();
		view->AddLine(start, end, pen);
		
		start = end;
		end.y += 1;
		view->AddLine(start, end, pen);
		
		// 255 (white)
		MakeGrey(&pen, 255, alpha);
		start = r.LeftTop();
		start.x += 1;
		start.y += 1;
		end.x -= 1;
		view->AddLine(start, end, pen);
		
		end = r.LeftBottom();
		end.y -= 1;
		end.x += 1;
		view->AddLine(start, end, pen);
		
		start = end;
		end.x += 1;
		view->AddLine(start, end, pen);
		
		view->EndLineArray();
		
		// Key fillings
		//   152
		MakeGrey(&pen, 152, alpha);
		view->SetHighColor(pen);
		BRect fillZone(r);
		fillZone.InsetBy(2, 2);
		
		end = r.LeftBottom();
		end.y -= 2;
		end.x += 7;
		fillZone.SetRightBottom(end);
		view->FillRect(fillZone);
		
		MakeGrey(&pen, 168, alpha);
		view->SetHighColor(pen);
		fillZone.OffsetBy(fillZone.Width(), 0);
		view->FillRect(fillZone);
		
		MakeGrey(&pen, 184, alpha);
		view->SetHighColor(pen);
		fillZone.OffsetBy(fillZone.Width(), 0);
		end = r.RightBottom();
		end.x -= 2;
		end.y -= 2;
		fillZone.SetRightBottom(end);
		view->FillRect(fillZone);
		
		MakeGrey(&pen, 0, alpha);
		view->SetHighColor(pen);
	} else {
		// PRESSED
		MakeGrey(&pen, 48, alpha);
		view->BeginLineArray(8);
		
		BPoint start = r.LeftBottom();
		BPoint end = r.LeftTop();
		view->AddLine(start, end, pen);
		
		start = end;
		end = r.RightTop();
		view->AddLine(start, end, pen);
		
		MakeGrey(&pen, 136, alpha);
		start = end + BPoint(0, 1);
		end = r.RightBottom();
		view->AddLine(start, end, pen);
		
		start = end;
		end = r.LeftBottom() + BPoint(1, 0);
		view->AddLine(start, end, pen);
		
		r.InsetBy(1, 1);
		
		MakeGrey(&pen, 72, alpha);
		start = r.LeftBottom();
		end = r.LeftTop();
		view->AddLine(start, end, pen);
		
		start = end;
		end = r.RightTop();
		view->AddLine(start, end, pen);
		
		MakeGrey(&pen, 152, alpha);
		start = end + BPoint(0, 1);
		end = r.RightBottom();
		view->AddLine(start, end, pen);
		
		start = end;
		end = r.LeftBottom() + BPoint(1, 0);
		view->AddLine(start, end, pen);
		
		view->EndLineArray();
		
		// Fill
		r.InsetBy(1, 1);
		
		MakeGrey(&pen, 112, alpha);
		view->FillRect(r);
		
		MakeGrey(&pen, 216, alpha);
		view->SetHighColor(pen);
	}
	
	if (keyLabel != NULL) {
		font_height fnt;
		be_fixed_font->GetHeight(&fnt);
		float stringHeight = fnt.ascent - fnt.descent;
		float stringWidth = view->StringWidth(keyLabel);
		r = outterBounds;
		BPoint strLoc((r.Width() / 2) - (stringWidth / 2), 
						r.Height() - ((r.Height() / 2) - (stringHeight / 2)));
		strLoc += r.LeftTop();
		view->DrawString(keyLabel, strLoc);
	}

	view->SetDrawingMode(origMode);
	view->SetHighColor(origHigh);
}

void BamKeysKeyView::GetIdealSize(BRect *rect, const char *label) {
	font_height fnt;
	be_fixed_font->GetHeight(&fnt);
	float stringHeight = fnt.ascent - fnt.descent;
	float stringWidth = be_fixed_font->StringWidth(label);
	stringHeight += 18;
	if (strlen(label) > 1) {
		stringWidth += 8;
	} else {
		stringWidth += 22;
	}
	rect->Set(0, 0, stringWidth, stringHeight);
}

void BamKeysKeyView::MakeGrey(rgb_color *color, uint8 level, uint8 alpha = 255) {
	color->red = level;
	color->green = level;
	color->blue = level;
	color->alpha = alpha;
}
