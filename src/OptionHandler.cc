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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif

#include <math.h>
#include <string>
#include <vector>

#include "IntlDefs.h"
#include "OptionHandler.h"
#include "Option.h"
#include "ArenaRealTime.h"
#include "ArenaController.h"
#include "String.h"

OptionHandler::OptionHandler()
{
#ifdef HAVE_LOCALE_H
  setlocale( LC_MESSAGES, "" );
  setlocale( LC_NUMERIC, "POSIX" );
#endif
#ifdef RTB_LOCALEDIR
  bindtextdomain( "RealTimeBattle", RTB_LOCALEDIR );
#else
  bindtextdomain( "RealTimeBattle", "/usr/local/share/locale" );
#endif
  textdomain( "RealTimeBattle" );

  initialize_groups();
  initialize_options();
}

void
OptionHandler::initialize_groups()
{
  group_names[GROUP_ENVIRONMENT] = _("Environment");
  group_names[GROUP_ROBOT] = _("Robot");
  group_names[GROUP_SHOT] = _("Shot");
  group_names[GROUP_EXTRAS] = _("Extras");
  group_names[GROUP_TIME] = _("Time");
  group_names[GROUP_SIZE_OF_WINDOWS] = _("Window sizes");
  group_names[GROUP_MISC] = _("Misc");
}

