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

#ifndef __ARENA_BASE__
#define __ARENA_BASE__

#ifndef NO_GRAPHICS
#include <glib.h>
#include <gdk/gdk.h>
#endif NO_GRAPHICS

#include <iostream.h>
#include <fstream.h>

#ifndef NO_GRAPHICS
#include "Gui.h"
#endif NO_GRAPHICS

#include "Vector2D.h"
#include "Messagetypes.h"
#include "Structs.h"
#include "Timer.h"
#include "List.h"
#include "Shape.h"

class ArenaController;
extern ArenaController the_arena_controller;
#define the_arena (*( the_arena_controller.get_the_arena()) )
#define realtime_arena (*( (ArenaRealTime*) (the_arena_controller.get_the_arena()) ))
#define replay_arena   (*( (ArenaReplay*) (the_arena_controller.get_the_arena()) ))

class Robot;
class String;
class Options;
class WallLine;
class WallCircle;
class WallInnerCircle;
class ArenaRealTime;

static const double infinity = 1.0e10;  //approximatly ;-)

extern class Options the_opts;
//extern class ArenaRealTime the_arena;
#ifndef NO_GRAPHICS
extern class Gui the_gui;
#endif

extern bool no_graphics;



class ArenaBase
{
public:

  ArenaBase();
  virtual ~ArenaBase();

  enum game_mode_t { DEBUG_MODE, NORMAL_MODE, COMPETITION_MODE };


  double get_shortest_distance(const Vector2D& pos, 
                               const Vector2D& vel, 
                               const double size, 
                               enum object_type& closest_shape, 
                               Shape*& colliding_object, 
                               const class Robot* the_robot = NULL );

  bool space_available(const Vector2D& pos, const double margin);


  void clear();
  virtual void interrupt_tournament();
  
  virtual bool timeout_function() = 0;

  virtual void end_game() = 0;

protected: 
  virtual void update() = 0;
  virtual void start_game() = 0;
  virtual void start_sequence() = 0;
  virtual void end_sequence() = 0;
  virtual void end_tournament() = 0;

public:

  void set_colours();

  void delete_lists(const bool kill_robots, const bool del_seq_list, 
                    const bool del_tourn_list, const bool del_arena_filename_list);
 void save_statistics_to_file(String filename);



  List<Shape>* get_object_lists() { return object_lists; }
  List<Robot>* get_all_robots_in_sequence() { return &all_robots_in_sequence; }
  List<Robot>* get_all_robots_in_tournament() { return &all_robots_in_tournament; }
  String get_current_arena_filename() { return current_arena_filename; }
  int get_sequence_nr() { return sequence_nr; }
  int get_games_per_sequence() { return games_per_sequence; }
  int get_games_remaining_in_sequence() { return games_remaining_in_sequence; }
  int get_sequences_remaining() { return sequences_remaining; }
  int get_robots_per_game() { return robots_per_game; }
  int get_number_of_robots() { return number_of_robots; }
  

  int get_robots_left() { return robots_left; }
  double get_total_time() { return (double)total_time; }
  double get_shooting_penalty();

  int increase_robot_count()  { return robot_count++; }
  int increase_shot_count()   { return shot_count++; }
  int increase_cookie_count() { return cookie_count++; }
  int increase_mine_count()   { return mine_count++; }

#ifndef NO_GRAPHICS
  GdkColor* get_bg_gdk_colour_p() { return &bg_gdk_colour; }
  GdkColor* get_fg_gdk_colour_p() { return &fg_gdk_colour; }
  GdkColor* get_rtb_message_gdk_colour_p() { return &rtb_message_gdk_colour; }
#endif NO_GRAPHICS
  long int get_bg_rgb_colour() { return bg_rgb_colour; }
  long int get_fg_rgb_colour() { return fg_rgb_colour; }

  state_t get_state() { return state; }
  Vector2D * get_boundary() { return boundary; }
  enum game_mode_t get_game_mode() { return game_mode; }
  void set_game_mode( const enum game_mode_t gm);

  int get_debug_level() { return debug_level; }
  bool is_max_debug_level() { return debug_level == max_debug_level; }
  virtual int set_debug_level( const int new_level);

  void pause_game_toggle();
  void step_paused_game();

protected:

  void update_timer();
  void reset_timer();

  void move_shots(const double time_period); 

  void parse_arena_line(ifstream& file, double& scale, int& succession);

  void set_state( const state_t );

  List<Shape> object_lists[LAST_OBJECT_TYPE];
  
  List<Robot> all_robots_in_tournament;
  List<Robot> all_robots_in_sequence;
  List<Vector2D> exclusion_points;

  String current_arena_filename;
  List<String> arena_filenames;               // list of Strings

  int** robots_in_sequence;


  String statistics_file_name;

  bool auto_start_and_end;

  int robot_count;
  int shot_count;
  int cookie_count;
  int mine_count;

  double timestep;
  double total_time;
  double current_timer;

  Timer timer;
  
  double next_check_time;

  int sequence_nr;
  int games_remaining_in_sequence;
  int games_per_sequence;
  int sequences_remaining;

  int number_of_robots;
  int robots_left;
  int robots_per_game;


#ifndef NO_GRAPHICS
  GdkColor bg_gdk_colour;
  GdkColor fg_gdk_colour;
  GdkColor rtb_message_gdk_colour;
#endif NO_GRAPHICS
  long int bg_rgb_colour;
  long int fg_rgb_colour;
  long int rtb_message_rgb_colour;

  Vector2D boundary[2];   // {top-left, bottom-right}
  
  state_t state;
  game_mode_t game_mode;
  
  int debug_level;
  int max_debug_level;

  bool halt_next;
  bool pause_after_next_game;
};


#endif __ARENA_BASE__
