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

#ifndef __ARENA_REALTIME__
#define __ARENA_REALTIME__

#include "Arena_Base.h"

class Arena_RealTime : public Arena_Base
{
public:
  Arena_RealTime();
  ~Arena_RealTime();

  gint timeout_function();
  void start_tournament(const GList* robotfilenamelist, 
                        const GList* arenafilenamelist,
                        int robots_p_game, 
                        int games_p_sequence,
                        int number_of_sequences);   

  void end_game();

  void broadcast(enum message_to_robot_type ...);
  
  void search_directories( String directory, GList* dir_gl,
                           const bool check_robots );
  void check_for_robots_and_arenas( String& word, GList* tour_gl,
                                    GList* dir_gl, const bool check_robots );
  void parse_tournament_file( String& fname );
  void parse_arena_file(String& filename);
  
  void set_filenames(String& log_fname, const String& statistics_fname, 
                     const String& tournament_fname, const String& option_fname);  

  void print_to_logfile(const char first_letter ... );
  
  Vector2D get_random_position();
  GList* get_arena_filenames() { return arena_filenames; }
  int get_current_arena_nr() { return current_arena_nr; }

  int set_debug_level( const int new_level);

  int get_max_debug_level() { return max_debug_level; }

  void quit_ordered();

  long int find_free_colour(const long int home_colour, const long int away_colour, 
                            const class Robot*);


private:

  void check_initialization();

  void update();  

  void update_robots();
  void read_robot_messages();

  void check_robots();
  void add_cookie();
  void add_mine();

  bool is_colour_allowed(const long int col, const double min_dist, const class Robot*);
  double colour_dist(const long int col1, const long int col2);

  void start_game();
  void start_sequence();
  void end_sequence();
  void start_sequence_follow_up();
  void end_sequence_follow_up();
  void end_tournament();

  String option_file_name;

  String tournament_file_name;
  
  ofstream LOG_FILE;
  bool use_log_file;

  int number_of_arenas;
  int current_arena_nr;
};


#endif __ARENA_REALTIME__