#ifndef PRIVATE_WINDOW
#define PRIVATE_WINDOW
/****************************************************************************
** WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING **
**                                                                         **
**                          DANGER, WILL ROBINSON!                         **
**                                                                         **
** The rest of the interfaces contained here are part of BeOS's            **
**                                                                         **
**                     >> PRIVATE NOT FOR PUBLIC USE <<                    **
**                                                                         **
**                                                       implementation.   **
**                                                                         **
** These interfaces              WILL CHANGE        in future releases.    **
** If you use them, your app     WILL BREAK         at some future time.   **
**                                                                         **
** (And yes, this does mean that binaries built from OpenTracker will not  **
** be compatible with some future releases of the OS.  When that happens,  **
** we will provide an updated version of this file to keep compatibility.) **
**                                                                         **
** WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING **
****************************************************************************/

// from interface_defs.h
struct window_info {
	team_id		team;
    int32   	id;	  		  /* window's token */

	int32		thread;
	int32		client_token;
	int32		client_port;
	uint32		workspaces;

	int32		layer;
    uint32	  	w_type;    	  /* B_TITLED_WINDOW, etc. */
    uint32      flags;	  	  /* B_WILL_FLOAT, etc. */
	int32		window_left;
	int32		window_top;
	int32		window_right;
	int32		window_bottom;
	int32		show_hide_level;
	bool		is_mini;
	char		name[1];
};

// from interface_misc.h
enum window_action {
	B_MINIMIZE_WINDOW,
	B_BRING_TO_FRONT
};

// from interface_misc.h
void		do_window_action(int32 window_id, int32 action, 
							 BRect zoomRect, bool zoom);
window_info	*get_window_info(int32 a_token);
int32		*get_token_list(team_id app, int32 *count);
void do_minimize_team(BRect zoomRect, team_id team, bool zoom);
void do_bring_to_front_team(BRect zoomRect, team_id app, bool zoom);


#endif /* PRIVATE_WINDOW */
