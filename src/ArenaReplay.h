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

#ifndef __ARENA_REPLAY__
#define __ARENA_REPLAY__

#include "ArenaBase.h"
#include "fstream.h"

class String;

class ArenaReplay : public ArenaBase
{
public:
  //  enum speed_t { REWIND, PLAY, FAST_FORWARD };

  ArenaReplay                   ();
  ~ArenaReplay                  ();

  bool timeout_function         ();
  void start_tournament         ();
  void end_game                 ();
  void set_filenames            ( String& replay_fname, String& message_fname,
                                  const String& statistics_fname,
                                  const String& option_fname );
  void   change_speed           ( const bool forward, const bool fast );
  void   change_game            ( const int inc_game, const int inc_seq );
  bool   step_forward           ( const int n_o_steps, const bool clear_time = true );
  void   change_replay_time     ( const double time );

  char   search_forward         ( const String& search_letters );
  String search_forward         ( const List<String>& search_strings );
  String search_backwards       ( const String& search_letters );

  double get_current_replay_time() { return current_replay_time; }
  double get_length_of_current_game();

private: 
  void update                   ();
  void start_game               ();
  void start_sequence           ();
  void end_sequence             ();
  void end_tournament           ();

  char parse_log_line           ();
  void parse_this_interval      ();
  void parse_this_time_index    ();
  void beginning_of_prev_line   ();
  void beginning_of_current_line();

  void make_statistics_from_file();
  void get_time_positions_in_game();
  int  find_streampos_for_time  ( const float cur_time );

  ifstream log_file;

  //  double last_replay_time;
  double current_replay_time;

  bool log_from_stdin;

  //  speed_t speed;
  double fast_forward_factor;

  streampos** game_position_in_log;

  struct time_pos_info_t
  {
    streampos pos;
    float time;
  };

  struct object_pos_info_t
  {
    object_type obj;
    streampos pos;
    float start_time;
    float end_time;
    Vector2D vel; // For shots
  };

  time_pos_info_t* time_position_in_log;
  int last_time_info;

  // These two vars are used when creating the arena
  double arena_scale;
  int arena_succession;
};


#endif __ARENA_REPLAY__
