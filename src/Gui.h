/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998  Erik Ouchterlony and Ragnar Ouchterlony

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

#ifndef __GUI__
#define __GUI__

#include <gtk/gtk.h>
#include <math.h>
#include "Vector2D.h"
#include "MovingObject.h"
//#include "Arena.h"
#include "String.h"

typedef void (*QuestionFunction)(bool);

enum zoom_t { NO_ZOOM, ZOOM_IN, ZOOM_OUT };
enum min_max_full_t { MMF_MIN, MMF_MAX, MMF_FULL_ROUND, MMF_ALL_ARENAS };
enum stat_button_t
{
  STAT_BUTTON_TOTAL = 0,
  STAT_BUTTON_SEQUENCE = 1,
  STAT_BUTTON_GAME = 2,
  STAT_BUTTON_ROBOT = 3,
  STAT_BUTTON_FIRST = 4,
  STAT_BUTTON_PREV = 5,
  STAT_BUTTON_UPDATE = 6,
  STAT_BUTTON_NEXT = 7,
  STAT_BUTTON_LAST = 8
};

enum stat_table_t
{
  STAT_TABLE_TOTAL,
  STAT_TABLE_SEQUENCE,
  STAT_TABLE_GAME,
  STAT_TABLE_ROBOT
};

enum start_tournament_button_t
{
  START_TORUNAMENT_REMOVE = 0,
  START_TORUNAMENT_SELECT_ALL_TOURNAMENT = 1,
  START_TORUNAMENT_UNSELECT_ALL_TOURNAMENT = 2,
  START_TORUNAMENT_ADD = 3,
  START_TORUNAMENT_SELECT_ALL_DIRECTORY = 4,
  START_TORUNAMENT_UNSELECT_ALL_DIRECTORY = 5
};

struct start_tournament_glist_info_t
{
  start_tournament_glist_info_t(const int r, const bool s, const class String& fn, const class String& dir) :
    row(r), selected(s), filename(fn), directory(dir) {}
  int row;
  bool selected;
  String filename;
  String directory;
};

void question_yes_callback(GtkWidget * widget, QuestionFunction function_name);
void question_no_callback(GtkWidget * widget, QuestionFunction function_name);
void new_robot_selected( GtkWidget * clist, gint row, gint column,
                         GdkEventButton *event, gpointer data );
void pause_button_callback(GtkWidget * widget, gpointer data);
void step_button_callback(GtkWidget * widget, gpointer data);
void end_game_button_callback(GtkWidget * widget, gpointer data);
void kill_robot_button_callback(GtkWidget * widget, gpointer data);
void decrease_debug_level_button_callback(GtkWidget * widget, gpointer data);
void increase_debug_level_button_callback(GtkWidget * widget, gpointer data);
void end_button_callback(GtkWidget * widget, gpointer data);
void end_tournament(bool really);
void statistics_button_callback(GtkWidget *widget, gpointer data);
void start_tournament_min_callback(GtkWidget *widget, gpointer data);
void start_tournament_max_callback(GtkWidget *widget, gpointer data);
void start_tournament_full_round_callback(GtkWidget *widget, gpointer data);
void start_tournament_all_arenas_callback(GtkWidget *widget, gpointer data);
void start_tournament_button_callback(GtkWidget *widget, gpointer data);
void kill_and_start_new_tournament(bool really);
void delete_event( GtkWidget * widget, gpointer data );
void no_zoom_callback(GtkWidget *widget, gpointer data);
void zoom_in_callback(GtkWidget *widget, gpointer data);
void zoom_out_callback(GtkWidget *widget, gpointer data);
gint redraw_arena (GtkWidget *widget, GdkEventExpose *event, gpointer data);

void save_statistics_callback(GtkWidget *widget, gpointer data);
void save_stats_filesel_ok_selected (GtkWidget* widget, GtkFileSelection *fs);
void destroy_stats_filesel();
void buttons_in_statistics_callback(GtkWidget *widget, gpointer type_p);

void start_tournament_start_callback(GtkWidget *widget, gpointer data);
void start_tournament_select_robots_buttons_callback(GtkWidget *widget, gpointer button_number_p);
void start_tournament_select_arenas_buttons_callback(GtkWidget *widget, gpointer button_number_p);
void start_tournament_selection_made( GtkWidget * clist, gint row, gint column,
                                      GdkEventButton *event, gpointer data );

