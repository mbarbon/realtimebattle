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

#ifndef __SUBSQUARE__
#define __SUBSQUARE__

#include <set>
#include "Vector2D.h"
#include "Messagetypes.h"

// This playing arena is divided into subrectangles in order to make finding close
// objects easier. The ArenaObjectCollector class is responsible for placing the
// objects in the arena in the right subsquares.

class Shape;

class SubSquare
{
public:
  SubSquare() {}
  ~SubSquare() {}

  void clean();  // removes shapes which are no longer inside the subsquare

  void add(Shape* s);

  double get_shortest_distance(const Vector2D& pos, const Vector2D& vel, 
                               const double size, enum object_type& closest_shape, 
                               Shape*& colliding_object, 
                               const class Shape* from_shape = NULL );

  friend class Arena;  // the arena needs to access the shapes smoothly

protected:
  
  int x;  // x,y = 0,0 for the top left subsquare
  int y;


  // This set contains _at least_ the shapes which are inside the subsquare.
  // It should be cleaned up once in a while to get rid of garbage.
  set<Shape*> shapes;  
  

  Vector2D top_left;
  Vector2D bottom_right;

};




#endif __SUBSQUARE__