void
OptionHandler::initialize_options()
{
  // ---------- Environment ----------

  all_double_options[OPTION_GRAV_CONST] =
    DoubleOption( GROUP_ENVIRONMENT, 9.82, 0.2, 20.0, false, true,
                  "Gravitational Constant", _("Gravitational Constant") );

  all_double_options[OPTION_AIR_RESISTANCE] = 
    DoubleOption( GROUP_ENVIRONMENT, 0.005, 0.0, 1.0, false, true,
                  "Air resistance", _("Air resistance") );

  all_double_options[OPTION_ROLL_FRICTION] = 
    DoubleOption( GROUP_ENVIRONMENT, 0.002, 0.0, 1.0, false, true,
                  "Roll friction", _("Roll friction") );

  all_double_options[OPTION_SLIDE_FRICTION] = 
    DoubleOption( GROUP_ENVIRONMENT, 0.1, 0.0, 5.0, false, true,
                  "Slide/brake friction", _("Slide/brake friction") );

  // ---------- Robot ----------

  all_double_options[OPTION_ROBOT_MAX_ACCELERATION] = 
    DoubleOption( GROUP_ROBOT, 2.0, 0.1, 10.0, true, true,
                  "Robot max acceleration", _("Robot max acceleration") );

  all_double_options[OPTION_ROBOT_MIN_ACCELERATION] = 
    DoubleOption( GROUP_ROBOT, -0.5, -10.0, 0.0, true, true,
                  "Robot min acceleration", _("Robot min acceleration") );
    
  all_double_options[OPTION_ROBOT_RADIUS] = 
    DoubleOption( GROUP_ROBOT, 0.5, 0.1, 10.0, false, true,
                  "Robot radius", _("Robot radius") );

  all_double_options[OPTION_ROBOT_MASS] = 
    DoubleOption( GROUP_ROBOT, 1.0, 0.01, 100.0, false, true,
                  "Robot mass", _("Robot mass") );

  all_double_options[OPTION_ROBOT_BOUNCE_COEFF] = 
    DoubleOption( GROUP_ROBOT, 0.7, 0.0, 1.0, false, true,
                  "Robot bounce coefficient", _("Robot bounce coefficient") );

  all_double_options[OPTION_ROBOT_HARDNESS] = 
    DoubleOption( GROUP_ROBOT, 0.5, 0.0, 1.0, false, true,
                  "Robot hardness coefficient", _("Robot hardness coefficient") );

  all_double_options[OPTION_ROBOT_PROTECTION] = 
    DoubleOption( GROUP_ROBOT, 0.5, 0.0, 1.0, false, true,
                  "Robot protection coefficient", _("Robot protection coefficient") );

  all_double_options[OPTION_ROBOT_FRONTSIZE] = 
    DoubleOption( GROUP_ROBOT, M_PI/3.0, 0.0, M_PI*2.0, false, true,
                  "Robot frontsize [radians]", _("Robot frontsize [radians]") );

  all_double_options[OPTION_ROBOT_FRONT_BOUNCE_COEFF] = 
    DoubleOption( GROUP_ROBOT, 0.7, 0.0, 1.0, false, true,
                  "Robot front bounce coefficient",
                  _("Robot front bounce coefficient") );

  all_double_options[OPTION_ROBOT_FRONT_HARDNESS] = 
    DoubleOption( GROUP_ROBOT, 0.9, 0.0, 1.0, false, true,
                  "Robot front hardness coefficient",
                  _("Robot front hardness coefficient") );

  all_double_options[OPTION_ROBOT_FRONT_PROTECTION] = 
    DoubleOption( GROUP_ROBOT, 0.9, 0.0, 1.0, false, true,
                  "Robot front protection coefficient",
                  _("Robot front protection coefficient") );

  all_double_options[OPTION_ROBOT_START_ENERGY] = 
    DoubleOption( GROUP_ROBOT, 100.0, 0.01, 10000.0, true, true,
                  "Robot start energy", _("Robot start energy") );

  all_double_options[OPTION_ROBOT_MAX_ENERGY] = 
    DoubleOption( GROUP_ROBOT, 120.0, 0.01, 10000.0, true, true,
                  "Robot max energy", _("Robot max energy") );

  all_double_options[OPTION_ROBOT_MAX_ROTATE] = 
    DoubleOption( GROUP_ROBOT, 2.0*M_PI / 8.0, 2.0*M_PI / 50.0, 2.0*M_PI * 5.0,
                          true, true, "Robot max rotate speed [rad/s]",
                          _("Robot max rotate speed [rad/s]") );

  all_double_options[OPTION_ROBOT_CANNON_MAX_ROTATE] = 
    DoubleOption( GROUP_ROBOT, 2.0*M_PI / 4.0, 2.0*M_PI / 50.0, 2.0*M_PI * 5.0,
                          true, true, "Robot cannon max rotate speed [rad/s]",
                          _("Robot cannon max rotate speed [rad/s]") );

  all_double_options[OPTION_ROBOT_RADAR_MAX_ROTATE] = 
    DoubleOption( GROUP_ROBOT, 2.0*M_PI / 3.0, 2.0*M_PI / 50.0, 2.0*M_PI * 5.0,
                          true, true, "Robot radar max rotate speed [rad/s]",
                          _("Robot radar max rotate speed [rad/s]") );

  all_long_options[OPTION_ROBOT_ENERGY_LEVELS] = 
    LongOption( GROUP_ROBOT, 10, 1, 100, true, true,
                "Robot energy levels", _("Robot energy levels") );

  // ---------- Shot ----------

  all_double_options[OPTION_SHOT_RADIUS] = 
    DoubleOption( GROUP_SHOT, 0.1, 0.01, 1.0, false, true,
                  "Shot radius", _("Shot radius") );

  all_double_options[OPTION_SHOT_SPEED] = 
    DoubleOption( GROUP_SHOT, 10.0, 0.1, 100.0, true, true,
                  "Shot speed", _("Shot speed") );

  all_double_options[OPTION_SHOOTING_PENALTY] = 
    DoubleOption( GROUP_SHOT, 0.075, 0.0, 1.0, false, true,
                  "Shooting penalty (times shot energy)",
                  _("Shooting penalty (times shot energy)") );

  all_double_options[OPTION_SHOT_MIN_ENERGY] = 
    DoubleOption( GROUP_SHOT, 0.5, 0.0, 100.0, true, true,
                  "Shot min energy", _("Shot min energy") );

  all_double_options[OPTION_SHOT_MAX_ENERGY] = 
    DoubleOption( GROUP_SHOT, 30.0, 0.0, 100000000.0, true, true,
                  "Shot max energy", _("Shot max energy") );

  all_double_options[OPTION_SHOT_ENERGY_INCREASE_SPEED] = 
    DoubleOption( GROUP_SHOT, 10.0, 0.0, 100000000.0, true, true,
                  "Shot energy increase speed [energy/s]",
                  _("Shot energy increase speed [energy/s]") );

  // ---------- Extras ----------

  all_double_options[OPTION_COOKIE_MAX_ENERGY] = 
    DoubleOption( GROUP_EXTRAS, 15.0, 0.0, 100000000.0, false, true,
                  "Cookie max energy", _("Cookie max energy") );

  all_double_options[OPTION_COOKIE_MIN_ENERGY] = 
    DoubleOption( GROUP_EXTRAS, 10.0, 0.0, 100000000.0, false, true,
                  "Cookie min energy", _("Cookie min energy") );

  all_double_options[OPTION_COOKIE_FREQUENCY] = 
    DoubleOption( GROUP_EXTRAS, 0.03, 0.0, 100000000.0, false, true,
                  "Cookie frequency [cookies per second]",
                  _("Cookie frequency [cookies per second]") );

  all_double_options[OPTION_COOKIE_RADIUS] = 
    DoubleOption( GROUP_EXTRAS, 0.3, 0.01, 1.0, false, true,
                  "Cookie radius", _("Cookie radius") );

  all_long_options[OPTION_COOKIE_COLOUR] = 
    LongOption( GROUP_EXTRAS, 0x35d715, 0x000000, 0xffffff, false, false,
                "Cookie colour", _("Cookie colour"), true );

  all_double_options[OPTION_MINE_MAX_ENERGY] = 
    DoubleOption( GROUP_EXTRAS, 25.0, 0.0, 100000000.0, false, true,
                  "Mine max energy", _("Mine max energy") );

  all_double_options[OPTION_MINE_MIN_ENERGY] = 
    DoubleOption( GROUP_EXTRAS, 15.0, 0.0, 100000000.0, false, true,
                  "Mine min energy", _("Mine min energy") );

  all_double_options[OPTION_MINE_FREQUENCY] = 
    DoubleOption( GROUP_EXTRAS, 0.06, 0.0, 100000000.0, false, true,
                  "Mine frequency [mines per second]",
                  _("Mine frequency [mines per second]") );

  all_double_options[OPTION_MINE_RADIUS] = 
    DoubleOption( GROUP_EXTRAS, 0.3, 0.01, 1.0, false, true,
                  "Mine radius", _("Mine radius") );

  all_long_options[OPTION_MINE_COLOUR] = 
    LongOption( GROUP_EXTRAS, 0xff0000, 0x000000, 0xffffff, false, false,
                "Mine colour", _("Mine colour"), true );

  // ---------- Time ----------

  all_double_options[OPTION_CHECK_INTERVAL] = 
    DoubleOption( GROUP_TIME, 1.0, 0.01, 1000000, false, false,
                  "Process check interval", _("Process check interval") );
    
  all_double_options[OPTION_TIMEOUT] = 
    DoubleOption( GROUP_TIME, 120.0, 1.0, 100000000.0, false, true,
                  "Timeout [s]", _("Timeout [s]") );

  all_double_options[OPTION_MAX_TIMESTEP] = 
    DoubleOption( GROUP_TIME, 0.1, 0.001, 1.0, false, false,
                  "Max timestep", _("Max timestep") );

  all_double_options[OPTION_TIMESCALE] = 
    DoubleOption( GROUP_TIME, 1.0, 0.01, 100.0, false, false,
                  "Timescale", _("Timescale") );

  all_double_options[OPTION_UPDATE_INTERVAL] = 
    DoubleOption( GROUP_TIME, 0.050, 0.010, 1.0, false, false,
                  "Update interval [s]", _("Update interval [s]") );

  all_double_options[OPTION_ROBOT_STARTUP_TIME] = 
    DoubleOption( GROUP_TIME, 1.0, 0.050, 1000000.0, false, false,
                  "Robot startup time [s]", _("Robot startup time [s]") );

  all_double_options[OPTION_CPU_START_LIMIT] = 
    DoubleOption( GROUP_TIME, 5.0, 0.01, 100000000.0, false, false,
                  "Start CPU time [s]", _("Start CPU time [s]") );

  all_double_options[OPTION_CPU_EXTRA] = 
    DoubleOption( GROUP_TIME, 2.5, 0.01, 100000000.0, false, false,
                  "Extra CPU time [s]", _("Extra CPU time [s]") );

  all_double_options[OPTION_CPU_PERIOD] = 
    DoubleOption( GROUP_TIME, 60.0, 1.0, 100000000.0, false, false,
                  "Extra CPU period [s]", _("Extra CPU period [s]") );

  all_double_options[OPTION_CPU_WARNING_PERCENT] = 
    DoubleOption( GROUP_TIME, 0.9, 0.1, 1.0, false, false,
                  "CPU time warning percentage", _("CPU time warning percentage") );

  all_long_options[OPTION_LOG_EVERY_NTH_UPDATE_INTERVAL] =
    LongOption( GROUP_TIME, 1, 1, 1000, false, false,
                "Logging frequency [Each nth update interval]",
                _("Logging frequency [Each n:th update interval]") );

  // ---------- Misc ----------

  all_long_options[OPTION_BACKGROUND_COLOUR] = 
    LongOption( GROUP_MISC, 0xfaf0e6, 0x000000, 0xffffff, false, false,
                "Background colour", _("Background colour"), true );

  all_long_options[OPTION_FOREGROUND_COLOUR] = 
    LongOption( GROUP_MISC, 0x000000, 0x000000, 0xffffff, false, false,
                "Foreground colour", _("Foreground colour"), true );

  all_long_options[OPTION_RTB_MESSAGE_COLOUR] = 
    LongOption( GROUP_MISC, 0x1111ee, 0x000000, 0xffffff, false, false,
                "Colour for RTB messages", _("Colour for RTB messages"), true );

  all_double_options[OPTION_ARENA_SCALE] = 
    DoubleOption( GROUP_MISC, 1.0, 0.001, 1000, false, true,
                  "Arena scale", _("Arena scale") );

  all_double_options[OPTION_FAST_FORWARD_FACTOR] = 
    DoubleOption( GROUP_MISC, 5.0, 0.001, 1000, false, false,
                  "Fast forward factor", _("Fast forward factor") );

  all_long_options[OPTION_MAX_ROBOTS_ALLOWED] =
    LongOption( GROUP_MISC, 120, 1, 10000, false, false,
                "Max robots allowed simultaneously",
                _("Max robots allowed simultaneously") );

  all_string_options[OPTION_ROBOT_SEARCH_PATH] =
    StringOption( GROUP_MISC, "", 1000, false, false,
                  "Robot search path", _("Robot search path") );

  all_string_options[OPTION_ARENA_SEARCH_PATH] =
    StringOption( GROUP_MISC, "", 1000, false, false,
                  "Arena search path", _("Arena search path") );

  all_string_options[OPTION_TMP_RTB_DIR] =
    StringOption( GROUP_MISC, "/tmp/rtb", 1000, false, false,
                  "Directory for temporary files", _("Directory for temporary files") );

  // ---------- Size of Windows ----------

  all_long_options[OPTION_ARENA_WINDOW_SIZE_X] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 350, 185, 10000, false, false,
                "Initial Arena window width", _("Initial Arena window width") );

  all_long_options[OPTION_ARENA_WINDOW_SIZE_Y] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 350, 120, 10000, false, false,
                "Initial Arena window height", _("Initial Arena window height") );

  all_long_options[OPTION_ARENA_WINDOW_POS_X] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                "Initial Arena window x position",
                _("Initial Arena window x position") );

  all_long_options[OPTION_ARENA_WINDOW_POS_Y] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                "Initial Arena window y position",
                _("Initial Arena window y position") );

  all_long_options[OPTION_CONTROL_WINDOW_POS_X] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                "Initial Control window x position",
                _("Initial Control window x position") );

  all_long_options[OPTION_CONTROL_WINDOW_POS_Y] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                "Initial Control window y position",
                _("Initial Control window y position") );

  all_long_options[OPTION_MESSAGE_WINDOW_SIZE_X] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 394, 300, 10000, false, false,
                "Initial Message window width", _("Initial Message window width") );

  all_long_options[OPTION_MESSAGE_WINDOW_SIZE_Y] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 210, 110, 10000, false, false,
                "Initial Message window height", _("Initial Message window height") );

  all_long_options[OPTION_MESSAGE_WINDOW_POS_X] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                "Initial Message window x position",
                _("Initial Message window x position") );

  all_long_options[OPTION_MESSAGE_WINDOW_POS_Y] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                "Initial Message window y position",
                _("Initial Message window y position") );

  all_long_options[OPTION_SCORE_WINDOW_SIZE_X] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 394, 175, 10000, false, false,
                "Initial Score window width", _("Initial Score window width") );

  all_long_options[OPTION_SCORE_WINDOW_SIZE_Y] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 374, 80, 10000, false, false,
                "Initial Score window height", _("Initial Score window height") );

  all_long_options[OPTION_SCORE_WINDOW_POS_X] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                "Initial Score window x position",
                _("Initial Score window x position") );

  all_long_options[OPTION_SCORE_WINDOW_POS_Y] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 0, 0, 10000, false, false,
                "Initial Score window y position",
                _("Initial Score window y position") );

  all_long_options[OPTION_STATISTICS_WINDOW_SIZE_X] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 499, 320, 10000, false, false,
                "Initial Statistics window width",
                _("Initial Statistics window width") );

  all_long_options[OPTION_STATISTICS_WINDOW_SIZE_Y] = 
    LongOption( GROUP_SIZE_OF_WINDOWS, 428, 130, 10000, false, false,
                "Initial Statistics window height",
                _("Initial Statistics window height") );
}

