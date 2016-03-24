#ifndef BAMKEY_INFO_WINDOW_H
#define BAMKEY_INFO_WINDOW_H

#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <Screen.h>
#include <InterfaceDefs.h>

#include "BKFilterState.h"
#include "BamKeysConstants.h"

class DrawView; // Forward Declaration

/**
 * Window that contains the view
 */
class InfoWindow : public BWindow {
	public:
		InfoWindow(BamKeysState *filterstate);
		~InfoWindow();
		
		friend class DrawView;
		
		virtual void Show(info_mode type);
		virtual void Hide();
		virtual void WorkspaceActivated(int32 workspace, bool active);
	private:
		BamKeysState *fFilter;
		
		BView *fView;
		BBitmap *fScreenshot;
		
		info_mode fCurrentMode;
};

/**
 * View that handles all the rendering of information
 */
class DrawView : public BView {
	public:
		DrawView();
		~DrawView();
		
		virtual void Draw(BRect update);
	private:
		void DrawGrid(InfoWindow *window);
		void DrawOutline(InfoWindow *window);
};

#endif /* BAMKEY_INFO_WINDOW_H */