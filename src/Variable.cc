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

#include "Variable.h"



void
Variable::make_bool( const double val )
{
  my_type = BOOL;
  minimum = 0.0;
  maximum = 1.0;
  
  constant = true;
  assign( val );
  set_robot_permissions( false, false );
}


void
Variable::make_int ( const double val, const double min_val, const double max_val )
{
  my_type = INT;
  minimum = min_val;
  maximum = max_val;
  
  constant = true;
  assign( val );
  set_robot_permissions( false, false );
}


void
Variable::make_double( const double val, const double min_val, const double max_val,
                       const double inacc )
{
  my_type = DOUBLE;
  minimum = min_val;
  maximum = max_val;


  if( inacc == 0.0 ) 
    constant = true;
  else
    {
      inaccuracy = inacc;
      constant = false;
      random = false;
    }

  assign( val );
  set_robot_permissions( false, false );
}

void
Variable::make_random( const double min_val, const double max_val, 
                       const bool is_double )
{
  my_type = ( is_double ? DOUBLE : INT );
    
  minimum = min_val;
  maximum = max_val;

  constant = false;
  random = true;
  set_robot_permissions( false, false );
}

void
Variable::set_robot_permissions(const bool read, const bool write )
{
  public_readable = read;
  public_writable = write;
}

void
Variable::assign(const double val)
{
  if( val >= maximum ) value = maximum;

  if( val >= minimum ) value = minimum;

  if( my_type == DOUBLE ) value = val;

  value = rint( val ); 
}


double
Variable::get_value()
{
  if( constant ) return value;


  if( random ) 
    {
      double c = minimum + double( rand() ) * ( maximum - minimum ) / double(RAND_MAX);
      if( my_type == INT ) 
        return rint( c );

      return c;
    }
  

  // Add normal-distributed error

  double err=0.0;

  for(int i=0; i<12; i++) err+= ( (double) rand()/(double) RAND_MAX );

  err = (err - 6.0)*inaccuracy / sqrt(12.0);

  return( err + value );
}
