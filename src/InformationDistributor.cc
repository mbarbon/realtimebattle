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

#include "InfoClasses.h"
#include "InformationDistributor.h"
#include "Various.h"

InformationDistributor::InformationDistributor()
{
  informationlist.resize( distributor_listsize );
  number_of_readers = 0;
  writing_point = informationlist.begin();
}

// TODO: Handle slow guis in a better way. Maybe an importance level?
void
InformationDistributor::insert_information( const InfoBase* _info )
{
  list<reader_t>::iterator li;
  for( li = readerlist.begin(); li != readerlist.end(); li++ )
    if( (*li).reading_point == writing_point )
      {
        ((*li).reading_point)++;
        if( (*li).reading_point == informationlist.end() )
          (*li).reading_point = informationlist.begin();
        Error( false, "Warning: Unread message for gui is being overwritten",
               "InformationDistributor::insert_information" );
      }
  const InfoBase* old_info = *writing_point;
  *writing_point = _info;
  delete old_info;
  writing_point++;
  if( writing_point == informationlist.end() )
    writing_point = informationlist.begin();
}

// Returns NULL if reader_id not found or there are no messages to be read.
const InfoBase*
InformationDistributor::get_information( const int reader_id )
{
  list<reader_t>::iterator li;
  for( li = readerlist.begin(); li != readerlist.end(); li++ )
    if( (*li).reader_id == reader_id )
      {
        if( (*li).reading_point == writing_point )
          return NULL;
        const InfoBase* info = *((*li).reading_point);
        ((*li).reading_point)++;
        if( (*li).reading_point == informationlist.end() )
          (*li).reading_point = informationlist.begin();
        return info;
      }
  return NULL;
}

const int
InformationDistributor::add_reader()
{
  number_of_readers++;
  readerlist.push_back( reader_t( number_of_readers, writing_point ) );
  return number_of_readers;
}
