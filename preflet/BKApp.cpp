#include "BKApp.h"
#include <String.h>
#include <Alert.h>

int main(int argc, char** argv) {
	BamKeysApp *app = new BamKeysApp();
	app->Run();
	
	return (0);
}

BamKeysApp::BamKeysApp() : BApplication(BAMKEY_APP_SIG) {
	fSetWindow = new BamKeysPrefsWindow();
}

BamKeysApp::~BamKeysApp() {
	delete fSetWindow;
}

void BamKeysApp::ReadyToRun() {
	fSetWindow->Show();
}

void BamKeysApp::AboutRequested() {
	BString aboutMsg("");
	aboutMsg << "BamKeys\n"
	         << "Keyboard Navigation for BeOS PowerUsers\n\n"
	         << "©2005 AspieWare : Jonathan Thompson, Bryan Varner\n";
	
	BAlert *aboutBox = new BAlert(NULL, aboutMsg.String(), "Close");
	aboutBox->Go();
}
