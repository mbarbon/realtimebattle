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

#ifndef __STRUCTS__
#define __STRUCTS__

#include <string>

enum game_mode_t { DEBUG_MODE, NORMAL_MODE, COMPETITION_MODE };

struct stat_t
{
  stat_t(int s, int g, int pos, double p, double  t, double tp) :
    sequence_nr(s), game_nr(g), position(pos), points(p), time_survived(t), total_points(tp) {}
  int sequence_nr;
  int game_nr;
  int position;
  double points;
  double time_survived;
  double total_points;
};

enum rotation_mode_t { NORMAL_ROT, SWEEP_LEFT, SWEEP_RIGHT, ROTATE_TO_RIGHT, ROTATE_TO_LEFT };

struct rotation_t
{
  rotation_t() : pos(0), vel(0), left(0), right(0), mode(NORMAL_ROT) {}
  rotation_t(const double p, const double v, const double l, const double r, const rotation_mode_t m) :
    pos(p), vel(v), left(l), right(r), mode(m) {}
  void set_rot(const double p, const double v, const double l, const double r, const rotation_mode_t m) 
    { pos=p; vel=v; left=l; right=r; mode=m; }
  double pos;
  double vel;
  double left;
  double right;
  rotation_mode_t mode;
};


enum state_t { NO_STATE, NOT_STARTED, STARTING_ROBOTS, GAME_IN_PROGRESS, 
               PAUSING_BETWEEN_GAMES, PAUSED, BEFORE_GAME_START,
               SHUTTING_DOWN_ROBOTS, FINISHED, EXITING };

struct start_tournament_info_t
{
  start_tournament_info_t( const start_tournament_info_t& i )
    { row = i.row; selected = i.selected;
    filename = i.filename; directory = i.directory; }
  start_tournament_info_t(const int r, const bool s,
                          const string fn, const string dir) :
    row(r), selected(s), filename(fn), directory(dir) {}
  int row;
  bool selected;
  string filename;
  string directory;
};

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

#endif __STRUCTS__
