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

#ifndef __ARENA__
#define __ARENA__

#include <glib.h>
#include <gdk/gdk.h>
#include "Vector2D.h"
#include "Messagetypes.h"
#include "Gui.h"
#include "Shape.h"
#include "Options.h"



//This fixes a problem with glib, which can give warning messages about converting NULL 
//from void* to GList* implicitly.
#undef g_list_previous
#undef g_list_next
#define g_list_previous(list) ((list) ? (((GList *)list)->prev) : (GList*)NULL)
#define g_list_next(list) ((list) ? (((GList *)list)->next) : (GList*)NULL)

static const double infinity = 1.0e10;  //approximatly (-;

extern class Options the_opts;
extern class Arena the_arena;
extern class Gui the_gui;

enum state_t { NO_STATE, NOT_STARTED, STARTING_ROBOTS, GAME_IN_PROGRESS, PAUSING_BETWEEN_GAMES,
               SHUTTING_DOWN_ROBOTS, FINISHED, EXITING };


// --------------------------  Arena --------------------------
class Arena 
{
public:

  enum game_mode_t { DEBUG_MODE, NORMAL_MODE, COMPETITION_MODE };

  Arena();
  ~Arena();

  void clear();
  void interrupt_tournament();

  double get_shortest_distance(const Vector2D& pos, const Vector2D& vel, 
                               const double size, enum object_type& closest_shape, 
                               void*& colliding_object, const class Robot* the_robot = NULL );

  bool space_available(const Vector2D& pos, const double margin);

  gint timeout_function();
  void start_tournament(const GList* robotfilenamelist, 
                        const GList* arenafilenamelist, // NULL terminated lists
                        int robots_p_game, 
                        int games_p_sequence,
                        int number_of_sequences);   

  void broadcast(enum message_to_robot_type ...);
  void set_colours();
  long find_free_colour(const long home_colour, const long away_colour, const class Robot*);
  void quit_ordered();
  void delete_lists(const bool kill_robots, const bool del_seq_list, 
                    const bool del_tourn_list, const bool del_arena_filename_list);

  Vector2D get_random_position();

  GList** get_object_lists() { return object_lists; }
  GList* get_all_robots_in_sequence() { return all_robots_in_sequence; }
  GList* get_all_robots_in_tournament() { return all_robots_in_tournament; }
  GList* get_arena_filenames() { return arena_filenames; }
  int get_sequence_nr() { return sequence_nr; }
  int get_games_per_sequence() { return games_per_sequence; }
  int get_games_remaining_in_sequence() { return games_remaining_in_sequence; }
  int get_sequences_remaining() { return sequences_remaining; }
  int get_robots_per_game() { return robots_per_game; }
  int get_current_arena_nr() { return current_arena_nr; }

  int get_robots_left() { return robots_left; }
  double get_total_time() { return (double)total_time; }
  double get_shooting_penalty();
  GdkColor* get_background_colour_p() { return &background_colour; }
  GdkColor* get_foreground_colour_p() { return &foreground_colour; }
  state_t get_state() { return state; }
  Vector2D * get_boundary() { return boundary; }
  enum game_mode_t get_game_mode() { return game_mode; }
  void set_game_mode( const enum game_mode_t gm);
  int set_debug_level( const int new_level);
  int get_debug_level() { return debug_level; }
  int get_max_debug_level() { return max_debug_level; }
  void paus_game_toggle();
  void step_paused_game();
  bool is_game_halted();
  bool get_use_proc() { return use_proc; }
  void set_use_proc(const bool v) { use_proc = v; }

  void end_game();

private:
  void parse_file(istream&);
  void check_initialization();

  void update();
  void update_timer();
  void reset_timer();
  void update_robots();
  void read_robot_messages();
  void move_shots();
  void update_explosions();

  void check_robots();
  void add_cookie();
  void add_mine();

  bool is_colour_allowed(const long colour, const double min_dist, const class Robot*);
  double colour_dist(const long col1, const GdkColor& col2);

  void start_game();
  void start_sequence();
  void start_sequence_follow_up();
  void end_sequence();
  void end_sequence_follow_up();

  void end_tournament();

  GList* object_lists[number_of_object_types];
  
  GList* all_robots_in_tournament;
  GList* all_robots_in_sequence;
  GList* exclusion_points;
  GList* arena_filenames;               // list of Strings

  int** robots_in_sequence;
  
  gdouble timestep;
  gdouble total_time;
  gdouble current_timer;

  GTimer* timer;
  
  gdouble next_check_time;

  int sequence_nr;
  int games_remaining_in_sequence;
  int games_per_sequence;
  int sequences_remaining;

  int number_of_arenas;
  int current_arena_nr;

  int number_of_robots;
  int robots_left;
  int robots_per_game;

  GdkColor background_colour;
  GdkColor foreground_colour;

  Vector2D boundary[2];   // {top-left, bottom-right}
  
  state_t state;
  game_mode_t game_mode;
  
  bool use_proc;

  int debug_level;
  int max_debug_level;

  bool halted;
  bool halt_next;
  bool paus_after_next_game;
};

// ---------------------  ArenaObject ---------------------

class ArenaObject
{
public:
  ArenaObject() {}
  virtual ~ArenaObject() {}
  virtual object_type get_object_type() = 0;
  //  class Error;
protected:
};

#endif __ARENA__
