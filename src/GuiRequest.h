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

#include "Structs.h"

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
  StartTournamentRequest       ( tourn_info_t & ti ) :
    my_tournament_info ( ti ) {};
  ~StartTournamentRequest      () {}
private:
  void accept                  () const;

  tourn_info_t my_tournament_info;
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

//Send every change in the Tournament to other servers ...
class BroadCastTournamentChangeRequest : public GuiRequest
{
 public:
  BroadCastTournamentChangeRequest  (string type, string value) 
    : channel(channel), type(type), value(value) {}
  ~BroadCastTournamentChangeRequest () {}
 
 private:
  void accept                () const;
  int channel;
  string type;
  string value;
};

class OpenTournamentAgreementChannelRequest : public GuiRequest
{
 public:
  OpenTournamentAgreementChannelRequest(bool create_channel) 
    : create_channel(create_channel)
    {}
 private:
  void accept                 () const;
  bool create_channel;
};



#endif // __GUIREQUEST__
