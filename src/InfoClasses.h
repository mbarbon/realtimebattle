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

#ifndef __INFOCLASSES__
#define __INFOCLASSES__

#include <string>

#include "Structs.h"
#include "Messagetypes.h"
#include "Vector2D.h"

enum infoclass_t
{
  INFO_UNKNOWN,
  INFO_STATE,
  INFO_TOURNAMENT_STARTED,
  INFO_TOURNAMENT_ENDED,
  INFO_GAME_STARTED,
  INFO_MESSAGE
};

class InfoBase
{
public:
  InfoBase                   ( const infoclass_t t = INFO_UNKNOWN ) : type(t) {}
  ~InfoBase                  () {}

  const infoclass_t get_type () const { return type; }
  const double get_game_time () const { return game_time; }

private:
  infoclass_t type;
  double game_time;
};

class StateInfo : public InfoBase
{
public:
  StateInfo                  ( const state_t s ) : InfoBase(INFO_STATE), state(s) {}
  ~StateInfo                 () {}

  const state_t get_state    () const { return state; }

private:
  state_t state;
};

class TournamentStartedInfo : public InfoBase
{
public:
  TournamentStartedInfo              ( const int nom, const int gpm )
    : InfoBase(INFO_TOURNAMENT_STARTED), number_of_matches(nom),
      games_per_match(gpm) {}
  ~TournamentStartedInfo             () {}

  const int get_number_of_matches    () const { return number_of_matches; }
  const int get_games_per_match      () const { return games_per_match; }

private:
  int number_of_matches;
  int games_per_match;
};

// Doesn't contain anything.
class TournamentEndedInfo : public InfoBase
{
public:
  TournamentEndedInfo  () : InfoBase(INFO_TOURNAMENT_ENDED) {}
  ~TournamentEndedInfo () {}
};

class GameStartedInfo : public InfoBase
{
public:
  GameStartedInfo                     ( const list<int> r, const int g, const int m
                                        //, const Arena g
                                        )
    : InfoBase(INFO_GAME_STARTED), list_of_participating_robots(r),
      game(g), match(m) //, arena_geometry(g)
    {}
  ~GameStartedInfo                    () {}

  const list<int>& get_list_of_participating_robots () const
    { return list_of_participating_robots; }
  const int get_match                 () const { return match; }
  const int get_game                  () const { return game; }
  //  const Arena get_arena_geometry  () const { return arena_geometry; }

private:
  list<int> list_of_participating_robots;
  int game;
  int match;
  // Arena arena_geometry;
};

class MessageInfo : public InfoBase
{
public:
  MessageInfo                       ( const string& s, const string& m )
    : InfoBase(INFO_MESSAGE), sender(s), message(m) {}
  ~MessageInfo                      () {}
  const string& get_sender          () const { return sender; }
  const string& get_message         () const { return message; }

private:
  string sender;
  string message;
};


#endif __INFOCLASSES__