void
OptionHandler::broadcast_opts()
{
  realtime_arena.broadcast( GAME_OPTION, ROBOT_MAX_ROTATE,
                            get_d(OPTION_ROBOT_MAX_ROTATE) );
  realtime_arena.broadcast( GAME_OPTION, ROBOT_CANNON_MAX_ROTATE,
                            get_d(OPTION_ROBOT_CANNON_MAX_ROTATE) );
  realtime_arena.broadcast( GAME_OPTION, ROBOT_RADAR_MAX_ROTATE,
                            get_d(OPTION_ROBOT_RADAR_MAX_ROTATE) );
  realtime_arena.broadcast( GAME_OPTION, ROBOT_MAX_ACCELERATION,
                            get_d(OPTION_ROBOT_MAX_ACCELERATION) );
  realtime_arena.broadcast( GAME_OPTION, ROBOT_MIN_ACCELERATION,
                            get_d(OPTION_ROBOT_MIN_ACCELERATION) );
  realtime_arena.broadcast( GAME_OPTION, ROBOT_START_ENERGY,
                            get_d(OPTION_ROBOT_START_ENERGY) );
  realtime_arena.broadcast( GAME_OPTION, ROBOT_MAX_ENERGY,
                            get_d(OPTION_ROBOT_MAX_ENERGY) );
  realtime_arena.broadcast( GAME_OPTION, ROBOT_ENERGY_LEVELS,
                            (double)get_l(OPTION_ROBOT_ENERGY_LEVELS) );
  realtime_arena.broadcast( GAME_OPTION, SHOT_SPEED,
                            get_d(OPTION_SHOT_SPEED ));
  realtime_arena.broadcast( GAME_OPTION, SHOT_MIN_ENERGY,
                            get_d(OPTION_SHOT_MIN_ENERGY ));
  realtime_arena.broadcast( GAME_OPTION, SHOT_MAX_ENERGY,
                            get_d(OPTION_SHOT_MAX_ENERGY) );
  realtime_arena.broadcast( GAME_OPTION, SHOT_ENERGY_INCREASE_SPEED,
                            get_d(OPTION_SHOT_ENERGY_INCREASE_SPEED) );
  realtime_arena.broadcast( GAME_OPTION, TIMEOUT, get_d(OPTION_TIMEOUT) );
  realtime_arena.broadcast( GAME_OPTION, DEBUG_LEVEL,
                            (double)the_arena.get_debug_level() );  
}

