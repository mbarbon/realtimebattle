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

#ifndef __INFORMATIONDISTRIBUTOR__
#define __INFORMATIONDISTRIBUTOR__

#include <list>
#include <assert.h>

#include "InfoClasses.h"

static const int distributor_listsize = 512;

// ---------------------------------------------------------------------------
// class InformationDistributor
// ---------------------------------------------------------------------------
// Things that the server thinks that the guis should know of ends up here
// as InfoBase
// ---------------------------------------------------------------------------

class InformationDistributor
{
public:
  struct reader_t {
    reader_t( const int id, list<const InfoBase*>::iterator& r )
      : reader_id(id), reading_point(r) {}

    const bool operator==( const int& id ) { return( reader_id == id ); }
    const bool operator!=( const int& id ) { return( reader_id != id ); }
    int reader_id;
    list<const InfoBase*>::iterator reading_point;
  };

  InformationDistributor                           ();
  ~InformationDistributor                          () {}

  void insert_information                          ( const InfoBase* );
  const InfoBase* get_information                  ( const int );
  const int add_reader                             ();
private:

  list<const InfoBase*> informationlist;
  list<const InfoBase*>::iterator writing_point;
  list<reader_t> readerlist;

  int next_reader_id;
};

#endif __INFORMATIONDISTRIBUTOR__
