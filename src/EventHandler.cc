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
#include <config.h>
#endif

#include <queue>


#include "EventHandler.h"
#include "Event.h"
#include "Timer.h"
#include "Various.h"

EventHandler::EventHandler()
{
  finished = false;
}

EventHandler::~EventHandler()
{
}




void
EventHandler::main_loop()
{
  double time_for_next_event;
  struct timeval time_to_wait;
  const Event* next_eventp;

  do
    {

      if( event_queue.empty() )
        Error(true, "Event queue unexpectedly empty", "EventHandler::eval_next_event");
  
      next_eventp = event_queue.top();

      for(;;) 
        {
          time_for_next_event = next_eventp->get_time();
          //          timer.double2timeval( time_for_next_event, time_to_wait );          

          if( time_to_wait.tv_usec == 0 ) break;    // Time to eval next event
          
          select(FD_SETSIZE, NULL, NULL, NULL, &time_to_wait);
        }

      current_time = timer.get();

      next_eventp->eval();
      delete next_eventp;

      event_queue.pop();
  
    }  while( !finished );

}

//
// Call this function with
//
// insert_event( new FooEvent( args ) );
//
// The event is deleted after evaluation.
void 
EventHandler::insert_event( Event* ev )
{
  event_queue.push( ev );
}

void
EventHandler::quit()
{
  insert_event( new QuitEvent( get_time(), this ) );
}