void
OptionHandler::log_all_options()
{
  for( int i = 0; i < LAST_LONG_OPTION; i++ )
    if( all_long_options[i].log() )
      {
        if( all_long_options[i].is_value_hexadecimal() )
          realtime_arena.print_to_logfile
            ( 'O', (int)'H', (all_long_options[i].get_rc_label() + ":").c_str(),
              all_long_options[i]() );
        else
          realtime_arena.print_to_logfile
            ( 'O', (int)'L', (all_long_options[i].get_rc_label() + ":").c_str(),
              all_long_options[i]() );
      }
  for( int i = 0; i < LAST_DOUBLE_OPTION; i++ )
    if( all_double_options[i].log() )
      realtime_arena.print_to_logfile
        ( 'O', (int)'D', (all_double_options[i].get_rc_label() + ":").c_str(),
          all_double_options[i]() );
  for( int i = 0; i < LAST_STRING_OPTION; i++ )
    if( all_string_options[i].log() )
      realtime_arena.print_to_logfile
        ( 'O', (int)'S', (all_string_options[i].get_rc_label() + ":").c_str(),
          all_string_options[i]().c_str() );
}

void
OptionHandler::read_options_from_rtbrc()
{
  char* home_dir;
  if( NULL == ( home_dir = getenv("HOME") ) )
    return;

  string resource_file = string(home_dir) + "/.rtbrc";
  read_options_file(resource_file,true);
}

