/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony

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

#include <list>
#include <algorithm>

#include "ArenaController.h"
#include "EventRT.h"
#include "EventGT.h"
#include "EventHandler.h"
#include "Tournament.h"
#include "ServerSocket.h"

extern ArenaController the_arena_controller;

void
CheckSocketEvent::eval() const
{
  //cout<<"CheckSocketEvent::eval()\n";
  server_p->check_socket();
  Event* next_event = new CheckSocketEvent(refresh, server_p);
  the_eventhandler.insert_RT_event(next_event);
}

void
StartTournamentEvent::eval() const 
{
  //cout<<"StartTournamentEvent::eval() const \n";
  Tournament* t = new Tournament();
  /*
    t = new Tournament(filename);

  cout<<"Tournament created\n";
  if( !t->did_load_succeed() )
    {
      Error(true, "Couldn't load tournament file", "StartTournamentEvent::evel");
    }
  */

  the_eventhandler.set_tournament( t );
  //  Event* next_event = new CheckTournamentNegociationEvent(refresh, refresh, t);
  //  the_eventhandler.insert_RT_event(next_event);
}

void
PrepareForNewMatchEvent::eval() const
{
  //cout<<"PrepareForNewMatchEvent::eval() const\n";
  assert( count_down >= 0 );

  if( (! the_arenap->all_robots_ready()) && (count_down != 0) )
    {
      Event* next_event = new PrepareForNewMatchEvent(eval_time, count_down - 1, my_tournament);
      the_eventhandler.insert_RT_event(next_event);
    }
  else
    {
      cout<<"Starting "<<count_down<<endl;
      the_arenap->start();
      the_eventhandler.insert_GT_event( new RobotsUpdateEvent( 0, 0.1 ) );
      the_eventhandler.insert_GT_event( new ShotsUpdateEvent( 0, 0.1 )  );
    }

}

void
StartNewMatchEvent::eval() const
{
  cout<<"StartNewMatchEvent::eval() const\n";
  //my_match->start_new_match();
}


void
EndMatchEvent::eval() const
{
  //cout<<"EndMatchEvent::eval() const\n";
  my_match->end_match();
}
