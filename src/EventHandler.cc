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
#include "Tournament.h"

EventHandler::EventHandler()
{
  current_time = 0.0;
  current_game_time = 0.0;
  game_speed_before_pause = 0.0;

  the_tournament = NULL;
  nb_RT_event = 0;
  nb_GT_event = 0;
}

EventHandler::~EventHandler()
{
  if( the_tournament != 0 )
    delete the_tournament;

}




void
EventHandler::main_loop()
{
  double time_for_next_event = 0.0;
  double tm;

  struct timeval time_to_wait;

  const Event* next_RTeventp;   //Next Event that is in realtime queue
  const Event* next_GTeventp;   //Next Event that is in gametime queue
  const Event* next_eventp;     //The one of the two we realy have to do...
  bool RT_event;          //Say that the event cames from the RT queue

  finished = false;

  do
    {
      RT_event = true;

      if( RT_event_queue.empty() && GT_event_queue.empty())
        {
          finished = true;
          break;
	}


      if( RT_event_queue.empty() )
        {
          next_eventp = GT_event_queue.top();
          RT_event = false;
          time_for_next_event = timer.gametime2realtime( next_eventp->get_time() );
        }
      else if( GT_event_queue.empty() )
        {
          next_eventp = RT_event_queue.top();
          RT_event = true;
          time_for_next_event = next_eventp->get_time();
        }
      else
        {
          next_RTeventp = RT_event_queue.top();
	  next_GTeventp = GT_event_queue.top();

          time_for_next_event = next_RTeventp->get_time();
          tm = timer.gametime2realtime( next_GTeventp->get_time() );

          if( time_for_next_event < tm )
            {
              RT_event = true;
              next_eventp = next_RTeventp;
            }
          else
            {
              RT_event = false;
              next_eventp = next_GTeventp;
              time_for_next_event = tm;
            }
        }  
        
      
      for(;;) 
        {
          timer.double2timeval( time_for_next_event, time_to_wait );

          //          current_time = timer.get_realtime();

	  //if(time_for_next_event - current_time < 0) break;

          if( time_to_wait.tv_usec == 0 && time_to_wait.tv_sec == 0 ) 
            break;    // Time to eval next event
          
          select(FD_SETSIZE, NULL, NULL, NULL, &time_to_wait);
        }

      current_time = time_for_next_event;
      current_game_time = timer.realtime2gametime( current_time );

      cout << "Current time: " << current_time;

      if(RT_event)
	{
	  //cout<<"Poping from RT_event_queue" << endl;
	  RT_event_queue.pop();
          nb_RT_event--;
	}
      else
	{
	  //cout<<"Poping from GT_event_queue" << endl;
	  GT_event_queue.pop();
          nb_GT_event--;
	}

      next_eventp->eval();      

      delete next_eventp;
  
    } while( !finished );

  // cleanup..

}

//
// Call these function with
//
// insert_?T_event( new FooEvent( args ) );
//
// The event is deleted after evaluation.


void 
EventHandler::insert_RT_event (Event* ev, bool absolute_time)
{ 
  if( !absolute_time ) ev->add_time(current_time);
  RT_event_queue.push( ev );
}

void 
EventHandler::insert_GT_event (Event* ev, bool absolute_time)
{
  if( !absolute_time ) ev->add_time(current_game_time);
  GT_event_queue.push( ev );
}

void
EventHandler::pause_game()
{
  if( timer.get_game_speed() == 0.0 )
    {
      set_game_speed( game_speed_before_pause );
    }
  else
    {
      game_speed_before_pause = timer.get_game_speed();
      set_game_speed( 0.0 );
    }
}

void
EventHandler::set_game_speed( const double speed )
{
  timer.set_game_speed( speed );
}

void
EventHandler::quit()
{
  finished = true;
}

void
EventHandler::set_tournament( Tournament* const t )
{
  if( the_tournament != NULL )
    {
      delete the_tournament;
      the_tournament = NULL;
    }

  the_tournament = t;
}







