/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2001  Erik Ouchterlony and Ragnar Ouchterlony

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
#include "Structs.h"


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

  Tournament();       //Need to have attributes one by one using the GUI and the Network

  /* NOTE : No use unless having a dynamic tournament.
            As we can run only one tournament at the time (can't we ?) no need for it 
  */
  Tournament(const int robots_p_match,
             const int number_o_matches,
             const vector<string>& robot_filenames,     //Robot_filenames not enough : also need the computer who will run it
             const vector<string>& arena_filenames);    //As robot (Or maybe the arena have to be localy stored)

  Tournament(const tourn_info_t& new_tournament);
  
  Tournament(const string& tournament_file);
  
  ~Tournament() {}

  void prepare_for_new_match();

  bool load_file( const string & tournament_file ) { return parse_tournament_file(tournament_file); }

  bool did_load_succeed() { return load_succeeded; }

  void reset();

  bool add_Robot(Robot* robot);
  bool add_Arena(string arena_filename);
  bool set_number_o_matches(int);
  bool set_robots_p_match(int);

  vector<Robot*>* get_the_robots(){return &the_robots;};

  void start();

private:

  void create_matches();

  bool parse_tournament_file( const string& tournament_file );


  void reorder_matches(const int start, const int end);

  int total_number_of_robots;
  int robots_per_match;

  int match_nr;
  int number_of_matches;

  bool started;

  vector<Match> the_matches;
  vector<Robot*> the_robots;  //Use a set instead of a vector (as for the arena)

  //NOTE : put the arena filenames directly in the_arenagadgets when ready
  vector<string> arena_filenames; 

  vector<ArenaGadget> the_arenagadgets;
  
  bool load_succeeded;

  tourn_info_t my_tournament_info;

};

extern Tournament the_tournament;

#endif // __TOURNAMENT__
