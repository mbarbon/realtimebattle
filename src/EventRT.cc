/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2000  Erik Ouchterlony and Ragnar Ouchterlony

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
# include <config.h>
#endif

#include "EventRT.h"
#include "EventHandler.h"

#include "Tournament.h"

    ///////////////////////////////////////////////////
   //                                               //
  //             CheckGUIEvent                     //
 //                                               //
///////////////////////////////////////////////////


void
CheckGUIEvent::eval() const 
{
  // Note: might have to supply process_all_options() with eventhandler in the future.
  //  gui_p->process_all_options();

  Event* next_event = new CheckGUIEvent(eval_time + refresh, refresh, gui_p);
  the_eventhandler.insert_RT_event(next_event);
}




void
StartTournamentEvent::eval() const 
{
  Tournament* t = new Tournament(filename);

  if( !t->did_load_succeed() )
    {
      Error(true, "Couldn't load tournament file", "StartTournamentEvent::evel");
    }

  the_eventhandler.set_tournament( t );
}
