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

#ifndef __EVENT_HANDLER__
#define __EVENT_HANDLER__

#include <queue>

#include "Event.h"
#include "Timer.h"

class QuitEvent;


class EventHandler
{

public:

  EventHandler();
  ~EventHandler();

  void main_loop();

  void insert_event( Event* ev );

  double get_time() { return current_time; }

  void quit();
private:
  
  friend class QuitEvent;

  void finish() { finished = true; }

  priority_queue<class Event> event_queue;
  
  Timer timer;
  double current_time;

  bool finished;
};

class QuitEvent : public Event
{
public:
  QuitEvent( const double time, EventHandler* ev) 
    : Event(time), my_event_handler(ev) {}

  void eval() const { my_event_handler->finish(); }

protected:
  EventHandler* my_event_handler;
};


#endif __EVENT_HANDLER__
