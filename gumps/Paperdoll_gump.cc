/*
Copyright (C) 2000 The Exult Team

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

#include "game.h"
#include "gamewin.h"
#include "Gump_button.h"
#include "misc_buttons.h"
#include "Paperdoll_gump.h"
#include "contain.h"

/*
 *
 *	SERPENT ISLE GUMPS
 *
 */


/*
 *	Statics:
 */

// Paperdoll is completely different to Actor
short Paperdoll_gump::diskx = 123, Paperdoll_gump::disky = 137;
short Paperdoll_gump::heartx = 98, Paperdoll_gump::hearty = 137;
short Paperdoll_gump::combatx = 51, Paperdoll_gump::combaty = 142;
short Paperdoll_gump::cstatx = 73, Paperdoll_gump::cstaty = 137;
short Paperdoll_gump::coords[26] = {
	114, 10,	/* ears */	115, 24,	/* back */
	115, 37,	/* belt */	37, 56,		/* lhand */
	115, 71,	/* lfinger */	114, 85,	/* legs */
	76, 98,		/* feet */	35, 70,		/* rfinger */
	115, 55,	/* rhand */	37, 37,		/* torso */
	37, 24,		/* head */	37, 11,		/* ammo */
	57, 51		/* back2 */
	};

/*
 *	The Combat Stats Button.
 */
class Cstats_button : public Gump_button
{
public:
	Cstats_button(Gump *par, int px, int py)
		: Gump_button(par, game->get_shape("gumps/combat_stats"), px, py)
		{  }
					// What to do when 'clicked':
	virtual void activate(Game_window *gwin);
};

/*
 *	Handle click on a combat stats button
 */

void Cstats_button::activate
	(
	Game_window *gwin
	)
{
					// ++++++Later.
}


/*
 *	Find the index of the closest 'spot' to a mouse point.
 *
 *	Output:	Index, or -1 if unsuccessful.
 */

int Paperdoll_gump::find_closest
	(
	int mx, int my,			// Mouse point in window.
	int only_empty			// Only allow empty spots.
	)
{
	mx -= x; my -= y;		// Get point rel. to us.
	long closest_squared = 1000000;	// Best distance squared.
	int closest = -1;		// Best index.
	for (size_t i = 0; i < sizeof(coords)/(2*sizeof(coords[0])); i++)
	{
		int dx = mx - spotx(i), dy = my - spoty(i);
		long dsquared = dx*dx + dy*dy;
					// Better than prev.?
		if (dsquared < closest_squared && (!only_empty ||
						!container->get_readied(i)))
		{
			closest_squared = dsquared;
			closest = i;
		}
	}
	return (closest);
}

/*
 *	Create the gump display for an actor.
 */

Paperdoll_gump::Paperdoll_gump
	(
	Container_game_object *cont,	// Container it represents.
	int initx, int inity, 		// Coords. on screen.
	int shnum			// Shape #.
	) : Gump(cont, initx, inity, 123, true)
{
	heart_button = new Heart_button(this, heartx, hearty);
	if (cont->get_npc_num() == 0)
		disk_button = new Disk_button(this, diskx, disky);
	else
		disk_button = NULL;
	if (cont->get_npc_num() == 0)
		combat_button = new Combat_button(this, combatx, combaty);
	else
		combat_button = NULL;
	cstats_button = new Cstats_button(this, cstatx, cstaty);
							
	for (size_t i = 0; i < sizeof(coords)/2*sizeof(coords[0]); i++)
	{			// Set object coords.
		Game_object *obj = container->get_readied(i);
		if (obj)
			set_to_spot(obj, i);
	}
}

/*
 *	Delete actor display.
 */

Paperdoll_gump::~Paperdoll_gump
	(
	)
{
	if (heart_button)
		delete heart_button;
	if (disk_button)
		delete disk_button;
	if (combat_button)
		delete combat_button;
	if (cstats_button)
		delete cstats_button;
}

/*
 *	Is a given screen point on one of our buttons?
 *
 *	Output: ->button if so.
 */

Gump_button *Paperdoll_gump::on_button
	(
	Game_window *gwin,
	int mx, int my			// Point in window.
	)
{
	Gump_button *btn = Gump::on_button(gwin, mx, my);
	if (btn)
		return btn;
	else if (heart_button && heart_button->on_button(gwin, mx, my))
		return heart_button;
	else if (disk_button && disk_button->on_button(gwin, mx, my))
		return disk_button;
	else if (combat_button && combat_button->on_button(gwin, mx, my))
		return combat_button;
	else if (cstats_button && cstats_button->on_button(gwin, mx, my))
		return cstats_button;
	return 0;
}

/*
 *	Add an object.
 *
 *	Output:	0 if cannot add it.
 */

int Paperdoll_gump::add
	(
	Game_object *obj,
	int mx, int my,			// Screen location of mouse.
	int sx, int sy			// Screen location of obj's hotspot.
	)
{
					// Find index of closest spot.
	int index = find_closest(mx, my);
	if (!container->add_readied(obj, index))
	{			// Can't add it there?
					// Try again for an empty spot.
		index = find_closest(mx, my, 1);
		if (index < 0 || !container->add_readied(obj, index))
					// Just try to add it.
			if (!container->add(obj))
				return (0);
	}
					// In case it went in another obj:
	index = container->find_readied(obj);
	if (index >= 0)
		set_to_spot(obj, index);// Set obj. coords.
	return (1);
}

/*
 *	Set object's coords. to given spot.
 */

void Paperdoll_gump::set_to_spot
	(
	Game_object *obj,
	int index			// Spot index.
	)
{
	Game_window *gwin = Game_window::get_game_window();
					// Get shape info.
	Shape_frame *shape = gwin->get_shape(*obj);
	int w = shape->get_width(), h = shape->get_height();
					// Set object's position.
	obj->set_chunk(spotx(index) + shape->get_xleft() - w/2 - object_area.x,
		spoty(index) + shape->get_yabove() - h/2 - object_area.y);
					// Shift if necessary.
	int x0 = obj->get_cx() - shape->get_xleft(), 
	    y0 = obj->get_cy() - shape->get_yabove();
	int newcx = obj->get_cx(), newcy = obj->get_cy();
	if (x0 < 0)
		newcx -= x0;
	if (y0 < 0)
		newcy -= y0;
	int x1 = x0 + w, y1 = y0 + h;
	if (x1 > object_area.w)
		newcx -= x1 - object_area.w;
	if (y1 > object_area.h)
		newcy -= y1 - object_area.h;
	obj->set_chunk(newcx, newcy);
}

/*
 *	Paint on screen.
 */

void Paperdoll_gump::paint
	(
	Game_window *gwin
	)
{
					// Watch for any newly added objs.
	for (size_t i = 0; i < sizeof(coords)/2*sizeof(coords[0]); i++)
	{			// Set object coords.
		Game_object *obj = container->get_readied(i);
		if (obj && !obj->get_cx() && !obj->get_cy())
			set_to_spot(obj, i);
	}
	Gump::paint(gwin);	// Paint gump & objects.
					// Paint buttons.
	if (heart_button)
		paint_button(gwin, heart_button);
	if (disk_button)
		paint_button(gwin, disk_button);
	if (combat_button)
		paint_button(gwin, combat_button);
	if (cstats_button)
		paint_button(gwin, cstats_button);
}
