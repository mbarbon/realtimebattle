/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-1999  Erik Ouchterlony and Ragnar Ouchterlony

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

#ifndef __VARIOUS__
#define __VARIOUS__

#include <gdk/gdktypes.h>

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
//#define abs(a) ((a) > 0 ? (a) : -(a))
//#define sgn(a) ((a) > 0 ? 1 : -1)

enum entry_datatype_t
{
  ENTRY_INT,
  ENTRY_DOUBLE,
  ENTRY_HEX,
  ENTRY_CHAR,
  ENTRY_BOOL
};

struct entry_t
{
  entry_t(entry_datatype_t d, bool s) :
    datatype(d), allow_sign(s) {}
  entry_datatype_t datatype;
  bool allow_sign;
};

int factorial(const int n);

int binomial(const int n, const int k);

GdkColor make_gdk_colour(const long col);

long gdk2hex_colour(const GdkColor& col);

void reorder_pointer_array(void** array, int size);

void entry_handler( GtkWidget * entry, entry_t * entry_info );

#endif __VARIOUS__
