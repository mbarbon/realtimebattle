/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-1999  Erik Ouchterlony and Ragnar Ouchterlony

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>

#include "Extras.h"
#include "ArenaController.h"
#include "ArenaRealTime.h"
#include "ArenaWindow.h"
#include "Options.h"

Extras::Extras(const Vector2D& c, const double r, const double e, const long int col)
  : Circle(c, r, col), energy(e)
{
  alive = true;
#ifndef NO_GRAPHICS
  if( !no_graphics )
    draw_shape( false );
#endif
}

Cookie::Cookie(const Vector2D& c, const double r, const double e) 
  : Extras(c, r, e, the_opts.get_l(OPTION_COOKIE_COLOUR))
{ 
   id = the_arena.increase_cookie_count();
   log_file_char = 'C';
}

Mine::Mine(const Vector2D& c, const double r, const double e)
  : Extras(c, r, e, the_opts.get_l(OPTION_MINE_COLOUR))
{
   id = the_arena.increase_mine_count();
   log_file_char = 'M';
}


void
Extras::die()
{
   alive = false;
#ifndef NO_GRAPHICS
   if (!no_graphics )
     the_gui.get_arenawindow_p()->
       draw_circle( last_drawn_center, last_drawn_radius,
                    *(the_arena.get_bg_gdk_colour_p()), true );
#endif

   if( the_arena_controller.is_realtime() )
     realtime_arena.print_to_logfile( 'D', log_file_char, id );
}
