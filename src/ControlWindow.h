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

#ifndef __CONTROL_WINDOW__
#define __CONTROL_WINDOW__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef NO_GRAPHICS

struct _GtkWidget;
typedef struct _GtkWidget GtkWidget;
typedef void* gpointer;

class ControlWindow
{
public:
  ControlWindow                    ( const int default_width  = -1,
                                     const int default_height = -1,
                                     const int default_x_pos  = -1,
                                     const int default_y_pos  = -1 );
  ~ControlWindow                   ();

  void set_window_title            ( const bool halted );
  GtkWidget* get_window_p          () { return window_p; }
  GtkWidget* get_debug_level_label () { return debug_level_label; }

  static void quit_rtb             ( GtkWidget* widget, gpointer data );
  static void pause                ( GtkWidget* widget,
                                     class ControlWindow* controlwindow_p );
  static void step                 ( GtkWidget* widget, gpointer data );
  static void end_game             ( GtkWidget* widget, gpointer data );
  static void kill_robot           ( GtkWidget* widget, gpointer data );
  static void increase_debug_level ( GtkWidget* widget,
                                     class ControlWindow* controlwindow_p );
  static void decrease_debug_level ( GtkWidget* widget,
                                     class ControlWindow* controlwindow_p );
  static void end_clicked          ( GtkWidget* widget, gpointer data ); 
  static void end_tournament       ( bool really );
  static void options_clicked      ( GtkWidget* widget,
                                     class ControlWindow* controlwindow_p );
  static void statistics_clicked   ( GtkWidget* widget, gpointer data );
  static void new_tournament       ( GtkWidget* widget,
                                     class ControlWindow* controlwindow_p );

private:

  GtkWidget* window_p;
  GtkWidget* debug_level_label;
};

#endif NO_GRAPHICS

#endif __CONTROL_WINDOW__
