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

#include <math.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdlib.h>
#include <iostream.h>

#include "Various.h"
#include "String.h"
#include "Error.h"

int
factorial(const int n)
{
  double fact = 1.0;
  for(int i=1; i<=n; i++)  
    fact *= i;

  if( fact < (double)INT_MAX )
    return (int)fact;
  else
    return INT_MAX;
}

int
binomial(const int n, const int k)
{
  int k2 = min(k, n-k);
  
  double bin = 1;
  for(int i=n; i>n-k2; i--)  
    bin *= i;

  for(int i=1; i<=k2; i++)  
    bin /= i;

  if( bin < (double)INT_MAX )
    return (int)bin;
  else
    return INT_MAX;
}

GdkColor
make_gdk_colour(const long col)
{
  GdkColormap *cmap;
  GdkColor colour;

  cmap = gdk_colormap_get_system();
  colour.red =   ((col & 0xff0000) >> 16 ) * 0x101;
  colour.green = ((col & 0x00ff00) >> 8  ) * 0x101;
  colour.blue =  (col & 0x0000ff) * 0x101;
  if( !gdk_color_alloc (cmap, &colour) )
    throw Error("Couldn't allocate colour", "Arena::make_gdk_color");

  return colour;
}

long 
gdk2hex_colour(const GdkColor& col)
{
  return  ( (col.blue & 0xff) | 
            ((col.green & 0xff) << 8) |
            ((col.red & 0xff) << 16) );
}
void 
reorder_pointer_array(void** array, int size)
{
  int n1, n2;
  void* temp_p;
  for(int i=0; i<size*5; i++)
    {
      n1 = (int)floor(size*((double)rand() / (double)RAND_MAX)) ;
      n2 = (int)floor(size*((double)rand() / (double)RAND_MAX)) ;
      if( n1 != n2 )
        {
          temp_p = array[n1];
          array[n1] = array[n2];
          array[n2] = temp_p;
        }
    }
}

void
entry_handler( GtkWidget * entry, entry_t * entry_info )
{
  String entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
  String old_entry_text = entry_text;
  bool point_found = false;

  for(int i=0;i<entry_text.get_length();i++)
    {
      switch( entry_info->datatype )
        {
        case ENTRY_INT:
          if( !((entry_text[i] >= '0' && entry_text[i] <= '9') ||
                (entry_text[i] == '-' && i == 0 && entry_info->allow_sign ))  )
            entry_text.erase(i);
          break;
        case ENTRY_DOUBLE:
          if( !((entry_text[i] >= '0' && entry_text[i] <= '9') ||
                (entry_text[i] == '.') ||
                (entry_text[i] == '-' && i == 0 && entry_info->allow_sign ))  )
            entry_text.erase(i);
          if( entry_text[i] == '.' && !point_found )
            point_found = true;
          else if( entry_text[i] == '.' && point_found )
            entry_text.erase(i);
          break;
        case ENTRY_HEX:
          if( !(((entry_text[i] >= '0' && entry_text[i] <= '9') || 
                 (entry_text[i] >= 'a' && entry_text[i] <= 'f') ||
                 (entry_text[i] >= 'A' && entry_text[i] <= 'F')) ||
                (entry_text[i] == '-' && i == 0 ) && entry_info->allow_sign ) )
            entry_text.erase(i);
          break;
        case ENTRY_CHAR:
          break;
        case ENTRY_BOOL:
          break;
        }
    }

  if(old_entry_text != entry_text)
    gtk_entry_set_text(GTK_ENTRY(entry),entry_text.chars());
}
