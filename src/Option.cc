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

void
StringOption::reset_value()
{
  value = default_value;
}

void
StringOption::change_value( const string& newval, const bool def )
{
  value = newval;
  if( def )
    default_value = newval;
}
