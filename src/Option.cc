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

#include <string>
#include <fstream>

#include "Option.h"
#include "String.h"

const string
LongOption::make_string_val( const long int temp_value ) const
{
  if( !hexadecimal )
    return lint2string( temp_value );
  else
    return hex2string( temp_value );
  return string("");
}

void
LongOption::reset_value()
{
  value = default_value;
}

const bool
LongOption::is_value_accepted( const long int newval ) const
{
  return( newval >= min_value && newval <= max_value );
}

const bool
LongOption::change_value( const long int newval, const bool def )
{
  if( newval >= min_value && newval <= max_value )
    {
      value = newval;
      if( def )
        default_value = newval;
      return true;
    }
  return false;
}

const bool
LongOption::change_value( const string& newval_string, const bool def )
{
  if( !newval_string.empty() )
    {
      if( !hexadecimal )
        return change_value( string2lint( newval_string ), def );
      else
        return change_value( string2hex( newval_string ), def );
    }
  return false;
}

string&
LongOption::make_correct_string_val( string& str ) const
{
  if( str.empty() )
    return str;

  string (* val2string) ( const long int ) = lint2string;
  long int (* string2val) ( const string& ) = string2lint;
  string allowed_vals = "0123456789";
  if( hexadecimal )
    {
      allowed_vals += "abcdefABCDEF";
      val2string = hex2string;
      string2val = string2hex;
    }

  string::size_type first_pos = 0;
  bool negative = false;
  if( min_value < 0 && (negative = (str[0] == '-')) )
    first_pos++;

  string::size_type pos = str.find_first_not_of(allowed_vals, first_pos);

  while( pos != string::npos )
    {
      str.erase( pos, 1 );
      pos = str.find_first_not_of(allowed_vals, pos);
    }

  long int nval = string2val( str );

  if( nval > max_value )
    return( str = val2string( max_value ) );
  if( nval < min_value )
    return( str = val2string( min_value ) );
  if( nval == 0 )
    if( negative )
      return( str = "-0" );
    else
      return( str = "0" );

  if( first_pos != string::npos && str[first_pos] == '0' )
    str.erase( first_pos, 1 );

  return str;
}

const string
DoubleOption::get_string_val() const
{
  return double2string( value );
}

const string
DoubleOption::get_string_min() const
{
  return double2string( min_value );
}

const string
DoubleOption::get_string_max() const
{
  return double2string( max_value );
}

const string
DoubleOption::get_string_def() const
{
  return double2string( default_value );
}

void
DoubleOption::reset_value()
{
  value = default_value;
}

const bool
DoubleOption::is_value_accepted( const double newval ) const
{
  return( newval >= min_value && newval <= max_value );
}

const bool
DoubleOption::change_value( const double newval, const bool def )
{
  if( newval >= min_value && newval <= max_value )
    {
      value = newval;
      if( def )
        default_value = newval;
      return true;
    }
  return false;
}

const bool
DoubleOption::change_value( const string& newval_string, const bool def )
{
  if( !newval_string.empty() )
    return change_value( string2double( newval_string ), def );
  return false;
}

string&
DoubleOption::make_correct_string_val( string& str ) const
{
  if( str.empty() )
    return str;

  string allowed_vals = "0123456789";

  string::size_type first_pos = 0;
  bool negative;
  if( min_value < 0 && (negative = (str[0] == '-')) )
    first_pos++;

  string::size_type pos = str.find_first_not_of(allowed_vals, first_pos);
  bool found_point = false;
  while( pos != string::npos )
    {
      if( str[pos] == '.' && found_point == false )
        {
          pos++;
          found_point = true;
        }
      else
        str.erase( pos, 1 );
      pos = str.find_first_not_of(allowed_vals, pos);
    }

  double nval = string2double( str );

  if( nval > max_value )
    return( str = double2string( max_value ) );
  if( nval < min_value )
    return( str = double2string( min_value ) );

  if( first_pos != string::npos && str[first_pos] == '0' )
    str.erase( first_pos, 1 );

  return str;
}

void
StringOption::reset_value()
{
  value = default_value;
}

const bool
StringOption::change_value( const string& newval, const bool def )
{
  value = newval;
  if( def )
    default_value = newval;
  return true;
}

const bool
BooleanOption::change_value( const bool  v,
                             const bool  def )
{
  value = v;
  if( def )
    default_value = v;

  return true;
}

const bool
BooleanOption::change_value( const string&  v,
                             const bool     def )
{
  bool b_val;
  if( lowercase(v) == "true" )
    b_val = true;
  else if( lowercase(v) == "false" )
    b_val = false;
  else
    return false;

  value = b_val;
  if( def )
    default_value = b_val;

  return true;
}

// TODO: Maybe a better variant.
string&
BooleanOption::make_correct_string_val( string& v ) const
{
  string lower_v( lowercase(v) );
  if( v.empty() || v == "" )
    {
      v = "false";
    }
  else if( lower_v != "true" && lower_v != "false" )
    {
      if( lower_v[0] == 't' )
        v = v[0] + "rue";
      else if( lower_v[0] == 'f' )
        v = v[0] + "alse";
      else
        v = "false";
    }
  return v;
}

string
BooleanOption::make_string_of_bool( const bool v ) const
{
  if( v == true )
    return "true";
  return "false";
}
