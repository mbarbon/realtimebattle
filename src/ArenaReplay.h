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
  ArenaReplay                  ();
  ~ArenaReplay                 ();

  bool timeout_function        ();
  void start_tournament        ();
  void end_game                ();
  void set_filenames           ( String& replay_fname, String& message_fname,
                                 const String& statistics_fname,
                                 const String& option_fname );

private: 
  void update                  ();
  void start_game              ();
  void start_sequence          ();
  void end_sequence            ();
  void end_tournament          ();

  char parse_log_line          ();
  void parse_this_interval     ();

  ifstream log_file;

  double last_replay_time;
  double current_replay_time;

  bool log_from_stdin;

  // These two vars are used when creating the arena
  double arena_scale;
  int arena_succession;
};


#endif __ARENA_REPLAY__
