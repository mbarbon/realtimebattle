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

#ifndef __TOURNAMENT__
#define __TOURNAMENT__

#include <vector>
#include <string>

#include "Gadgets/ArenaGadget.h"
#include "Robot.h"


class Match
{
public:
  Match() {}
  ~Match() {}

  friend class Tournament;

  void start_new_match() {}
  void end_match() {}


private:
  
  string log_file_name;
  string stats_file_name;

  vector<int> robot_ids;
};


// The Tournament class takes care of the match schedule.
// It will start the games in order and store the results using 
// the Statistics class.

class Tournament
{
public:

//    Tournament(const int robots_p_match,
//               const int number_o_matches,
//               const vector<Robot*>& robots,
//               const vector<ArenaGadget>& arenas);

  Tournament(const int robots_p_match,
             const int number_o_matches,
             const vector<string>& robot_filenames,
             const vector<string>& arena_filenames);

  Tournament(const string& tournament_file);

  ~Tournament() {}

  void prepare_for_new_match();

  bool did_load_succeed() { return load_succeeded; }

private:

  void start();

  void create_matches();

  bool parse_tournament_file( const string& tournament_file );


  void reorder_matches(const int start, const int end);

  int total_number_of_robots;
  int robots_per_match;

  int match_nr;
  int number_of_matches;

  
  vector<Match> the_matches;
  vector<Robot> the_robots;

  vector<ArenaGadget> the_arenagadgets;
  
  bool load_succeeded;

};

#endif __TOURNAMENT__
