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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <set>
#include <math.h>

#include "SubSquare.h"
#include "Shape.h"



void
SubSquare::add(Shape* s)
{
  shapes.insert(s);
}


void
SubSquare::clean()
{
  set<Shape*>::iterator li;
  for( li = shapes.begin(); li != shapes.end(); li++ )
    {
      if( ! (*li)->is_still_inside_subsquare(*this) )
        shapes.erase(li);
    }
}


double
SubSquare::get_shortest_distance(const Vector2D& pos, const Vector2D& dir, 
                                  const double size, object_type& closest_shape, 
                                  Shape*& colliding_object, const Shape* from_shape)
{
  double dist = DBL_MAX;
  double d;
  closest_shape = NOOBJECT;

  set<Shape*>::iterator li;

  for( li = shapes.begin(); li != shapes.end() ; li++)
    {
      if( (*li) != from_shape )
        {
          d = (*li)->get_distance(pos, dir, size);
          if( d < dist)
            {

              // How should the object type be found ?
              //              closest_shape = (object_type)obj_type;
              colliding_object = (*li);
              dist = d;
            }
        }
    }


  return dist;
}
