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

#ifndef __GUIEVENT__
#define __GUIEVENT__

#include <string>

#include "Structs.h"

class GuiEvent
{
public:
  GuiEvent::GuiEvent                () {}
  GuiEvent::~GuiEvent               () {}

private:
};

class GuiMessageEvent : public GuiEvent
{
public:
  GuiMessageEvent::GuiMessageEvent  ( const string& s, const string& m )
    : sender(s), message(m) {}
  GuiMessageEvent::~GuiMessageEvent () {}
  const string& get_sender          () const { return sender; }
  const string& get_message         () const { return message; }

private:
  string sender;
  string message;
};

class GuiStateEvent : public GuiEvent
{
public:
  GuiStateEvent::GuiStateEvent  ( const state_t s )
    : state(s) {}
  GuiStateEvent::~GuiStateEvent () {}

  const state_t get_state       () const { return state; }

private:
  state_t state;
};

#endif __GUIEVENT__
