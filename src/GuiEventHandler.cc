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

#include <list>

#include "GuiEvent.h"
#include "GuiEventHandler.h"

GuiEventHandler::GuiEventHandler()
{
  eventlist.resize( guieventlist_size );
  number_of_readers = 0;
  writing_point = eventlist.begin();
}

// TODO: Check if inserting deletes unread events for a gui.
void
GuiEventHandler::insert_event( const GuiEvent* _event )
{
  const GuiEvent* old_event = *writing_point;
  *writing_point = _event;
  delete old_event;
  writing_point++;
  if( writing_point == eventlist.end() )
    writing_point = eventlist.begin();
}

// Returns NULL if reader_id not found or there are no messages to be read.
// TODO: Check if we try to read a message if there are no messages waiting.
const GuiEvent*
GuiEventHandler::get_event( const int reader_id )
{
  list<reader_t>::iterator li;
  for( li = readerlist.begin(); li != readerlist.end(); li++ )
    if( (*li).reader_id == reader_id )
      {
        const GuiEvent* event = *((*li).reading_point);
        ((*li).reading_point)++;
        if( (*li).reading_point == eventlist.end() )
          (*li).reading_point = eventlist.begin();
        return event;
      }
  return NULL;
}

const int
GuiEventHandler::add_reader()
{
  number_of_readers++;
  readerlist.push_back( reader_t( number_of_readers, writing_point ) );
  return number_of_readers;
}
