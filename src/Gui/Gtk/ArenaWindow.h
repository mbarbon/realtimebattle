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

#ifndef RTB_GTKGUI__ARENA_WINDOW__
#define RTB_GTKGUI__ARENA_WINDOW__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <gtk/gtk.h>

#include "Vector2D.h"
#include "Window.h"

class ArenaDisplay;

class ArenaWindow : public Window
{
public:
  ArenaWindow                         ();
  ~ArenaWindow                        ();

  void create                         ( const int&           default_width  = -1,
                                        const int&           default_height = -1,
                                        const int&           default_x_pos  = -1,
                                        const int&           default_y_pos  = -1 );
  void destroy                        ();

  void set_window_title               () {};

private:
  // Graphics
  static void no_zoom                 ( GtkWidget*           widget,
                                        ArenaWindow*         object_p );
  static void zoom_in                 ( GtkWidget*           widget,
                                        ArenaWindow*         object_p );
  static void zoom_out                ( GtkWidget*           widget,
                                        ArenaWindow*         object_p );
  static gint close_window            ( GtkWidget*           widget,
                                        GdkEvent*            event,
                                        ArenaWindow*         object_p );
  static gint keyboard_handler        ( GtkWidget*           widget,
                                        GdkEventKey*         event,
                                        ArenaWindow*         object_p );

  ArenaDisplay* the_display;

  static const int arenawindow_min_width  ;
  static const int arenawindow_min_height ;
};

#endif // RTB_GTKGUI__ARENA_WINDOW__
