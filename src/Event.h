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

#ifndef __EVENT__
#define __EVENT__

class EventHandler;

class Event
{

public:
  Event( const double time, EventHandler * ev ) : eval_time(time), my_event_handler(ev) {}

  virtual ~Event() {}

  virtual void eval() const {}

  double get_time() const { return eval_time; } 

  friend bool operator<(const Event& a, const Event& b) { return a.eval_time < b.eval_time; } 

protected:

  double eval_time;
  EventHandler * my_event_handler;
};

#endif __EVENT__
