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


//-------------------------------------------------------------------
//   SubSquareLineIterator
//-------------------------------------------------------------------


SubSquareLineIterator::
SubSquareLineIterator(const Vector2D& pos, const Vector2D& vel)
{
  begin(pos, vel);
}

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

void
SubSquareLineIterator::move_one()
{
  s+=d;
  if( s >= 1.0 )
    {
      s-= 1.0;
      xx += side_x;
      yy += side_y;
    }
  
  xx += main_x;
  yy += main_y;
}

direction_t
SubSquareLineIterator::get_start_dir_for_sector()
{
  if( main_y == 0 )
    return ( main_x == 1 ? DOWN : UP );

  return ( main_y == 1 ? RIGHT : LEFT );
}



//-------------------------------------------------------------------
//   SubSquareSectorIterator
//-------------------------------------------------------------------

SubSquareSectorIterator::
SubSquareSectorIterator(const Vector2D& pos, 
                        const double left_angle, 
                        const double right_angle )
  : left_line(pos, angle2vec(left_angle)),
    right_line(pos, angle2vec(right_angle))
  
{
  the_arena.get_subsquare_at( pos, xx, yy );
  center_x = xx;
  center_y = yy;
  is_ok = true;
  distance = 0;
}

SubSquareSectorIterator&
SubSquareSectorIterator::operator++(int)
{
  do 
    {
      if( xx == right_line.x() && yy == right_line.y() )
        {
          if( !is_ok ) return *this; // No ok subsq found

          left_line.move_one();
          right_line.move_one();
      
          xx = left_line.x();
          yy = left_line.y();

          dir = left_line.get_start_dir_for_sector();
          distance++;

          is_ok = false; // will be true when we find an ok subsq
        }
      else
        {
          switch( dir )
            {
            case RIGHT:
              xx++;
              if( xx - center_x == distance ) 
                dir = DOWN; 
              break;
            case DOWN:
              yy++;
              if( yy - center_x == distance ) 
                dir = LEFT; 
              break;
            case LEFT:
              xx--;
              if( center_x - xx == distance ) 
                dir = UP; 
              break;
            case UP:
              yy--;
              if( center_y - yy == distance ) 
                dir = RIGHT; 
              break;
            }
        }
    } while ( xx < 0 || xx >= the_arena.get_number_of_subsquares_x() || 
              yy < 0 || yy >= the_arena.get_number_of_subsquares_y() );

  is_ok = true;
  return *this;
}

//-------------------------------------------------------------------
//   SubSquareCircleIterator
//-------------------------------------------------------------------


SubSquareCircleIterator::
SubSquareCircleIterator(const Vector2D& pos)

{
  the_arena.get_subsquare_at( pos, xx, yy );
  center_x = xx;
  center_y = yy;
  is_ok = true;
  distance = 0;
  dir = UP;
}

SubSquareCircleIterator&
SubSquareCircleIterator::operator++(int)
{

  // Order:

  //    25 10 11 12 13
  //    24  9  2  3 14
  //    23  8  1  4 15
  //    22  7  6  5 16
  //    21 20 19 18 17
  


  do 
    {
      switch( dir )
        {
        case RIGHT:
          xx++;
          if( xx - center_x == distance ) 
            dir = DOWN; 
          break;
        case DOWN:
          yy++;
          if( yy - center_x == distance ) 
            dir = LEFT; 
          break;
        case LEFT:
          xx--;
          if( center_x - xx == distance ) 
            dir = UP; 
          break;
        case UP:
          yy--;
          if( center_y - yy > distance ) 
            {
              if( !is_ok ) return *this;
              dir = RIGHT; 
              distance++;
              is_ok = false;
            }
          
          break;
        }
    } while ( xx < 0 || xx >= the_arena.get_number_of_subsquares_x() || 
              yy < 0 || yy >= the_arena.get_number_of_subsquares_y() );
  
  is_ok = true;
  return *this;
}

