/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2001  Erik Ouchterlony and Ragnar Ouchterlony

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
  my_type = BOOL_V;
  minimum = 0;
  maximum = 1;
  
  constant = true;
  assign( int(rint(val)) );
  set_robot_permissions( false, false );
}


void
Variable::make_int( const double val, const double min_val, const double max_val )
{
  my_type = INT_V;
  minimum = int(min_val);
  maximum = int(max_val);
  
  constant = true;
  assign( int(rint(val)) );
  set_robot_permissions( false, false );
}


void
Variable::make_double( const double val, const double min_val, const double max_val,
                       const double inacc )
{
  my_type = DOUBLE_V;

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
  my_type = ( is_double ? DOUBLE_V : INT_V );
    
  if( is_double )
    {
      minimum = min_val;
      maximum = max_val;
      value = min_val;
    }
  else
    {
      minimum = int(rint(min_val));
      maximum = int(rint(max_val));
      value = int(rint(min_val));
    }

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
Variable::assign(const Value& val)
{
  value = val;

  if( value.is_double )
    {
      if( value.d_val >= double(maximum) ) 
        value.d_val = double(maximum);
      else if( value.d_val <= double(minimum) )
        value.d_val = double(minimum);
    }
  else
    {
      if( value.i_val >= int(maximum) ) 
        value.i_val = int(maximum);
      else if( value.i_val <= int(minimum) )
        value.i_val = int(minimum);
    }
}


Value&
Variable::get_value() const
{
  if( constant ) return value;


  if( random ) 
    {
      if( value.is_double )
        {
          value.d_val = double(minimum) + double( rand() ) * 
            ( double(maximum) - double(minimum) ) / double(RAND_MAX);
        }
      else
        {
          value.i_val = int(minimum) + rand() % ( int(maximum) - int(minimum) + 1 );
        }

      return value;
    }
  

  // Add normal-distributed error

  assert( value.is_double );

  double err=0.0;

  for(int i=0; i<12; i++) err+= ( (double) rand()/(double) RAND_MAX );

  err = (err - 6.0)*inaccuracy / sqrt(12.0);

  value.d_val = mean + err;

  return value;
}

Variable&
Variable::operator+=(int n)
{
  assert( !value.is_double ); 
  
  value.i_val += n;

  if( value.i_val >= int(maximum) ) value.i_val = int(maximum);
  else if( value.i_val <= int(minimum) ) value.i_val = int(minimum);

  return *this;
}

Variable&
Variable::operator-=(int n)
{
  assert( !value.is_double ); 
  
  value.i_val -= n;

  if( value.i_val >= int(maximum) ) value.i_val = int(maximum);
  else if( value.i_val <= int(minimum) ) value.i_val = int(minimum);

  return *this;
}