void
OptionHandler::read_options_file( const string& file_string, const bool as_default )
{
  ifstream file(file_string.c_str());
  if( !file )
    return;

  for(;;)
    {
      char temp;
      char buffer[1000];
      bool option_found_flag = false;

      file >> ws;
      file.get(buffer,100,':');
      file.get(temp);
      string option_name(buffer);
      if(option_name == "")
        break;

      for( int i = 0; i < LAST_LONG_OPTION; i++ )
        if( option_name == all_long_options[i].get_rc_label() )
          {
            long option_value = 0;
            if( all_long_options[i].is_value_hexadecimal() )
              {
                string temp_string;
                file >> temp_string;
                int i = 0;
                while( temp_string[i] == ' ' ) i++;
                option_value = string2hex( temp_string.substr(i,string::npos) );
              }
            else
              file >> option_value;

            file.get(buffer,100,'\n');
            option_value = min( option_value, all_long_options[i].get_max_value() );
            option_value = max( option_value, all_long_options[i].get_min_value() );

            all_long_options[i].change_value( option_value, as_default );
            option_found_flag = true;
          }
      for( int i = 0; i < LAST_DOUBLE_OPTION; i++ )
        if( option_name == all_double_options[i].get_rc_label() )
          {
            double option_value;
            file >> option_value;
            file.get(buffer,100,'\n');
            option_value = min( option_value, all_double_options[i].get_max_value() );
            option_value = max( option_value, all_double_options[i].get_min_value() );
            all_double_options[i].change_value( option_value, as_default );
            option_found_flag = true;
          }
      for( int i = 0; i < LAST_STRING_OPTION; i++ )
        if( option_name == all_string_options[i].get_rc_label() )
          {
            string option_value;
            file >> option_value;
            int i = 0;
            while( option_value[i] == ' ' ) i++;
            file.get(buffer,100,'\n');
            all_string_options[i].change_value
              ( option_value.substr(i, string::npos), as_default );
            option_found_flag = true;
          }
      if(!option_found_flag)
        file.get(buffer,1000,'\n');        
    }
}

