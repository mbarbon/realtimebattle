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

#ifndef __START_TOURNAMENT_WINDOW__
#define __START_TOURNAMENT_WINDOW__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef NO_GRAPHICS

#include <gtk/gtk.h>

class StartTournamentWindow
{
public:

  enum min_max_full_t { MMF_MIN, MMF_MAX, MMF_FULL_ROUND, MMF_ALL_ARENAS };
  enum start_tournament_button_t
  {
    START_TORUNAMENT_REMOVE = 0,
    START_TORUNAMENT_SELECT_ALL_TOURNAMENT = 1,
    START_TORUNAMENT_UNSELECT_ALL_TOURNAMENT = 2,
    START_TORUNAMENT_ADD = 3,
    START_TORUNAMENT_SELECT_ALL_DIRECTORY = 4,
    START_TORUNAMENT_UNSELECT_ALL_DIRECTORY = 5
  };

  struct select_buttons_t
  {
    select_buttons_t                   ( bool r, int b,
                                         class StartTournamentWindow* s ) :
      robot(r), button_nr(b), stw_p(s) {}
    bool robot;
    int button_nr;
    StartTournamentWindow* stw_p;
  };

  struct min_max_full_buttons_t
  {
    min_max_full_buttons_t             ( int e, min_max_full_t t,
                                         class StartTournamentWindow* s ) :
      entry(e), type(t), stw_p(s)      {}
    int entry;
    min_max_full_t type;
    StartTournamentWindow* stw_p;
  };

  StartTournamentWindow                ( const int default_width  = -1,
                                         const int default_height = -1,
                                         const int default_x_pos  = -1,
                                         const int default_y_pos  = -1);
  ~StartTournamentWindow               ();

  GtkWidget* get_window_p              () { return window_p; }

  static void cancel_new_tournament    ( GtkWidget* widget,
                                         class StartTournamentWindow* stw_p );
  static void set_entry                ( GtkWidget* widget,
                                         struct min_max_full_buttons_t* mmf_p );
  static void kill_and_start_new_tournament( GtkWidget* widget,
                                             class StartTournamentWindow* stw_p );
  static void start                    ( GtkWidget* widget,
                                         class StartTournamentWindow* stw_p );
  static void button_selected          ( GtkWidget* widget,
                                         struct select_buttons_t* button );
  static void selection_made           ( GtkWidget * clist,
                                         gint row, gint column,
                                         GdkEventButton *event,
                                         class StartTournamentWindow* stw_p );

private:

  void add_clist                       ( GtkWidget* clist, GtkWidget* box );
  void change_all_selection            ( const bool robots,
                                         const bool dir,
                                         const bool all );
  void add_all_selected                ( const bool robots );
  void remove_all_selected             ( const bool robots );
  start_tournament_glist_info_t* find_row_in_clist( const int row, GList* glist );
  
  GtkWidget** get_entries              () { return entries; }
  GList* get_selected_robot_tournament () { return selected_robot_tournament; };
  GList* get_selected_robot_directory  () { return selected_robot_directory; };
  GList* get_selected_arena_tournament () { return selected_arena_tournament; };
  GList* get_selected_arena_directory  () { return selected_arena_directory; };
  GtkWidget* get_robots_in_tournament_clist() { return robots_in_tournament_clist; }
  GtkWidget* get_robots_in_directory_clist () { return robots_in_directory_clist; }
  GtkWidget* get_arenas_in_tournament_clist() { return arenas_in_tournament_clist; }
  GtkWidget* get_arenas_in_directory_clist () { return arenas_in_directory_clist; }

  GtkWidget* window_p;

  GtkWidget* entries[3];

  GtkWidget* robots_in_tournament_clist;
  GtkWidget* robots_in_directory_clist;
  GtkWidget* arenas_in_tournament_clist;
  GtkWidget* arenas_in_directory_clist;

  GList* selected_robot_tournament;
  GList* selected_robot_directory;
  GList* selected_arena_tournament;
  GList* selected_arena_directory;
};

#endif NO_GRAPHICS

#endif __START_TOURNAMENT_WINDOW__
