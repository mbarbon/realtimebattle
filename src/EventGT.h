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

#ifndef __EventGT__
#define __EventGT__

#include "Event.h"


class EventHandler;
class Arena;


class QuitEvent : public Event
{
 public:
  QuitEvent( const double time, EventHandler* ev) 
    : Event(time, ev) {}
  
  void eval() { my_event_handler->finish(); }
};

//All the 'recursive' update event should have the same 'face'
class RobotsUpDateEvent : public Event
{
 public:
  RobotsUpDateEvent(const double time, Arena* ar, EventHandler* ev)          //for compatibility if we need some...
    : Event(time, ev), my_arena(ar), refresh(time), NoMoreThan(0) {}

  RobotsUpDateEvent(const double time, const double StopAt, Arena* ar, EventHandler* ev)
    : Event(time, ev), my_arena(ar), refresh(time), NoMoreThan(StopAt) {}

  RobotsUpDateEvent(const double time, const double StopAt, const double UpDateDelay, Arena* ar, EventHandler* ev)
    : Event(time, ev), my_arena(ar), refresh(UpDateDelay), NoMoreThan(StopAt) {}
  
  void eval() const 
    {
      //      my_arena->update_robots();
      NextEvent();
    }

 protected:
  void NextEvent() const
    {
      if((NoMoreThan != 0)&&((eval_time+refresh < NoMoreThan)))
	{//Add the next event if the game isn't finished...
	  
	  Event * NextUpDate = new RobotsUpDateEvent(eval_time + refresh, refresh, NoMoreThan, my_arena, my_event_handler);
	  my_event_handler->insert_event(NextUpDate);
	}
    }
  
  Arena* my_arena;
  double refresh; //Time between 2 Update of robot
  double NoMoreThan;
};

class ShotsUpdateEvent : public Event
{
public:
  ShotsUpdateEvent( const double time, Arena* ar, EventHandler * ev) 
    : Event(time, ev), my_arena(ar), NoMoreThan(0) {}

  ShotsUpdateEvent(const double time, const double StopAt, Arena* ar, EventHandler* ev)
    : Event(time, ev), my_arena(ar), refresh(time), NoMoreThan(StopAt) {}

  ShotsUpdateEvent(const double time, const double StopAt, const double UpDateDelay, Arena* ar, EventHandler* ev)
    : Event(time, ev), my_arena(ar), refresh(UpDateDelay), NoMoreThan(StopAt) {}

  void eval() const 
    { 
      //      my_arena->update_shots();
      NextEvent();
    }

protected:
  void NextEvent() const
    {   
      if((NoMoreThan != 0)&&((eval_time+refresh < NoMoreThan)))
	{ //Add the next event if the game isn't finished...
	  Event * NextUpDate = new ShotsUpdateEvent(eval_time + refresh, refresh, NoMoreThan, my_arena, my_event_handler);
	  my_event_handler->insert_event(NextUpDate);
	}
    }

  Arena* my_arena;
  double refresh; //Time between 2 Update of robot
  double NoMoreThan;
};


#endif






