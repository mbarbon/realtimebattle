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

#ifndef __GUIEVENTHANDLER__
#define __GUIEVENTHANDLER__

#include <list>

#include "GuiEvent.h"

static const int guieventlist_size = 512;

class GuiEventHandler
{
public:
  struct reader_t {
    reader_t::reader_t( const int id, list<const GuiEvent*>::iterator& r )
      : reader_id(id), reading_point(r) {}
      
    int reader_id;
    list<const GuiEvent*>::iterator reading_point;
  };

  GuiEventHandler::GuiEventHandler               ();
  GuiEventHandler::~GuiEventHandler              () {}

  void insert_event                              ( const GuiEvent* );
  const GuiEvent* get_event                      ( const int );
  const int add_reader                           ();
private:

  list<const GuiEvent*> eventlist;
  list<const GuiEvent*>::iterator writing_point;
  list<reader_t> readerlist;

  int number_of_readers;
};

#endif __GUIEVENTHANDLER__
