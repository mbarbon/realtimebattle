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

class Match;


// The Tournament class takes care of the match schedule.
// It will start the games in order and store the results using 
// the Statistics class.

class Tournament
{
public:

  Tournament() {}
  ~Tournament() {}

  void prepare_for_new_match();

private:

  int round_nr;
  int rounds_per_match;
  int match_nr;
  int matches_in_tournament;

  int total_number_of_robots;
  int number_of_robots_this_match;

  
  Match* the_matches;


  Robot** the_robots;
  
}

class Match
{
  Match() {}
  ~Match() {}

  //  friend class Tournament;

  void start_new_match();
  void end_match();


private:
  
  string log_file_name;
  string stats_file_name;

  int* robot_ids;
}

class PrepareForNewMatchEvent : public Event
{
public:
  PrepareForNewMatchEvent( const double time, Tournament* t) 
    : Event(time), my_tournament(t) {}

  void eval() const { my_tournament->prepare_for_new_match(); }

protected:
  Tournament* my_tournament;
};


class StartNewMatchEvent : public Event
{
public:
  StartNewMatchEvent( const double time, Match* m) 
    : Event(time), my_match(m) {}

  void eval() const { my_match->start_new_match(); }

protected:
  Match* my_match;
};

class EndMatchEvent : public Event
{
public:
  EndMatchEvent( const double time, Match* m) 
    : Event(time), my_match(m) {}

  void eval() const { my_match->end_match(); }

protected:
  Match* my_match;
};


#endif __TOURNAMENT__
