#ifndef BAMKEY_CONSTANTS
#define BAMKEY_CONSTANTS

#define BAMKEY_APP_SIG "application/x-vnd.AspieWare-BamKeys"

#define BAMKEY_PORT_NAME "BamKeys_Ctrl"
#define BAMKEY_PREFERENCES_NAME "BamKeys"

#define BAMKEY_STOP_FILTER 'STOP'
#define BAMKEY_START_FILTER 'STRT'
#define BAMKEY_UPDATE_FILTER 'Pref'

enum info_mode {
	BAMKEY_ERROR,
	BAMKEY_SHOW_GRID,
	BAMKEY_OUTLINE_ZONE
};

#endif /* BAMKEY_CONSTANTS */