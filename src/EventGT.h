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

class Arena;
class Script;

//All the 'recursive' update event should have the same 'face'
class RobotsUpdateEvent : public Event
{
 public:
  RobotsUpdateEvent(const double time, const double refresh_time)
    : Event(time), refresh(refresh_time) {}
  
  void eval() const ;

 protected:

  double refresh; //Time between 2 update of robots
};


class ShotsUpdateEvent : public Event
{
public:
  ShotsUpdateEvent( const double time, const double refresh_time) 
    : Event(time), refresh(refresh_time) {}

  void eval() const ;

protected:

  double refresh;           //Time between 2 updates of shots
};



class ContinueScriptEvent : public Event
{
public:
  ContinueScriptEvent( const double time, Script* scr )
    : Event(time), my_script(scr) {}

  void eval() const ;

protected:
  Script* my_script;
};


#endif






