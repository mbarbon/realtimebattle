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

#ifndef __CONTROL_WINDOW__
#define __CONTROL_WINDOW__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string>

#include "Structs.h"

static const int max_debug_level = 5;

struct _GtkWidget;
typedef struct _GtkWidget GtkWidget;
struct _GtkItemFactory;
typedef struct _GtkItemFactory GtkItemFactory;
union _GdkEvent;
struct _GtkAdjustment;
typedef struct _GtkAdjustment GtkAdjustment;
typedef union _GdkEvent GdkEvent;
typedef void* gpointer;

struct dialog_result_t;
template<class T> class FileSelector;

class ControlWindow
{
public:
  ControlWindow                    ( const int default_width  = -1,
                                     const int default_height = -1,
                                     const int default_x_pos  = -1,
                                     const int default_y_pos  = -1 );
  ~ControlWindow                   ();

  void set_status                  ( const state_t& state );
  string get_status_string         ( const state_t& state ) const;
  string get_matchinfo_string      ( const int& time, const int& match_nr,
                                     const int& matches_per_round );
  void set_matchinfo               ( const int& time, const int& match_nr,
                                     const int& matches_per_round );
  void set_debug_sensitivity       ( const bool );
  void set_replay_sensitivity       ( const bool );
  void replay                      ( const string& );


  static void change_debug_level   ( GtkAdjustment *adj,
                                     class ControlWindow* cw_p );
  static void rewind_pressed       ( GtkWidget* widget,
                                     class ControlWindow* cw_p );
  static void rewind_released      ( GtkWidget* widget,
                                     class ControlWindow* cw_p );
  static void fast_forward_pressed ( GtkWidget* widget,
                                     class ControlWindow* cw_p );
  static void fast_forward_released( GtkWidget* widget,
                                     class ControlWindow* cw_p );
  static void change_current_replay_time( GtkAdjustment *adj,
                                          class ControlWindow* cw_p );

  static void kill_and_open_filesel( int result );
  void open_replay_filesel         ();

  GtkWidget* get_window_p          () { return window_p; }

  bool is_arenawindow_checked      ();
  bool is_messagewindow_checked    ();
  bool is_scorewindow_checked      ();

  GtkWidget* get_show_arena_menu_item() { return show_arena_menu_item; }
  GtkWidget* get_show_message_menu_item() { return show_message_menu_item; }
  GtkWidget* get_show_score_menu_item() { return show_score_menu_item; }

  void set_progress_time           ( const double time );
  void change_time_limitations     ();

private:
  enum menu_t
  { 
    MENU_LOGFILE, MENU_MESSAGEFILE, MENU_QUIT, MENU_NEW_TOURNAMENT,
    MENU_REPLAY_TOURNAMENT, MENU_PAUSE, MENU_END, MENU_OPTIONS,
    MENU_STATISTICS, MENU_SHOW_ARENA, MENU_SHOW_MESSAGES,
    MENU_SHOW_SCORE, MENU_STEP, MENU_END_MATCH, MENU_KILL_MARKED_ROBOT,
    MENU_STEP_FORWARD, MENU_STEP_BACKWARD, MENU_NEXT_MATCH,
    MENU_PREV_MATCH, MENU_NEXT_ROUND, MENU_PREV_ROUND, MENU_ABOUT
  };

  GtkWidget* new_button_from_xpm_d ( char**, const int xsize = -1,
                                     const int ysize = -1 );

  void show_about                  ();
  char* translate_menu_path        ( const char* );
  void menu_set_sensitive          ( const char*, bool sensitive = true );

  void destroy_about               ();

  static void delete_event_occured ( GtkWidget* widget, GdkEvent* event,
                                     class ControlWindow* cw_p );
  static void quit_rtb             ( GtkWidget* widget,
                                     class ControlWindow* cw_p );
  static void menu_callback        ( class ControlWindow* cw_p,
                                     guint callback_action, GtkWidget *widget );
  static void about_callback       ( GtkWidget* widget,
                                     class ControlWindow* cw_p );

  GtkWidget* window_p;
  GtkItemFactory* item_factory;
  GtkWidget* status_label;
  GtkWidget* matchinfo_label;
  GtkWidget* debug_level;

  GtkWidget* rewind_button;
  GtkWidget* fast_forward_button;
  GtkWidget* time_control;
  GtkAdjustment* current_replay_time_adjustment;
  GtkWidget* show_arena_menu_item;
  GtkWidget* show_message_menu_item;
  GtkWidget* show_score_menu_item;

  GtkWidget* about_window;

  // FileSelector
  FileSelector<ControlWindow>* get_filesel () { return filesel; }
  FileSelector<ControlWindow>* filesel;
};

#endif __CONTROL_WINDOW__
