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

#ifndef __GUISTRUCTS__
#define __GUISTRUCTS__

#include <gdk/gdk.h>

#include "GuiVarious.h"

struct pixmap_t
{
  pixmap_t() : pixmap( (GdkPixmap*)NULL ), window( (GdkWindow*)NULL ) {}
  ~pixmap_t();

  void set_pixmap(GdkColor& col, GdkWindow* win);
  void get_pixmap(GdkColor& col, GdkWindow* win, GdkPixmap*& pixm, GdkBitmap*& bitm);

  GdkPixmap* pixmap;
  GdkWindow* window;
  GdkBitmap* bitmap;
};

struct rgb_gdk_colour_t
{
  rgb_gdk_colour_t               ( const long int&    rgb_col );
  rgb_gdk_colour_t               ( const GdkColor&    gdk_col );

  void set_colour                ( const long int&    rgb_col );
  void set_colour                ( const GdkColor&    gdk_col );

  long int rgb_colour;
  GdkColor gdk_colour;
};

#endif __GUISTRUCTS__
