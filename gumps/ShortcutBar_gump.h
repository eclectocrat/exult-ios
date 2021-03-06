/*
Copyright (C) 2011-2013 The Exult Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _SHORTCUTBAR_GUMP_H
#define _SHORTCUTBAR_GUMP_H


#include "SDL.h"
#include "gamewin.h"
#include "../objs/objs.h"
#include "misc_buttons.h"
#include "Modal_gump.h"
#include "Gamemenu_gump.h"

#define SHORTCUT_BAR_USER_EVENT 0x53425545 // "SBUE"

/* -------------------------------------------- */

typedef enum {
	SB_ITEM_DISK,
	SB_ITEM_TOGGLE_COMBAT,
	SB_ITEM_MAP,
	SB_ITEM_SPELLBOOK,
	SB_ITEM_BACKPACK,
	SB_ITEM_KEY,
	SB_ITEM_KEYRING,
	SB_ITEM_NOTEBOOK,
	SB_ITEM_TARGET,
	SB_ITEM_JAWBONE,
	SB_ITEM_FEED,
} ShortcutBarButtonItemType;

struct ShortcutBarButtonItem {
	const char *name;
	ShortcutBarButtonItemType type;
	ShapeID *shapeId;
	Rectangle *rect; /* coordinate values related to shortcut bar */
	int shapeOffsetX;
	int shapeOffsetY;
	bool pushed;
	bool translucent;
};

#define MAX_SHORTCUT_BAR_ITEMS 10

class ShortcutBar_gump: public Gump {
public:
	ShortcutBar_gump(int placex = 0, int placey = 0);
	~ShortcutBar_gump();
	int handle_event(SDL_Event *event);
	void paint();

	// Don't close on end_gump_mode
	virtual bool is_persistent() const {
		return true;
	}
	// Can't be dragged with mouse
	virtual bool is_draggable() const {
		return false;
	}
	// Show the hand cursor
	virtual bool no_handcursor() const {
		return true;
	}

	int startx;
	int resx;
	int gamex;
	int starty;
	int resy;
	int gamey;
	void onUserEvent(SDL_Event *event);
// add dirty region, if dirty
	virtual void update_gump();
	void set_changed() { has_changed = true; }
	void check_for_updates(int shnum);

private:
	ShortcutBarButtonItem buttonItems[MAX_SHORTCUT_BAR_ITEMS];
	int numButtons;
	int lastClickedButton;
	SDL_TimerID timerId;
	Game_object *is_party_item(int shnum, int frnum = c_any_framenum,
	                   int qual = c_any_qual);
	void createButtons();
	void deleteButtons();
	void onItemClicked(int index, bool doubleClicked);
	void mouse_down(SDL_Event *event, int mx, int my);
	void mouse_up(SDL_Event *event, int mx, int my);
	bool has_changed;

	int locx;
	int locy;
	int width;
	int height;
};

class Gump_button;
typedef std::vector<Gump_button *> Gump_button_vector;
typedef std::map<Game_object *, int *> Game_object_map_xy;

#endif
