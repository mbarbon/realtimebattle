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

#ifndef __GUI__
#define __GUI__

#ifndef NO_GRAPHICS

#include <gtk/gtk.h>
#include <list>

#include "Messagetypes.h"
#include "DrawingObjects.h"

class GuiInterface;
class ArenaWindow;
class MessageWindow;
class ScoreWindow;
class StatisticsWindow;
class StartTournamentWindow;
class GuiInterface;

#define the_gui (*(gui_p))

class Gui
{
public:
  Gui();
  ~Gui() {}

  int main_loop                                ( GuiInterface* );
  int timeout_function                         ();
  void get_information                         ();
  void update_lists                            ();
  void set_colours                             ();

  list<DrawingShape*>* get_drawing_objects_lists() { return drawing_objects_lists; }

  const int get_number_of_matches              () const { return number_of_matches; }
  const int get_games_per_match                () const { return games_per_match; }
  const int get_match_nr                       () const { return match_nr; }
  const int get_game_nr                        () const { return game_nr; }

  const double get_match_time                  () const { return match_time; }

  GdkColor* get_bg_gdk_colour_p                () { return &bg_gdk_colour; }
  GdkColor* get_fg_gdk_colour_p                () { return &fg_gdk_colour; }
  GdkColor* get_rtb_message_gdk_colour_p       () { return &rtb_message_gdk_colour; }

  long int get_bg_rgb_colour                   () const { return bg_rgb_colour; }
  long int get_fg_rgb_colour                   () const { return fg_rgb_colour; }
  long int get_rtb_message_rgb_colour          () const 
  { return rtb_message_rgb_colour; }

  bool is_arenawindow_up                       () const
    { return ( arenawindow_p != NULL ); }
  ArenaWindow* get_arenawindow_p               () const
    { return arenawindow_p; }
  void open_arenawindow                        ();
  void close_arenawindow                       ();

  bool is_messagewindow_up                     () const
    { return ( messagewindow_p != NULL ); }
  MessageWindow* get_messagewindow_p           () const
    { return messagewindow_p; }
  void open_messagewindow                      ();
  void close_messagewindow                     ();

  bool is_scorewindow_up                       () const
    { return ( scorewindow_p != NULL ); }
  ScoreWindow* get_scorewindow_p               () const
    { return scorewindow_p; }
  void open_scorewindow                        ();
  void close_scorewindow                       ();

  bool is_statisticswindow_up                  () const
    { return ( statisticswindow_p != NULL ); }
  StatisticsWindow* get_statisticswindow_p     () const
    { return statisticswindow_p; }
  void open_statisticswindow                   ();
  void close_statisticswindow                  ();

  bool is_starttournamentwindow_up             () const
    { return ( starttournamentwindow_p != NULL ); }
  StartTournamentWindow* get_starttournament_p () const
    { return starttournamentwindow_p; }
  static void kill_and_start_new_tournament    ( int result );
  void open_starttournamentwindow              ();
  void close_starttournamentwindow             ();

  static gint update_function                  (gpointer data);

private:

  GuiInterface* guiinterface_p;

  list<DrawingShape*> drawing_objects_lists[LAST_OBJECT_TYPE];

  double match_time;

  int number_of_matches;
  int games_per_match;
  int match_nr;
  int game_nr;

  long int bg_rgb_colour;
  long int fg_rgb_colour;
  long int rtb_message_rgb_colour;

  GdkColor bg_gdk_colour;
  GdkColor fg_gdk_colour;
  GdkColor rtb_message_gdk_colour;

  ArenaWindow* arenawindow_p;
  MessageWindow* messagewindow_p;
  ScoreWindow* scorewindow_p;
  StatisticsWindow* statisticswindow_p;
  StartTournamentWindow* starttournamentwindow_p;
};

#endif NO_GRAPHICS

#endif __GUI__
