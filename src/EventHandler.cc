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
//#include "Various.h"

EventHandler::EventHandler()
{
  current_time = 0;
  finished = false;
  pausedTime = 0;
  paused = false;
  NbRTEvent = NbGTEvent = 0;
}

EventHandler::~EventHandler()
{
}




void
EventHandler::main_loop()
{
  double time_for_next_event;
  //struct timeval time_to_wait;
  const Event* next_RTeventp;   //Next Event that is in realtime queue
  const Event* next_GTeventp;   //Next Event that is in gametime queue
  const Event* next_eventp;     //The one of the two we realy have to do...
  bool RTEvent = true;          //Say that the event cames from the RT queue

  do
    {
      //Should we use the same algorithm twice, or should we test RT & GT at the same time (what I'm doing here)
      
      if( RT_event_queue.empty() || GT_event_queue.empty())
        {
	  cout<<"Erreur Irreversible...\n";
	  exit(0);
	}

      //	Error(true, "Event queue unexpectedly empty", "EventHandler::eval_next_event");
      
      next_RTeventp = RT_event_queue.top();
      if(!paused)
	{//Select the one to execute...
	  next_GTeventp = GT_event_queue.top();
	  if((next_GTeventp->get_time() - pausedTime) < (next_RTeventp->get_time()))
	    {
	      RTEvent = false;
	      next_eventp = next_GTeventp;
	    }
	  else
	    {
	      next_eventp = next_RTeventp;
	    }
	}
      else
	{//The game is paused, only the RT event have to be executed
	  next_eventp = next_RTeventp;
	}


      for(;;) 
        {
          time_for_next_event = next_eventp->get_time();

          //          timer.double2timeval( time_for_next_event, time_to_wait );          

          current_time = timer.get_realtime();

	  if(time_for_next_event - current_time < 0) break;

          //if( time_to_wait.tv_usec == 0 ) break;    // Time to eval next event
          
          //select(FD_SETSIZE, NULL, NULL, NULL, &time_to_wait);
        }

      current_time = timer.get_realtime();

      next_eventp->eval();
      delete next_eventp;

      if(RTEvent)
	{
	  cout<<"Poping from RTEventQueue\n";
	  RT_event_queue.pop();
	  NbRTEvent--;
	}
      else
	{
	  cout<<"Poping from GTEventQueue\n";
	  GT_event_queue.pop();
	  NbGTEvent--;
	}
  
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
  RT_event_queue.push( ev );
  cout<<"There are "<<++NbRTEvent<<" events in the RT queue\n";
}


void 
EventHandler::insert_RT_event (Event* ev)
{
  RT_event_queue.push( ev );
  cout<<"There are "<<++NbRTEvent<<" events in the RT queue\n";
}

void 
EventHandler::insert_GT_event (Event* ev)
{
  GT_event_queue.push( ev );
  cout<<"There are "<<++NbGTEvent<<" events in the GT queue\n";
}


void
EventHandler::pauseGame()
{
  if(!paused)
    {
      pauseStarted.reset();
    }
  else
    {
      pausedTime += pauseStarted.get_realtime();   //How long the pause was.
    }
  paused = !paused;
}


void
EventHandler::quit()
{
  insert_event( new QuitEvent( get_time(), this ) );
}


double 
EventHandler::get_time() 
{ 
  return current_time; 
}








