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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream.h>

#include "Various.h"
#include "String.h"
#include "Options.h"

extern class Options the_opts;

void
Error(const bool fatal, const String& error_msg, const String& function_name)
{
  cerr << "RealTimeBattle: Error in " << function_name << ": " << error_msg << endl;
  perror("RealTimeBattle: errno message");

  if( fatal == true )
    {
      gtk_main_quit();
      exit(EXIT_FAILURE);
    }
}

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


void
read_dirs_from_system(List<String>& robotdirs, List<String>& arenadirs)
{
  String dirs;

  robotdirs.delete_list();
  arenadirs.delete_list();

  dirs = the_opts.get_s(OPTION_ROBOT_SEARCH_PATH);
  split_colonseparated_dirs(dirs, robotdirs);

#ifdef ROBOTDIR
  String * str = new String(ROBOTDIR "/");
  robotdirs.insert_last( str );
#endif

  dirs = the_opts.get_s(OPTION_ARENA_SEARCH_PATH);
  split_colonseparated_dirs(dirs, arenadirs);

#ifdef ARENADIR
  str = new String(ARENADIR "/");
  arenadirs.insert_last( str );
#endif
}

// This function splits a string of colonseparated directories into a glist
void
split_colonseparated_dirs(String& dirs, List<String>& str_list)
{
  String current_dir = dirs;
  int pos, lastpos = 0;
  while( (pos = dirs.find(':', lastpos)) != -1 )
    {
      current_dir = get_segment(dirs, lastpos, pos-1);
      if(current_dir[current_dir.get_length() - 1] != '/')
        current_dir += '/';

      String* str = new String(current_dir);
      str_list.insert_last( str );

      lastpos = pos+1;
    }

  if(current_dir != "")
    {
      current_dir = get_segment(dirs, lastpos, -1); 
      if(current_dir[current_dir.get_length() - 1] != '/')
        current_dir += '/';

      String* str = new String(current_dir);
      str_list.insert_last( str );
    }
}

bool
check_if_filename_is_robot( String& fname )
{
  struct stat filestat;
  if( 0 == stat( fname.chars(), &filestat ) )
    if( S_ISREG( filestat.st_mode) && (filestat.st_mode & S_IXOTH) ) // Check if file is a regular file that can be executed
      return true;

  return false;
}

bool
check_if_filename_is_arena( String& fname )
{
  struct stat filestat;
  if( 0 == stat( fname.chars(), &filestat ) && fname.get_length() > 6 )
    // Check if file is a regular file that is readable and ends with .arena
    if( S_ISREG( filestat.st_mode) &&
        (filestat.st_mode & S_IROTH)  &&
        String(".arena") == get_segment(fname, -6, -1) )
      return true;

  return false;
}

#ifndef NO_GRAPHICS

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

  return (str2int(text1) - str2int(text2));
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

  double n1 = str2dbl(text1);
  double n2 = str2dbl(text2);

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

#endif NO_GRAPHICS
