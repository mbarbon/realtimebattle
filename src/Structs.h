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

#ifndef __STRUCTS__
#define __STRUCTS__

#include <string>
#include <list>
#include <vector>

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
struct arena_info_t 
{
  string filename;
  string directory;
  int row;
  bool selected;

  string type;
};
struct robot_info_t
{
  robot_info_t( const robot_info_t& i )
  { 
    row = i.row; selected = i.selected;
    filename = i.filename; directory = i.directory;
    team = i.team; 
  }
  robot_info_t(const string& fn, const string& dir, int& r, bool s, int t) :
    filename(fn), directory(dir), row(r), selected(s), team(t) {}
  string filename;
  string directory;
  int row;
  bool selected;
  int team;
};
struct match_info_t
{
  arena_info_t match_arena;
  list<robot_info_t> robots;
  vector<int> team_colors;
};
struct tourn_info_t
{
  list<match_info_t> matches;
  list<match_info_t>::iterator current_match;
  bool keep_same_robot_set;
};

struct message_t
{
  message_t( const message_t& m )
    : sender(m.sender), message(m.message) {}
  message_t( const string& s, const string& m )
    : sender(s), message(m) {}
  string sender;
  string message;
};

// ---------- Environment ----------

#define OPTION_GRAV_CONST     "Gravity"
#define OPTION_AIR_RESISTANCE "Air resistance"
#define OPTION_ROLL_FRICTION  "Roll friction"
#define OPTION_SLIDE_FRICTION "Slide friction"

// ---------- Robot ----------

#define OPTION_ROBOT_MAX_ACCELERATION   "Max acceleration"
#define OPTION_ROBOT_MIN_ACCELERATION   "Min acceleration"
#define OPTION_ROBOT_RADIUS             "Robot radius"
#define OPTION_ROBOT_MASS               "Robot mass"
#define OPTION_ROBOT_BOUNCE_COEFF       "Robot Bounce coeff"
#define OPTION_ROBOT_HARDNESS           "Robot hardness"
#define OPTION_ROBOT_PROTECTION         "Robot proection"
#define OPTION_ROBOT_FRONTSIZE          "Robot frontsize"
#define OPTION_ROBOT_FRONT_BOUNCE_COEFF "Robot front bounce coeff"
#define OPTION_ROBOT_FRONT_HARDNESS     "Robot front hardness"
#define OPTION_ROBOT_FRONT_PROTECTION   "Robot front protection"
#define OPTION_ROBOT_START_HEALTH       "Start health"
#define OPTION_ROBOT_MAX_HEALTH         "Robot max health"
#define OPTION_ROBOT_MAX_ROTATE         "Robot max rotate"
#define OPTION_ROBOT_CANNON_MAX_ROTATE  "Cannon max rotate"
#define OPTION_ROBOT_RADAR_MAX_ROTATE   "Radar max rotate"
#define OPTION_ROBOT_HEALTH_LEVELS      "health_levels"

// ---------- Shot ----------

#define OPTION_SHOT_RADIUS                "Shot radius"
#define OPTION_SHOT_SPEED                 "Shot speed"
#define OPTION_SHOOTING_PENALTY           "Shooting penalty"
#define OPTION_SHOT_MIN_ENERGY            "Shot min energy"
#define OPTION_SHOT_MAX_ENERGY            "Shot max energy"
#define OPTION_SHOT_ENERGY_INCREASE_SPEED "Shot energy increase speed"

// ---------- Extras ----------

#define OPTION_COOKIE_MAX_ENERGY "Cookie max energy"
#define OPTION_COOKIE_MIN_ENERGY "Cookie min energy"
#define OPTION_COOKIE_FREQUENCY  "Cookie frequency"
#define OPTION_COOKIE_RADIUS     "Cookie radius"
#define OPTION_COOKIE_COLOUR     "Cookie colour"
#define OPTION_MINE_MAX_ENERGY   "Mine max energy"
#define OPTION_MINE_MIN_ENERGY   "Mine min energy"
#define OPTION_MINE_FREQUENCY    "Mine frequency"
#define OPTION_MINE_RADIUS       "Mine radius"
#define OPTION_MINE_COLOUR       "Mine colour"

// ---------- Time ----------

#define OPTION_CHECK_INTERVAL                "Check interval"
#define OPTION_TIMEOUT                       "Timeout"
#define OPTION_MAX_TIMESTEP                  "Max timestep"
#define OPTION_TIMESCALE                     "Timescale"
#define OPTION_UPDATE_INTERVAL               "Update interval"
#define OPTION_ROBOT_STARTUP_TIME            "Robot startup time"
#define OPTION_CPU_START_LIMIT               "CPU start limit"
#define OPTION_CPU_EXTRA                     "CPU Extra"
#define OPTION_CPU_PERIOD                    "CPU Period"
#define OPTION_CPU_WARNING_PERCENT           "CPU warning percent"
#define OPTION_LOG_EVERY_NTH_UPDATE_INTERVAL "Logging frequency"

// ---------- Misc ----------

#define OPTION_BACKGROUND_COLOUR   "Background colour"
#define OPTION_FOREGROUND_COLOUR   "Foreground colour"
#define OPTION_RTB_MESSAGE_COLOUR  "RTB message colour"
#define OPTION_ARENA_SCALE         "Arena scale"
#define OPTION_FAST_FORWARD_FACTOR "Fast forward factor"
#define OPTION_MAX_ROBOTS_ALLOWED  "max_robots" 
#define OPTION_ROBOT_SEARCH_PATH   "Robot search path"
#define OPTION_ARENA_SEARCH_PATH   "Arena search path"
#define OPTION_TMP_RTB_DIR         "RTB dir"

// ---------- Size of Windows ----------

#define OPTION_ARENA_WINDOW_SIZE_X      "Arena window xsize"
#define OPTION_ARENA_WINDOW_SIZE_Y      "Arena window ysize"
#define OPTION_ARENA_WINDOW_POS_X       "Arena window xpos"
#define OPTION_ARENA_WINDOW_POS_Y       "Arena window ypos"
#define OPTION_CONTROL_WINDOW_POS_X     "Control window xpos"
#define OPTION_CONTROL_WINDOW_POS_Y     "Control window ypos"
#define OPTION_MESSAGE_WINDOW_SIZE_X    "Message window xsize"
#define OPTION_MESSAGE_WINDOW_SIZE_Y    "Message window ysize"
#define OPTION_MESSAGE_WINDOW_POS_X     "Message window xpos"
#define OPTION_MESSAGE_WINDOW_POS_Y     "Message window ypos"
#define OPTION_SCORE_WINDOW_SIZE_X      "Score window xsize"
#define OPTION_SCORE_WINDOW_SIZE_Y      "Score window ysize"
#define OPTION_SCORE_WINDOW_POS_X       "Score window xpos"
#define OPTION_SCORE_WINDOW_POS_Y       "Score window ypos"
#define OPTION_STATISTICS_WINDOW_SIZE_X "Statistics window xsize"
#define OPTION_STATISTICS_WINDOW_SIZE_Y "Statistics window ysize"

#endif // __STRUCTS__
