#ifndef BAMKEY_KEY_VIEW
#define BAMKEY_KEY_VIEW

#include <Control.h>
#include <InterfaceDefs.h>
#include <Message.h>
#include <Rect.h>
#include <String.h>

class BamKeysKeyView : public BView {
	public:
		BamKeysKeyView(BRect frame, const char* name, int32 key, 
		               uint32 resizeMode = B_FOLLOW_LEFT | B_FOLLOW_TOP, 
		               uint32 flags = B_WILL_DRAW);
		~BamKeysKeyView();
		
		virtual void AttachedToWindow();
		
		void SetKey(int32 key, int32 modifiers = 0);
		void SetTransparency(uint8 alpha);
		
		void Update(int32 keys);
		virtual void Draw(BRect update);
		
		static void GetIdealSize(BRect *rect, const char *label);
		static void GetKeyLabel(BString *keyLabel, int32 keycode, int32 modifiers = 0);
		static void DrawTo(BView* view, BRect bounds, int32 key, 
		                   bool pressed, const char* keyLabel = NULL, uint8 alpha = 255);
		static void MakeGrey(rgb_color *color, uint8 level, uint8 alpha = 255);
	private:
		int32 fKey;
		bool fPressed;
		BString *fKeyLabel;
		uint8 fTransparency;
};

#endif BAMKEY_KEY_VIEW