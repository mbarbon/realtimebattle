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

#ifndef __ARENA_OBJECT_COLLECTOR__
#define __ARENA_OBJECT_COLLECTOR__


// This playing arena is divided into subrectangles in order to make finding close
// objects easier. The ArenaObjectCollector class is responsible for placing the
// objects in the arena in the right subsquares.

class Shape;

class ArenaObjectCollector
{
public:

  ArenaObjectCollector() {}
  ~ArenaObjectCollector() {}
  
  void add( Shape& shape );
  void remove( Shape& shape );


private:

  

  set<Shape>** objects_in_subsquare;
  
  
  

};


#endif __ARENA_OBJECT_COLLECTOR__
