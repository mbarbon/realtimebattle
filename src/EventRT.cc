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

#include <pthread.h>
#include <list>
#include <algorithm>

#include "ArenaController.h"
#include "EventRT.h"
#include "EventHandler.h"
#include "GuiInterface.h"
#include "Tournament.h"


    ///////////////////////////////////////////////////
   //                                               //
  //             CheckGUIEvent                     //
 //                                               //
///////////////////////////////////////////////////

extern list<unsigned int> GI_exit_list;
extern pthread_mutex_t the_mutex;
extern ArenaController the_arena_controller;

void
CheckGUIEvent::eval() const 
{
  // Note: might have to supply process_all_options() with eventhandler in the future.
  gui_p->process_all_requests();

  pthread_mutex_lock( &the_mutex );
  list<unsigned int>::iterator li;
  if( (li = find( GI_exit_list.begin(), GI_exit_list.end(), gui_p->get_unique_id() ))
      != GI_exit_list.end() )
    {
      the_arena_controller.exit_gui( *li );
      GI_exit_list.erase( li );
      pthread_mutex_unlock( &the_mutex );
      return;
    }
  pthread_mutex_unlock( &the_mutex );

  Event* next_event = new CheckGUIEvent(refresh, refresh, gui_p);
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

void
PrepareForNewMatchEvent::eval() const
{
  my_tournament->prepare_for_new_match();
}

void
StartNewMatchEvent::eval() const
{
  my_match->start_new_match();
}


void
EndMatchEvent::eval() const
{
  my_match->end_match();
}