void
OptionHandler::save_options_to_file( const string& fname,
                                     const bool as_default ) const
{
  string filename = fname;
  if( as_default )
    {
      char* home_dir;
      if( NULL == ( home_dir = getenv("HOME") ) )
        return;

      filename = string(home_dir) + "/.rtbrc";
    }

  ofstream file(filename.c_str(), ios::out);

  if( !file )
    {
      cerr << "Couldn't open save file" << endl;
      return;
    }

  for(int i=0;i<LAST_DOUBLE_OPTION;i++)
    file << all_double_options[i].get_rc_label() << ": "
         << all_double_options[i]() << endl;

  for(int i=0;i<LAST_LONG_OPTION;i++)
    if( all_long_options[i].is_value_hexadecimal() )
      file << all_long_options[i].get_rc_label() << ": "
           << hex2string( all_long_options[i]() ) << endl;
    else
      file << all_long_options[i].get_rc_label() << ": "
           << all_long_options[i]() << endl;

  for(int i=0;i<LAST_STRING_OPTION;i++)
    file << all_string_options[i].get_rc_label() << ": "
         << all_string_options[i]() << endl;
}

void
OptionHandler::set_long_option( const option_long_t option, const long int val )
{
  long int temp_val = min( val, all_long_options[option].get_max_value() );
  temp_val = max( temp_val, all_long_options[option].get_min_value() );
  
  all_long_options[option].change_value( temp_val );
}

void
OptionHandler::set_double_option( const option_double_t option, const double val )
{
  double temp_val = min( val, all_long_options[option].get_max_value() );
  temp_val = max( temp_val, all_long_options[option].get_min_value() );
  
  all_double_options[option].change_value( temp_val );
}

void
OptionHandler::set_string_option( const option_string_t option, const string& val )
{
  all_string_options[option].change_value( val );
}

option_return_t
OptionHandler::get_option_from_string( const string& option_name )
{
  for( int i=0; i<LAST_DOUBLE_OPTION; i++ )
    if( option_name == all_double_options[i].get_rc_label() )
      return option_return_t( OPTIONTYPE_LONG, i );
  for( int i=0; i<LAST_LONG_OPTION; i++ )
    if( option_name == all_long_options[i].get_rc_label() )
      return option_return_t( OPTIONTYPE_DOUBLE, i );
  for( int i=0; i<LAST_STRING_OPTION; i++ )
    if( option_name == all_string_options[i].get_rc_label() )
      return option_return_t( OPTIONTYPE_STRING, i );

  return option_return_t( OPTIONTYPE_NOTFOUND, -1 );
}

