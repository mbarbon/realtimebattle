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

class InfoBase
{
public:
  InfoBase::InfoBase                () {}
  InfoBase::~InfoBase               () {}

private:
};

class GameTimeInfo : public InfoBase
{
public:
  GameTimeInfo::GameTimeInfo  ( const double t ) : game_time(t) {}
  GameTimeInfo::~GameTimeInfo () {}

  const double get_game_time  () const { return game_time; }

private:
  double game_time;
};

class StateInfo : public InfoBase
{
public:
  StateInfo::StateInfo       ( const state_t s )
    : state(s) {}
  StateInfo::~StateInfo      () {}

  const state_t get_state    () const { return state; }

private:
  state_t state;
};

class TournamentStartedInfo : public InfoBase
{
  //  TournamentStartedInfo::TournamentStartedInfo   
  //  TournamentStartedInfo::~TournamentStartedInfo  () {}
};

class StartGameInfo : public InfoBase
{
public:
  StartGameInfo::StartGameInfo    ( const list<int> r//, const Arena g
                                            )
    : list_of_participating_robots(r)//, arena_geometry(g)
    {}
  StartGameInfo::~StartGameInfo   () {}

  const list<int>& get_list_of_participating_robots () const
    { return list_of_participating_robots; }
  //  const Arena get_arena_geometry       () const { return arena_geometry; }

private:
  list<int> list_of_participating_robots;
  // Arena arena_geometry;
};

class ObjectMovedInfo : public InfoBase
{
public:
  ObjectMovedInfo::ObjectMovedInfo  ( const object_type t, const int i,
                                              const Vector2D& pos )
    : type(t), id(i), position(pos) {}
  ObjectMovedInfo::~ObjectMovedInfo () {}

  const object_type get_type        () const { return type; }
  const int get_id                  () const { return id; }
  const Vector2D get_position       () const { return position; }

private:
  object_type type;
  int id;
  Vector2D position;
};

class ObjectCreatedInfo : public InfoBase
{
public:
  ObjectCreatedInfo::ObjectCreatedInfo  ( const object_type t, const int i )
    : type(t), id(i) {}
  ObjectCreatedInfo::~ObjectCreatedInfo () {}

  const object_type get_type            () const { return type; }
  const int get_id                      () const { return id; }

private:
  object_type type;
  int id;
};

class ObjectDiedInfo : public InfoBase
{
public:
  ObjectDiedInfo::ObjectDiedInfo    ( const object_type t, const int i )
    : type(t), id(i) {}
  ObjectDiedInfo::~ObjectDiedInfo   () {}

  const object_type get_type        () const { return type; }
  const int get_id                  () const { return id; }

private:
  object_type type;
  int id;
};

class MessageInfo : public InfoBase
{
public:
  MessageInfo::MessageInfo          ( const string& s, const string& m )
    : sender(s), message(m) {}
  MessageInfo::~MessageInfo         () {}
  const string& get_sender          () const { return sender; }
  const string& get_message         () const { return message; }

private:
  string sender;
  string message;
};


#endif __INFOCLASSES__
