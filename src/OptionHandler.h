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

#ifndef __OPTIONHANDLER__
#define __OPTIONHANDLER__

#include "Option.h"
#include <string>
#include <vector>

enum option_long_t
{
  OPTION_MAX_ROBOTS_ALLOWED, 

  OPTION_ROBOT_ENERGY_LEVELS,

  OPTION_BACKGROUND_COLOUR,
  OPTION_FOREGROUND_COLOUR,
  OPTION_RTB_MESSAGE_COLOUR,
  OPTION_COOKIE_COLOUR,
  OPTION_MINE_COLOUR,

  OPTION_LOG_EVERY_NTH_UPDATE_INTERVAL,

  OPTION_ARENA_WINDOW_SIZE_X,
  OPTION_ARENA_WINDOW_SIZE_Y,
  OPTION_ARENA_WINDOW_POS_X,
  OPTION_ARENA_WINDOW_POS_Y,
  OPTION_CONTROL_WINDOW_POS_X,
  OPTION_CONTROL_WINDOW_POS_Y,
  OPTION_MESSAGE_WINDOW_SIZE_X,
  OPTION_MESSAGE_WINDOW_SIZE_Y,
  OPTION_MESSAGE_WINDOW_POS_X,
  OPTION_MESSAGE_WINDOW_POS_Y,
  OPTION_SCORE_WINDOW_SIZE_X,
  OPTION_SCORE_WINDOW_SIZE_Y,
  OPTION_SCORE_WINDOW_POS_X,
  OPTION_SCORE_WINDOW_POS_Y,
  OPTION_STATISTICS_WINDOW_SIZE_X,
  OPTION_STATISTICS_WINDOW_SIZE_Y,

  LAST_LONG_OPTION
};

enum option_double_t
{
  OPTION_GRAV_CONST,
  OPTION_AIR_RESISTANCE,
  OPTION_ROLL_FRICTION,
  OPTION_SLIDE_FRICTION,
  OPTION_ROBOT_MAX_ACCELERATION,
  OPTION_ROBOT_MIN_ACCELERATION,

  OPTION_ROBOT_RADIUS,
  OPTION_ROBOT_MASS,

  OPTION_ROBOT_BOUNCE_COEFF,
  OPTION_ROBOT_HARDNESS,
  OPTION_ROBOT_PROTECTION,

  OPTION_ROBOT_FRONT_BOUNCE_COEFF,
  OPTION_ROBOT_FRONT_HARDNESS,
  OPTION_ROBOT_FRONT_PROTECTION,

  OPTION_ROBOT_FRONTSIZE,

  OPTION_ROBOT_START_ENERGY,
  OPTION_ROBOT_MAX_ENERGY,

  OPTION_ROBOT_MAX_ROTATE,
  OPTION_ROBOT_CANNON_MAX_ROTATE,
  OPTION_ROBOT_RADAR_MAX_ROTATE,

  OPTION_SHOT_RADIUS,
  OPTION_SHOT_SPEED,
  OPTION_SHOOTING_PENALTY,
  OPTION_SHOT_MIN_ENERGY,
  OPTION_SHOT_MAX_ENERGY,
  OPTION_SHOT_ENERGY_INCREASE_SPEED,

  OPTION_COOKIE_MAX_ENERGY,
  OPTION_COOKIE_MIN_ENERGY,
  OPTION_COOKIE_FREQUENCY,
  OPTION_COOKIE_RADIUS,

  OPTION_MINE_MAX_ENERGY,
  OPTION_MINE_MIN_ENERGY,
  OPTION_MINE_FREQUENCY,
  OPTION_MINE_RADIUS,

  OPTION_ARENA_SCALE,
  OPTION_FAST_FORWARD_FACTOR,

  OPTION_TIMEOUT,
  OPTION_MAX_TIMESTEP,
  OPTION_TIMESCALE,
  OPTION_UPDATE_INTERVAL,
  OPTION_ROBOT_STARTUP_TIME,

  OPTION_CPU_START_LIMIT,
  OPTION_CPU_EXTRA,
  OPTION_CPU_PERIOD,
  OPTION_CPU_WARNING_PERCENT,

  OPTION_CHECK_INTERVAL,

  LAST_DOUBLE_OPTION
};

enum option_string_t
{
  OPTION_ROBOT_SEARCH_PATH,
  OPTION_ARENA_SEARCH_PATH,
  OPTION_TMP_RTB_DIR,
  LAST_STRING_OPTION
};

enum optiontype_t
{
  OPTIONTYPE_LONG,
  OPTIONTYPE_DOUBLE,
  OPTIONTYPE_STRING,
  OPTIONTYPE_NOTFOUND
};

struct option_return_t
{
  option_return_t( const optiontype_t d, const int n ) :
    datatype(d), option_number(n) {}

  optiontype_t datatype;
  int option_number;
};

class OptionHandler
{
public:
  OptionHandler::OptionHandler           ();
  OptionHandler::~OptionHandler          () {}

  inline long int get_l                  ( option_long_t   option ) const
    { return all_long_options[option](); }
  inline double get_d                    ( option_double_t option ) const
    { return all_double_options[option](); }
  inline string get_s                    ( option_string_t option ) const
    { return all_string_options[option](); }

  void broadcast_opts                    ();
  void log_all_options                   ();

  void save_options_to_file              ( const string&, const bool ) const;
  void read_options_from_rtbrc           ();
  void read_options_file                 ( const string&, const bool );

  void set_long_option                   ( const option_long_t, const long int );
  void set_double_option                 ( const option_double_t, const double );
  void set_string_option                 ( const option_string_t, const string& );

  option_return_t get_option_from_string ( const string& option_name );

private:
  void initialize_groups                 ();
  void initialize_options                ();

  string group_names[LAST_GROUP];

  LongOption   all_long_options[LAST_LONG_OPTION];
  DoubleOption all_double_options[LAST_DOUBLE_OPTION];
  StringOption all_string_options[LAST_STRING_OPTION];
};

#endif __OPTIONHANDLER__
