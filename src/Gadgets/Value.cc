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

#include "Value.h"

//
// Conversion operators
//
  

Value::operator int() const
{
  return( is_double ? int(rint(d_val)) : i_val );
}


Value::operator double() const
{ 
  return( is_double ? d_val : double(i_val) );
}


//
// Operators
//

Value& 
Value::operator+=(const Value& v) 
{
  if( is_double )
    {
      d_val += double( v );
    }
  else if( v.is_double )
    {
      d_val = double( i_val ) + v.d_val;
      is_double = true;
    }
  else
    i_val += v.i_val;
    
  return *this; 
}

Value& 
Value::operator-=(const Value& v) 
{ 
  if( is_double )
    {
      d_val -= double( v );
    }
  else if( v.is_double )
    {
      d_val = double( i_val ) - v.d_val;
      is_double = true;
    }
  else
    i_val -= v.i_val;
  
  return *this; 
}

Value& 
Value::operator*=(const Value& v) 
{ 
  if( is_double )
    {
      d_val *= double( v );
    }
  else if( v.is_double )
    {
      d_val = double( i_val ) * v.d_val;
      is_double = true;
    }
  else
    i_val *= v.i_val;

  return *this; 
}

Value& 
Value::operator/=(const Value& v) 
{ 
  if( is_double )
    {
      d_val /= double( v );
    }
  else if( v.is_double )
    {
      d_val = double( i_val ) / v.d_val;
      is_double = true;
    }
  else
    i_val /= v.i_val;

  return *this; 
}

Value&
Value::invert()
{ 
  d_val = 1/(operator double());
  return *this;
}

Value&
Value::neg()
{
  if( is_double )
    d_val = -d_val;
  else
    i_val = -i_val;

  return *this;
}


//
// Comparison operators
//

Value&
Value::operator< (const Value& v)
{
  if( is_double || v.is_double )
    i_val = ( operator double() < double(v) );
  else
    i_val = ( i_val < v.i_val );

  is_double = false;
  return *this;
}

Value&
Value::operator<= (const Value& v)
{
  if( is_double || v.is_double )
    i_val = ( operator double() <= double(v) );
  else
    i_val = ( i_val <= v.i_val );

  is_double = false;
  return *this;
}

Value&
Value::operator> (const Value& v)
{
  if( is_double || v.is_double )
    i_val = ( operator double() > double(v) );
  else
    i_val = ( i_val > v.i_val );

  is_double = false;
  return *this;
}

Value&
Value::operator>= (const Value& v)
{
  if( is_double || v.is_double )
    i_val = ( operator double() >= double(v) );
  else
    i_val = ( i_val >= v.i_val );

  is_double = false;
  return *this;
}

Value&
Value::operator== (const Value& v)
{
  if( is_double || v.is_double )
    i_val = ( operator double() == double(v) );
  else
    i_val = ( i_val == v.i_val );

  is_double = false;
  return *this;
}

Value&
Value::operator!= (const Value& v)
{
  if( is_double || v.is_double )
    i_val = ( operator double() != double(v) );
  else
    i_val = ( i_val != v.i_val );

  is_double = false;
  return *this;
}




//
//  Trigonometry
//

Value&
Value::cos()
{ 
  d_val = ::cos(operator double());
  is_double = true;
  return *this;
}

Value&
Value::sin()
{ 
  d_val = ::sin(operator double());
  is_double = true;
  return *this;
}

Value&
Value::tan()
{ 
  d_val = ::tan(operator double());
  is_double = true;
  return *this;
}

Value&
Value::acos()
{ 
  d_val = ::acos(operator double());
  is_double = true;
  return *this;
}

Value&
Value::asin()
{ 
  d_val = ::asin(operator double());
  is_double = true;
  return *this;
}

Value&
Value::atan()
{ 
  d_val = ::atan(operator double());
  is_double = true;
  return *this;
}

