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

class ArenaWindow;
class MessageWindow;
class ScoreWindow;
class StatisticsWindow;
class StartTournamentWindow;
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

class Gui
{
public:
  Gui();
  ~Gui() {}

  void close_question_window();

  void quit_event();

  void ask_user(String question, QuestionFunction function_name);

  bool is_arenawindow_up                       ()
    { return ( arenawindow_p != NULL ); }
  ArenaWindow* get_arenawindow_p               ()
    { return arenawindow_p; }
  void open_arenawindow                        ();
  void close_arenawindow                       ();

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

  GtkWidget* question_window;

  ArenaWindow* arenawindow_p;
  MessageWindow* messagewindow_p;
  ScoreWindow* scorewindow_p;
  StatisticsWindow* statisticswindow_p;
  StartTournamentWindow* starttournamentwindow_p;
};

#endif NO_GRAPHICS

#endif __GUI__
