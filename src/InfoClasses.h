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
#include "Various.h"

enum infoclass_t
{
  INFO_UNKNOWN,
  INFO_QUIT,
  INFO_STATE,
  INFO_TOURNAMENT_STARTED,
  INFO_TOURNAMENT_ENDED,
  INFO_GAME_STARTED,
  INFO_MESSAGE
};

// For shape positions supplied via the InfoClasses

enum object_shape_t { SHAPE_LINE, SHAPE_CIRCLE, SHAPE_INNER_CIRCLE, SHAPE_ARC };

struct object_pos_info
{
  object_pos_info( object_shape_t t ) : type(t) {}
  object_shape_t type;
};

struct object_line_pos_info : public object_pos_info
{
  object_line_pos_info( const Vector2D& sp, const Vector2D& dir,
                        const double len, const double th )
    : object_pos_info( SHAPE_LINE ), start_point(sp), direction(dir),
      length(len), thickness(th) {}
  Vector2D start_point;
  Vector2D direction;
  double length;
  double thickness;
};

struct object_circle_pos_info : public object_pos_info
{
  object_circle_pos_info( const Vector2D& c, const double r )
    : object_pos_info( SHAPE_CIRCLE ), center(c), radius(r) {}
  Vector2D center;
  double radius;
};

struct object_inner_circle_pos_info : public object_pos_info
{
  object_inner_circle_pos_info( const Vector2D& c, const double r )
    : object_pos_info( SHAPE_INNER_CIRCLE ), center(c), radius(r) {}
  Vector2D center;
  double radius;
};

struct object_arc_pos_info : public object_pos_info
{
  object_arc_pos_info( const Vector2D& c, const double ir, const double or,
                       const double sa, const double ea )
    : object_pos_info( SHAPE_ARC ), center(c),
      inner_radius(ir), outer_radius(or), start_angle(sa), end_angle(ea) {}
  Vector2D center;
  double inner_radius;
  double outer_radius;
  double start_angle;
  double end_angle;
};

// ---------------------------------------------------------------------------
// class InfoBase
// ---------------------------------------------------------------------------
// The base class for all InfoClasses
// ---------------------------------------------------------------------------

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

// All other InfoClasses.
class QuitInfo : public InfoBase
{
public:
  QuitInfo                   () : InfoBase(INFO_QUIT) {}
  ~QuitInfo                  () {}
private:
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
  // TODO: supply gui with all robots.

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
  GameStartedInfo                     ( const list<int> r,
                                        const list<object_pos_info*> w,
                                        const int g, const int m )
    : InfoBase(INFO_GAME_STARTED), list_of_participating_robots(r),
      list_of_walls(w), game(g), match(m) {}
  ~GameStartedInfo                    ()
  {
    clear_and_delete_pointer_list( list_of_walls );
  }

  const list<int>& get_list_of_participating_robots () const // list of robot_ids
    { return list_of_participating_robots; }
  const list<object_pos_info*>& get_list_of_walls () const { return list_of_walls; }
  const int get_match                 () const { return match; }
  const int get_game                  () const { return game; }

private:
  list<int> list_of_participating_robots;
  list<object_pos_info*> list_of_walls;
  int game;
  int match;
};

// MessageInfo type is most efficient when used with several messages,
// e.g. should contain all messages since the last time MessageInfo was sent.
class MessageInfo : public InfoBase
{
public:
  struct message_t
  {
    message_t( const string& s, const string& m )
      : sender(s), message(m) {}
    string sender;
    string message;
  };
  MessageInfo                       ( const string& s, const string& m )
    : InfoBase(INFO_MESSAGE) { message_list.push_back( message_t( s, m ) ); }
  MessageInfo                       ( const list<message_t>& ml )
    : InfoBase(INFO_MESSAGE), message_list(ml) {}
  ~MessageInfo                      () {}
  const list<message_t>& get_message_list () const { return message_list; }

private:
  list<message_t> message_list;
};


#endif __INFOCLASSES__

