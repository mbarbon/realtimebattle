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

#include <math.h>

#include "Explosion.h"
#include "Variable.h"

Explosion::Explosion(const Vector2D& c, const ExplosionGadget& gad, const double st_time) :
  Circle(c, 0.0), my_gadget(gad), start_time(st_time)
{
  max_size = my_gadget.get_variables()[ExplosionGadget::SIZE];
  reach_maximum_time = start_time + my_gadget.get_variables()[ExplosionGadget::GROWTIME];
  start_shrink_time = reach_maximum_time + 
    my_gadget.get_variables()[ExplosionGadget::ATMAXTIME];
  end_time = start_shrink_time + my_gadget.get_variables()[ExplosionGadget::SHRINKTIME];
}


void
Explosion::update_size( const double time )
{
  if( time > end_time )
    {
      killed = true;
    }
  else if( time > start_shrink_time )
    {
      radius = max_size * ( 1.0 - ( time - start_shrink_time ) / 
                            ( end_time - start_shrink_time ) );
    }
  else if( time > reach_maximum_time )
    {
      radius = max_size;
    }
  else 
    {
      radius = max_size * ( time - start_time ) / 
        ( reach_maximum_time - start_time );
    }
}

double
Explosion::get_percentage_inside( const Vector2D& c, const double r )
{
  double d = length( c - center );

  if( d >= r + radius ) return 0.0;

  if( radius >= d + r ) return 1.0;

  double l = ( radius*radius - r*r ) / d;

  double l1 = 0.5* ( d + l );
  double l2 = 0.5* ( d - l );

  return ( radius * radius / ( r*r*M_PI ) * acos( l1/radius ) +
           acos( l2 / r ) - sqrt( radius * radius - l1*l1 ) * d );
}
