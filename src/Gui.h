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

#ifndef __GUI__
#define __GUI__

#ifndef NO_GRAPHICS

#include <gtk/gtk.h>
#include <math.h>

#include "Structs.h"
#include "Vector2D.h"

class MessageWindow;
class ScoreWindow;
class StatisticsWindow;
class StartTournamentWindow;

//#include "MovingObject.h"
//#include "Arena.h"
//#include "Robot.h"
//#include "String.h"

class Robot;
class String;

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

void question_yes_callback                            ( GtkWidget* widget, QuestionFunction function_name );
void question_no_callback                             ( GtkWidget* widget, QuestionFunction function_name );

void delete_event                                     ( GtkWidget* widget, gpointer data );

void no_zoom_callback                                 ( GtkWidget* widget, gpointer data );
void zoom_in_callback                                 ( GtkWidget* widget, gpointer data );
void zoom_out_callback                                ( GtkWidget* widget, gpointer data );

gint redraw_arena                                     ( GtkWidget* widget, GdkEventExpose* event, gpointer data );

class Gui
{
public:
  Gui();
  ~Gui() {}

  void setup_arena_window();
  void setup_statistics_window();

  void close_arena_window();
  void close_statistics_window();
  void close_question_window();

  void quit_event();

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

  void set_arena_window_title();

  void ask_user(String question, QuestionFunction function_name);

  double get_zoom() { return zoom; }

  bool is_messagewindow_up                     ()
    { return ( messagewindow_p != NULL ); }
  MessageWindow* get_messagewindow_p           ()
    { return messagewindow_p; }
  void open_messagewindow                      ();
  void close_messagewindow                     ();

  bool is_scorewindow_up                       ()
    { return ( scorewindow_p != NULL ); }
  ScoreWindow* get_scorewindow_p               ()
    { return scorewindow_p; }
  void open_scorewindow                        ();
  void close_scorewindow                       ();

  bool is_statisticswindow_up                  ()
    { return ( statisticswindow_p != NULL ); }
  StatisticsWindow* get_statisticswindow_p     ()
    { return statisticswindow_p; }
  void open_statisticswindow                   ();
  void close_statisticswindow                  ();

  bool is_starttournamentwindow_up             ()
    { return ( starttournamentwindow_p != NULL ); }
  StartTournamentWindow* get_starttournament_p ()
    { return starttournamentwindow_p; }
  static void kill_and_start_new_tournament    ( bool really );
  void open_starttournamentwindow              ();
  void close_starttournamentwindow             ();

private:

  //  bool statistics_up;

  GtkWidget* drawing_area;
  GtkWidget* da_scrolled_window;

  GtkWidget* arena_window;
  GtkWidget* question_window;

  Vector2D da_scrolled_window_size;

  GdkColormap* colormap;

  int zoomfactor;
  double zoom;
  Vector2D boundary[2];

  MessageWindow* messagewindow_p;
  ScoreWindow* scorewindow_p;
  StatisticsWindow* statisticswindow_p;
  StartTournamentWindow* starttournamentwindow_p;
};

#endif NO_GRAPHICS

#endif __GUI__
