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

#ifndef __EventRT__
#define __EventRT__

#include "Event.h"
#include "EventHandler.h"
#include "GuiInterface.h"

class CheckGUIEvent : public Event
{
  CheckGUIEvent(const double time, EventHandler* ev, GuiServerInterface* gp )
    : Event(time, ev), NoMoreThan(0), gui_p(gp) {}
  CheckGUIEvent(const double time, const double StopAt,
                EventHandler* ev, GuiServerInterface* gp )
    : Event(time, ev), refresh(time), NoMoreThan(StopAt), gui_p(gp) {}
  CheckGUIEvent(const double time, const double StopAt,
                const double UpDateDelay, EventHandler* ev, GuiServerInterface* gp )
    : Event(time, ev), refresh(UpDateDelay), NoMoreThan(StopAt), gui_p(gp) {}

  void eval() const;

protected:
  void NextEvent() const;

  double refresh;           //Time between 2 Update of robot
  double NoMoreThan;

  GuiServerInterface* gui_p;

  // Note: Wouldn't it be nice to have a a base class for 'recursive' events,
  //       containing next_event() and such things?
};




/*class PrepareForNewMatchEvent : public Event
{
 public:
  PrepareForNewMatchEvent( const double time, Tournament* t) 
    : Event(time), my_tournament(t) {}
};

class StartNewMatchEvent : public Event
{
public:
  StartNewMatchEvent( const double time, Match* m) 
    : Event(time), my_match(m) {}
};

class EndMatchEvent : public Event
{
public:
  EndMatchEvent( const double time, Match* m) 
    : Event(time), my_match(m) {}
};*/



#endif
