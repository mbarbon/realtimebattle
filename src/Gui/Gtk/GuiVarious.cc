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
# include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <string.h>
#include <string>

#include "GuiVarious.h"
#include "Various.h"
#include "String.h"

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
    Error(true, "Couldn't allocate colour", "Various:make_gdk_color");

  return colour;
}

long 
gdk2hex_colour(const GdkColor& col)
{
  return  ( (col.blue & 0xff) | 
            ((col.green & 0xff) << 8) |
            ((col.red & 0xff) << 16) );
}

#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
gint
int_compare(GtkCList* clist, gconstpointer ptr1, gconstpointer ptr2)
{
  char* text1 = NULL;
  char* text2 = NULL;

  GtkCListRow* row1 = (GtkCListRow*) ptr1;
  GtkCListRow* row2 = (GtkCListRow*) ptr2;

  switch (row1->cell[clist->sort_column].type)
    {
    case GTK_CELL_TEXT:
      text1 = GTK_CELL_TEXT (row1->cell[clist->sort_column])->text;
      break;
    case GTK_CELL_PIXTEXT:
      text1 = GTK_CELL_PIXTEXT (row1->cell[clist->sort_column])->text;
      break;
    default:
      break;
    }
 
  switch (row2->cell[clist->sort_column].type)
    {
    case GTK_CELL_TEXT:
      text2 = GTK_CELL_TEXT (row2->cell[clist->sort_column])->text;
      break;
    case GTK_CELL_PIXTEXT:
      text2 = GTK_CELL_PIXTEXT (row2->cell[clist->sort_column])->text;
      break;
    default:
      break;
    }

  if (!text2)
    return (text1 != NULL);

  if (!text1)
    return -1;

  return (string2int(text1) - string2int(text2));
}

gint
float_compare(GtkCList *clist, gconstpointer ptr1, gconstpointer ptr2)
{
  char* text1 = NULL;
  char* text2 = NULL;

  GtkCListRow* row1 = (GtkCListRow*) ptr1;
  GtkCListRow* row2 = (GtkCListRow*) ptr2;

  switch (row1->cell[clist->sort_column].type)
    {
    case GTK_CELL_TEXT:
      text1 = GTK_CELL_TEXT (row1->cell[clist->sort_column])->text;
      break;
    case GTK_CELL_PIXTEXT:
      text1 = GTK_CELL_PIXTEXT (row1->cell[clist->sort_column])->text;
      break;
    default:
      break;
    }
 
  switch (row2->cell[clist->sort_column].type)
    {
    case GTK_CELL_TEXT:
      text2 = GTK_CELL_TEXT (row2->cell[clist->sort_column])->text;
      break;
    case GTK_CELL_PIXTEXT:
      text2 = GTK_CELL_PIXTEXT (row2->cell[clist->sort_column])->text;
      break;
    default:
      break;
    }

  if (!text2)
    return (text1 != NULL);

  if (!text1)
    return -1;

  double n1 = string2double(text1);
  double n2 = string2double(text2);

  if(n1 > n2)
    return 1;
  else if(n2 > n1)
    return -1;

  return 0;
}

gint
string_case_sensitive_compare(GtkCList *clist, gconstpointer ptr1, gconstpointer ptr2)
{
  char *text1 = NULL;
  char *text2 = NULL;

  GtkCListRow *row1 = (GtkCListRow *) ptr1;
  GtkCListRow *row2 = (GtkCListRow *) ptr2;

  switch (row1->cell[clist->sort_column].type)
    {
    case GTK_CELL_TEXT:
      text1 = GTK_CELL_TEXT (row1->cell[clist->sort_column])->text;
      break;
    case GTK_CELL_PIXTEXT:
      text1 = GTK_CELL_PIXTEXT (row1->cell[clist->sort_column])->text;
      break;
    default:
      break;
    }
 
  switch (row2->cell[clist->sort_column].type)
    {
    case GTK_CELL_TEXT:
      text2 = GTK_CELL_TEXT (row2->cell[clist->sort_column])->text;
      break;
    case GTK_CELL_PIXTEXT:
      text2 = GTK_CELL_PIXTEXT (row2->cell[clist->sort_column])->text;
      break;
    default:
      break;
    }

  if (!text2)
    return (text1 != NULL);

  if (!text1)
    return -1;

  return strcmp (text1, text2);
}

gint
string_case_insensitive_compare(GtkCList *clist, gconstpointer ptr1, gconstpointer ptr2)
{
  char *text1 = NULL;
  char *text2 = NULL;

  GtkCListRow *row1 = (GtkCListRow *) ptr1;
  GtkCListRow *row2 = (GtkCListRow *) ptr2;

  switch (row1->cell[clist->sort_column].type)
    {
    case GTK_CELL_TEXT:
      text1 = GTK_CELL_TEXT (row1->cell[clist->sort_column])->text;
      break;
    case GTK_CELL_PIXTEXT:
      text1 = GTK_CELL_PIXTEXT (row1->cell[clist->sort_column])->text;
      break;
    default:
      break;
    }
 
  switch (row2->cell[clist->sort_column].type)
    {
    case GTK_CELL_TEXT:
      text2 = GTK_CELL_TEXT (row2->cell[clist->sort_column])->text;
      break;
    case GTK_CELL_PIXTEXT:
      text2 = GTK_CELL_PIXTEXT (row2->cell[clist->sort_column])->text;
      break;
    default:
      break;
    }

  if (!text2)
    return (text1 != NULL);

  if (!text1)
    return -1;

  return strcasecmp (text1, text2);
}
#endif
