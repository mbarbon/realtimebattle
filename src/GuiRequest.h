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

#ifndef __GUIREQUEST__
#define __GUIREQUEST__

#include <string>
#include <vector>

class GuiRequest
{
public:
  GuiRequest                   () {}
  virtual ~GuiRequest          () {}

  virtual void accept          () const = 0;
private:
};

class QuitProgramRequest : public GuiRequest
{
public:
  QuitProgramRequest           ( const bool _success) : success(_success) {}
  ~QuitProgramRequest          () {}

  void accept                  () const;
private:
  bool success;
};

class OptionChangeRequest : public GuiRequest
{
public:
  OptionChangeRequest          ( const string& opt, const string& str_val,
                                 const bool def ) 
    : option(opt), string_value(str_val), as_default(def) {}
  ~OptionChangeRequest         () {}

  void accept                  () const;
private:
  string option;
  string string_value;
  bool as_default;
};

class TogglePauseGameRequest : public GuiRequest
{
public:
  TogglePauseGameRequest       () {}
  ~TogglePauseGameRequest      () {}

  void accept                  () const;
};

class StartTournamentRequest : public GuiRequest
{
public:
  StartTournamentRequest       ( const int rounds_pm, const int robots_pm,
                                 const int nom, const vector<string>& _robots,
                                 const vector<string>& _arenas )
    : rounds_per_match(rounds_pm), robots_per_match(robots_pm),
      number_of_matches(nom), robot_files(_robots), arena_files(_arenas) {}
  ~StartTournamentRequest      () {}
private:
  void accept                  () const;

  int rounds_per_match;
  int robots_per_match;
  int number_of_matches;
  vector<string> robot_files;
  vector<string> arena_files;
};

class EndTournamentRequest : public GuiRequest
{
public:
  EndTournamentRequest         () {}
  ~EndTournamentRequest        () {}
private:
  void accept                  () const;
};

class ChangeDebugLevelRequest : public GuiRequest
{
public:
  ChangeDebugLevelRequest    ( const int _new_level ) : new_debug_level(_new_level) {}
  ~ChangeDebugLevelRequest   () {}
  
private:
  void accept                  () const;

  int new_debug_level;
};

#endif __GUIREQUEST__
