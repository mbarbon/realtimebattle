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

#include <assert.h>
#include <math.h>

#include "SubSquareIterators.h"
#include "Arena.h"
#include "Vector2D.h"

void
SubSquareLineIterator::begin(const Vector2D& pos, const Vector2D& vel)
{
  the_arena.get_subsquare_at( pos, xx, yy );

  double subsq_size_x = (the_arena.get_subsquare_size())[0];
  double subsq_size_y = (the_arena.get_subsquare_size())[1];

  // this is the fraction moved into the current subsquare
  double p_x = pos[0] / subsq_size_x - double(xx);
  double p_y = pos[1] / subsq_size_y - double(yy);

  double l;

 
  if( fabs(vel[0]) > fabs(vel[1]) )
    {
      main_y = side_x = 0;

      main_x = ( vel[0] >= 0.0 ? 1 : -1 );
      side_y = ( vel[1] >= 0.0 ? 1 : -1 );

      d = fabs( vel[1] / subsq_size_y / ( vel[0] / subsq_size_x ) );

      s = ( vel[1] >= 0.0 ? p_y : 1-p_y );
      l = ( vel[0] >= 0.0 ? p_x : 1-p_x );
    }
  else
    {
      main_x = side_y = 0;
     
      main_y = ( vel[1] >= 0.0 ? 1 : -1 );
      side_x = ( vel[0] >= 0.0 ? 1 : -1 );

      d = fabs( vel[0] / subsq_size_x / ( vel[1] / subsq_size_y ) );

      s = ( vel[0] >= 0.0 ? p_x : 1-p_x );
      l = ( vel[1] >= 0.0 ? p_y : 1-p_y );
    }
 
  assert( d >= 0.0 && d <= 1.0 );
  assert( l >= 0.0 && l <= 1.0 );
  assert( s >= 0.0 && s <= 1.0 );

  s -= l;  // Back to the edge, so that next time we can move a whole step forward

  is_ok = true; 
  next_move_in_main_direction_without_check = false;
}

SubSquareLineIterator&
SubSquareLineIterator::operator++ (int)
{
  assert( is_ok );

  if( next_move_in_main_direction_without_check )
    {
      xx += main_x;
      yy += main_y;
      next_move_in_main_direction_without_check = false;
    }
  else
    {
      s += d;

      if( s >= 1.0 )
        {
          s -= 1.0;
          xx += side_x;
          yy += side_y;
          
          next_move_in_main_direction_without_check = true;
        }
      else
        {
          xx += main_x;
          yy += main_y;
        }
    }
  
  if( xx < 0 || xx >= the_arena.get_number_of_subsquares_x() || 
      yy < 0 || yy >= the_arena.get_number_of_subsquares_y() ) 
    {
      is_ok = false;
    }

  return *this;
}