class Gui
{
public:
  Gui();
  ~Gui();

  void setup_control_window();
  void setup_score_window();
  void setup_message_window();
  void setup_arena_window();
  void setup_start_tournament_window();
  void setup_statistics_window();

  void close_control_window();
  void close_score_window();
  void close_message_window();
  void close_arena_window();
  void close_start_tournament_window();
  void close_statistics_window();
  void close_question_window();

  void quit_event();

  void print_to_message_output(String from_robot, String text);
  void draw_objects(const bool clear_objects_first);
  void draw_all_walls();

  void change_zoom();
  void change_zoomfactor( const zoom_t type );
  int change_to_pixels_x(const double input);
  int change_to_pixels_y(const double input);

  void clear_area();
  void draw_circle( const Vector2D& center, const double radius, GdkColor& colour, const bool filled );
  void draw_line( const Vector2D& start, const Vector2D& end, GdkColor& colour );
  void draw_line( const Vector2D& start, const Vector2D& end, const double length, 
                  const double thickness, GdkColor& colour );
  void draw_rectangle( const Vector2D& start, const Vector2D& end, GdkColor& colour, const bool filled );

  void change_stat_type( stat_table_t type );
  void change_statistics( int change, bool absoult_change );
  void add_new_row( void* rp, void* sp, int games_played );
  void add_the_statistics_to_clist();
  void stat_make_title_button();
  void save_statistics_to_file(String filename);

  void set_control_window_title( const bool halted );
  void set_score_window_title();
  void set_arena_window_title();
  void add_robots_to_score_list();

  void change_selected_robot( const int row );
  void display_new_debug_level( const int debug_level );

  void start_new_tournament();
  void start_tournament_change_all_selection(bool robots, bool dir, bool all);
  void start_tournament_change_one_selection(const int row, const GtkWidget * clist);
  start_tournament_glist_info_t* start_tournament_find_row_in_clist(const int row, GList * lista);
  void start_tournament_add_all_selected( bool robots );
  void start_tournament_remove_all_selected( bool robots );
  void set_entry(const int entry, const enum min_max_full_t mmf);

  void ask_user(String question, QuestionFunction function_name);

  bool get_statistics_up() { return statistics_up; }
  bool get_start_tournament_up() { return start_tournament_up; }
  GtkWidget* get_score_clist() { return score_clist; }
  GList* get_robotdirs() { return robotdirs; }
  GList* get_arenadirs() { return arenadirs; }
  Robot* get_selected_robot() { return selected_robot; }
  double get_zoom() { return zoom; }
  GtkWidget* get_filesel_widget() { return filesel_widget; }
  void set_filesel_widget(GtkWidget* filesel) { filesel_widget = filesel; }

private:
  void read_dirs_from_system();
  void split_colonseparated_dirs(String& dirs, GList * gl);

  bool statistics_up;
  bool start_tournament_up;

  GtkWidget* message_clist;
  GtkWidget* drawing_area;
  GtkWidget* da_scrolled_window;

  GtkWidget* score_clist;
  GtkWidget* stat_clist;
  GtkWidget* stat_title_button;
  GtkWidget* stat_title_hbox;

  GtkWidget* filesel_widget;

  Robot* selected_robot;
  GtkWidget* debug_level_widget;

  GtkWidget* robots_in_tournament_clist;
  GtkWidget* robots_in_directory_clist;
  GtkWidget* arenas_in_tournament_clist;
  GtkWidget* arenas_in_directory_clist;

  GtkWidget* start_tournament_entries[3];

  GtkWidget* control_window;
  GtkWidget* score_window;
  GtkWidget* message_window;
  GtkWidget* arena_window;
  GtkWidget* start_tournament_window;
  GtkWidget* statistics_window;
  GtkWidget* question_window;

  Vector2D control_window_size;
  Vector2D start_tournament_window_size;

  Vector2D da_scrolled_window_size;

  GdkColormap* colormap;

  GList* selected_items_in_robot_tournament;
  GList* selected_items_in_robot_directory;
  GList* selected_items_in_arena_tournament;
  GList* selected_items_in_arena_directory;

  GList* robotdirs;
  GList* arenadirs;

  int zoomfactor;
  double zoom;
  Vector2D boundary[2];

  stat_table_t stat_table_type;
  int stat_looking_at_nr;
};

#endif
