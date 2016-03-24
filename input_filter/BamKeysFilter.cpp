#include <add-ons/input_server/InputServerFilter.h>
#include <Beep.h>

#include "BamKeysFilter.h"

extern "C" _EXPORT BInputServerFilter* instantiate_input_filter();

BInputServerFilter* instantiate_input_filter() {
	return (new BamKeysFilter());
}

BamKeysFilter::BamKeysFilter() : BHandler("BamKeys") {
	fDoFilter = true;
	fMsgPort = new MessagePort(BAMKEY_PORT_NAME, true, 16, this);
	
	fRegion = new BRegion();
	
	fState = new BamKeysState(0, 0);
	fInfoDisplay = new InfoWindow(fState);

	ReadPrefs(new Prefs(BAMKEY_PREFERENCES_NAME));
}

BamKeysFilter::~BamKeysFilter() {
	delete fRegion;
	delete fMsgPort;
	if (fInfoDisplay->Lock()) {
		fInfoDisplay->Quit();
	}
	fRegion = NULL;
	delete fState;
}

/**
 * Loads up the preferences from the given file.
 */
void BamKeysFilter::ReadPrefs(BMessage *prefs) {
	int32 totalRows, totalColumns;
	if (prefs->FindInt32("rows", &totalRows) == B_OK &&
	    prefs->FindInt32("columns", &totalColumns) == B_OK)
	{
		fState->SetSize(totalRows, totalColumns);
	} else {
		fState->SetSize(0, 0);
	}
	
	int32 gridKey, gridMod;
	if (prefs->FindInt32("grid_key", &gridKey) == B_OK &&
	    prefs->FindInt32("grid_modifier", &gridMod) == B_OK)
	{
		fState->SetGridCombo(gridKey, gridMod);
	} else {
		fState->SetGridCombo(0, 0);
	}
	
	for (int32 i = 0; i < (totalRows * totalColumns); i++) {
		prefs->FindInt32("key", i, &(fState->fKeys[i]));
		prefs->FindInt32("modifier", i, &(fState->fModKeys[i]));
	}
	
	if (prefs->FindBool("enabled", &fDoFilter) != B_OK) {
		fDoFilter = true;
	}
	
	fRow = -1;
	fColumn = -1;
	fMatches = 0;
}

filter_result BamKeysFilter::Filter(BMessage *message, BList *outList) {
	if (fDoFilter) {
		
		int32 key;
		int32 modifiers;
		if (message->FindInt32("key", &key) == B_OK &&
		    message->FindInt32("modifiers", &modifiers) == B_OK)
		{
			bool match = false;
			
			// Check window-selector hotkeys.
			if (message->what == B_KEY_DOWN) {
				int32 totalRows, totalColumns;
				fState->GetSize(&totalRows, &totalColumns);
				
				for (int column = 0; column < totalColumns; column++) {
					for (int row = 0; row < totalRows; row++) {
						if (key == fState->fKeys[(row * totalColumns) + column] &&
						    modifiers == fState->fModKeys[(row * totalColumns) + column])
						{
							match = true;
							if (row == fRow && column == fColumn) {
								fMatches++;
							} else {
								fRow = row;
								fColumn = column;
								
								fRegion->Set(BRect(
										(fState->GetScreenSize().Width() / totalColumns * column),
										(fState->GetScreenSize().Height() / totalRows * row),
										(fState->GetScreenSize().Width() / totalColumns * (column + 1)),
										(fState->GetScreenSize().Height() / totalRows * (row + 1))));
								
								fMatches = 1;
							}
							break;
						}
					}
					if (match) {
						break;
					}
				}
				
				if (match) {
					int32 currentWorkspace = current_workspace();
		
					int32 tokenCount;
					int32 *tokens = get_token_list(-1, &tokenCount);
					
					int32 canidates = 0;
					do {
						// If we've had to repeat this loop we want the next available window
						if (canidates > 0) {
							fMatches = 2;
							canidates = 0;
						}
						
						// By starting at 1, we're skipping index 0 (currently front-most window)
						for (int32 i = 1; i < tokenCount; i++) {
							window_info *window = get_window_info(tokens[i]);
							if (isTargetable(window, currentWorkspace)) {
								canidates++;
								if (canidates == fMatches) {
									do_window_action(window->id, B_BRING_TO_FRONT, BRect(0, 0, 0, 0), false);
								}
							}
							free(window);
						}
					} while (fMatches > canidates && canidates > 1);
					free(tokens);
				} else {
					fMatches = 0;
				}
			}
			
			int32 gridKey, gridMod;
			fState->GetGridCombo(&gridKey, &gridMod);
			// Check for Grid-Display Modifiers
			if (key == gridKey &&
				modifiers == gridMod)
			{
				if (message->what == B_KEY_DOWN) {
					fInfoDisplay->Show(BAMKEY_SHOW_GRID);
				} else {
					fInfoDisplay->Hide();
				}
			}
		}
	}
	return B_DISPATCH_MESSAGE;
}

/**
 * Handles incomming messages from the MessagePort being delivered to this Filter.
 */
void BamKeysFilter::MessageReceived(BMessage *message) {
	switch (message->what) {
		case BAMKEY_STOP_FILTER:
			fDoFilter = false;
			break;
		case BAMKEY_START_FILTER:
			fDoFilter = true;
			break;
		case BAMKEY_UPDATE_FILTER:
			ReadPrefs(message);
			break;
		default:
			BHandler::MessageReceived(message);
	}
}

/**
 * boolean filter to determine if a window is targetable from the current workspace.
 * 
 * If the window lives on a non-active workspace, is hidden, or otherwise shouldn't 
 * activatable from BamKeys, we return false. As if you didn't figure that out from
 * the name of the function.
 */
bool BamKeysFilter::isTargetable(window_info *window, int32 currentWorkspace) {
	bool targetable = false;
	/* 
	   According to the OpenTracker source (Specifically in Switcher.cpp)
	   is_mini is true if minimized.
	   show_hide_level is <= 0 when visible, >= 1 when not.
	   w_type should be pretty self explanatory.
	*/
	
	// On this workspace, not minimized, and visible.
	if (((window->workspaces & (1 << currentWorkspace)) != 0) &&
		(! window->is_mini && window->show_hide_level <= 0))
	{
		BRect wndRect(window->window_left, 
		              window->window_top, 
		              window->window_right, 
		              window->window_bottom);
		
		targetable = ((window->w_type == kNormalWindow || window->w_type == kWindowScreen) &&
		              fRegion->Intersects(wndRect));
	}
	return targetable;
}
