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

class String;

struct _GtkWidget;
typedef struct _GtkWidget GtkWidget;
union _GdkEvent;
struct _GtkAdjustment;
typedef struct _GtkAdjustment GtkAdjustment;
typedef union _GdkEvent GdkEvent;
typedef void* gpointer;

struct dialog_result_t;

class ControlWindow
{
public:
  ControlWindow                    ( const int default_width  = -1,
                                     const int default_height = -1,
                                     const int default_x_pos  = -1,
                                     const int default_y_pos  = -1 );
  ~ControlWindow                   ();

  void set_window_title            ( const String& text );
  void remove_replay_widgets       ();
  void clear_extra_widgets         ();
  void display_debug_widgets       ();
  void display_replay_widgets      ();

  static void delete_event_occured ( GtkWidget* widget, GdkEvent* event,
                                     class ControlWindow* cw_p );
  static void quit_rtb             ( GtkWidget* widget,
                                     class ControlWindow* cw_p );
  static void pause                ( GtkWidget* widget,
                                     class ControlWindow* cw_p );
  static void step                 ( GtkWidget* widget, gpointer data );
  static void end_game             ( GtkWidget* widget, gpointer data );
  static void kill_robot           ( GtkWidget* widget, gpointer data );
  static void change_debug_level   ( GtkAdjustment *adj,
                                     class ControlWindow* cw_p );
  static void end_clicked          ( GtkWidget* widget, gpointer data ); 
  static void end_tournament       ( int result );
  static void options_clicked      ( GtkWidget* widget,
                                     class ControlWindow* cw_p );
  static void statistics_clicked   ( GtkWidget* widget,
                                     class ControlWindow* cw_p );
  static void new_tournament       ( GtkWidget* widget,
                                     class ControlWindow* cw_p );
  static void replay_tournament    ( GtkWidget* widget,
                                     class ControlWindow* cw_p );
  static void replay               ( GtkWidget* widget,
                                     class ControlWindow* cw_p );
  static void destroy_filesel      ( GtkWidget* widget,
                                     class ControlWindow* cw_p );
  static void kill_and_open_filesel( int result );
  void open_replay_filesel         ();

  GtkWidget* get_filesel           () { return filesel; }
  GtkWidget* get_window_p          () { return window_p; }
  void set_filesel                 ( GtkWidget* fs ) { filesel = fs; }

private:

  GtkWidget* window_p;
  GtkWidget* debug_level;
  GtkWidget* filesel;

  GtkWidget* window_hbox;
  GtkWidget* vseparator;
  GtkWidget* extra_vbox;
};

#endif NO_GRAPHICS

#endif __CONTROL_WINDOW__
