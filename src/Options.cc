/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-1999  Erik Ouchterlony and Ragnar Ouchterlony

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

#include <fstream.h>
#include <math.h>
#include <stdlib.h>

#include "Options.h"
#include "Arena_RealTime.h"
#include "Arena_Controller.h"
#include "OptionsWindow.h"
#include "Messagetypes.h"
#include "Various.h"

//extern class Arena_RealTime the_arena;

Options::Options()
{
  all_double_options[OPTION_GRAV_CONST] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ENVIRONMENT, 9.82, 0.2, 20.0, 12,
                          false, true, "Gravitational Constant" );

  all_double_options[OPTION_AIR_RESISTANCE] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ENVIRONMENT, 0.005, 0.0, 1.0, 12,
                          false, true, "Air resistance" );

  all_double_options[OPTION_ROLL_FRICTION] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ENVIRONMENT, 0.002, 0.0, 1.0, 12,
                          false, true, "Roll friction" );

  all_double_options[OPTION_SLIDE_FRICTION] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ENVIRONMENT, 0.1, 0.0, 5.0, 12,
                          false, true, "Slide/brake friction" );

  all_double_options[OPTION_ROBOT_MAX_ACCELERATION] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 2.0, 0.1, 10.0, 12,
                          true, true, "Robot max acceleration" );

  all_double_options[OPTION_ROBOT_MIN_ACCELERATION] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, -0.5, -10.0, 0.0, 12,
                          true, true, "Robot min acceleration" );
    
  all_double_options[OPTION_ROBOT_RADIUS] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 0.5, 0.1, 10.0, 12, 
                          false, true, "Robot radius" );

  all_double_options[OPTION_ROBOT_MASS] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 1.0, 0.01, 100.0, 12,
                          false, true, "Robot mass" );

  all_double_options[OPTION_ROBOT_BOUNCE_COEFF] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 0.7, 0.0, 1.0, 12,
                          false, true, "Robot bounce coefficient" );

  all_double_options[OPTION_ROBOT_HARDNESS] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 0.5, 0.0, 1.0, 12,
                          false, true, "Robot hardness coefficient" );

  all_double_options[OPTION_ROBOT_PROTECTION] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 0.5, 0.0, 1.0, 12,
                          false, true, "Robot protection coefficient" );

  all_double_options[OPTION_ROBOT_FRONTSIZE] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, M_PI/3.0, 0.0, M_PI*2.0, 12,
                          false, true, "Robot frontsize [radians]" );

  all_double_options[OPTION_ROBOT_FRONT_BOUNCE_COEFF] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 0.7, 0.0, 1.0, 12,
                          false, true, "Robot front bounce coefficient" );

  all_double_options[OPTION_ROBOT_FRONT_HARDNESS] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 0.9, 0.0, 1.0, 12,
                          false, true, "Robot front hardness coefficient" );

  all_double_options[OPTION_ROBOT_FRONT_PROTECTION] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 0.9, 0.0, 1.0, 12,
                          false, true, "Robot front protection coefficient" );

  all_double_options[OPTION_ROBOT_START_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 100.0, 0.01, 10000.0, 12,
                          true, true, "Robot start energy" );

  all_double_options[OPTION_ROBOT_MAX_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 120.0, 0.01, 10000.0, 12,
                          true, true, "Robot max energy" );

  all_double_options[OPTION_ROBOT_MAX_ROTATE] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 2.0*M_PI / 8.0, 2.0*M_PI / 50.0,
                          2.0*M_PI * 5.0, 12,
                          true, true, "Robot max rotate speed [rad/s]" );

  all_double_options[OPTION_ROBOT_CANNON_MAX_ROTATE] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 2.0*M_PI / 4.0, 2.0*M_PI / 50.0,
                          2.0*M_PI * 5.0, 12,
                          true, true, "Robot cannon max rotate speed [rad/s]" );

  all_double_options[OPTION_ROBOT_RADAR_MAX_ROTATE] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_ROBOT, 2.0*M_PI / 3.0, 2.0*M_PI / 50.0,
                          2.0*M_PI * 5.0, 12,
                          true, true, "Robot radar max rotate speed [rad/s]" );

  all_double_options[OPTION_CHECK_INTERVAL] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 1.0, 0.01, 1000000, 12,
                          false, false, "Process check interval" );
    
  all_long_options[OPTION_ROBOT_ENERGY_LEVELS] = 
    option_info_t<long>(ENTRY_INT, PAGE_ROBOT, 10, 1, 100, 4,
                        true, true, "Robot energy levels" );

  all_double_options[OPTION_SHOT_RADIUS] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_SHOT, 0.1, 0.01, 1.0, 12,
                          false, true, "Shot radius" );

  all_double_options[OPTION_SHOT_SPEED] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_SHOT, 10.0, 0.1, 100.0, 12,
                          true, true, "Shot speed" );

  all_double_options[OPTION_SHOOTING_PENALTY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_SHOT, 0.075, 0.0, 1.0, 12,
                          false, true, "Shooting penalty (times shot energy)" );

  all_double_options[OPTION_SHOT_MIN_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_SHOT, 0.5, 0.0, 100.0, 12,
                          true, true, "Shot min energy" );

  all_double_options[OPTION_SHOT_MAX_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_SHOT, 30.0, 0.0, 100000000.0, 12,
                          true, true, "Shot max energy" );

  all_double_options[OPTION_SHOT_ENERGY_INCREASE_SPEED] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_SHOT, 10.0, 0.0, 100000000.0, 12,
                          true, true, "Shot energy increase speed [energy/s]" );

  all_long_options[OPTION_BACKGROUND_COLOUR] = 
    option_info_t<long>(ENTRY_HEX, PAGE_MISC, 0xfaf0e6, 0x000000, 0xffffff, 6,
                        false, false, "Background colour" );

  all_long_options[OPTION_FOREGROUND_COLOUR] = 
    option_info_t<long>(ENTRY_HEX, PAGE_MISC, 0x000000, 0x000000, 0xffffff, 6,
                        false, false, "Foreground colour" );

  all_double_options[OPTION_COOKIE_MAX_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 15.0, 0.0, 100000000.0, 12,
                          false, true, "Cookie max energy" );

  all_double_options[OPTION_COOKIE_MIN_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 10.0, 0.0, 100000000.0, 12,
                          false, true, "Cookie min energy" );

  all_double_options[OPTION_COOKIE_FREQUENCY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 0.03, 0.0, 100000000.0, 12,
                          false, true, "Cookie frequency [cookies per second]" );

  all_double_options[OPTION_COOKIE_RADIUS] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 0.3, 0.01, 1.0, 12,
                          false, true, "Cookie radius" );

  all_long_options[OPTION_COOKIE_COLOUR] = 
    option_info_t<long>(ENTRY_HEX, PAGE_EXTRAS, 0x35d715, 0x000000, 0xffffff, 6,
                        false, false, "Cookie colour" );

  all_double_options[OPTION_MINE_MAX_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 25.0, 0.0, 100000000.0, 12,
                          false, true, "Mine max energy" );

  all_double_options[OPTION_MINE_MIN_ENERGY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 15.0, 0.0, 100000000.0, 12,
                          false, true, "Mine min energy" );

  all_double_options[OPTION_MINE_FREQUENCY] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 0.06, 0.0, 100000000.0, 12,
                          false, true, "Mine frequency [mines per second]" );

  all_double_options[OPTION_MINE_RADIUS] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_EXTRAS, 0.3, 0.01, 1.0, 12,
                          false, true, "Mine radius" );

  all_long_options[OPTION_MINE_COLOUR] = 
    option_info_t<long>(ENTRY_HEX, PAGE_EXTRAS, 0xff0000, 0x000000, 0xffffff, 6,
                        false, false, "Mine colour" );

  all_double_options[OPTION_ARENA_SCALE] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_MISC, 1.0, 0.001, 1000, 12,
                          false, true, "Arena scale" );

  all_double_options[OPTION_TIMEOUT] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 120.0, 1.0, 100000000.0, 12,
                          false, true, "Timeout [s]" );

  all_double_options[OPTION_MAX_TIMESTEP] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 0.1, 0.001, 1.0, 12,
                          false, false, "Max timestep" );

  all_double_options[OPTION_TIMESCALE] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 1.0, 0.01, 100.0, 12,
                          false, false, "Timescale" );

  all_double_options[OPTION_CPU_START_LIMIT] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 5.0, 0.01, 100000000.0, 12,
                          false, false, "Start CPU time [s]" );

  all_double_options[OPTION_CPU_EXTRA] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 2.5, 0.01, 100000000.0, 12,
                          false, false, "Extra CPU time [s]" );

  all_double_options[OPTION_CPU_PERIOD] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 60.0, 1.0, 100000000.0, 12,
                          false, false, "Extra CPU period [s]" );

  all_double_options[OPTION_CPU_WARNING_PERCENT] = 
    option_info_t<double>(ENTRY_DOUBLE, PAGE_TIME, 0.9, 0.1, 1.0, 12,
                          false, false, "CPU time warning percentage" );

  all_string_options[OPTION_ROBOT_SEARCH_PATH] =
    option_info_t<String>(ENTRY_CHAR, PAGE_MISC, "", "", "", 1000,
                          false, false, "Robot search path" );

  all_string_options[OPTION_ARENA_SEARCH_PATH] =
    option_info_t<String>(ENTRY_CHAR, PAGE_MISC, "", "", "", 1000,
                          false, false, "Arena search path" );

  all_long_options[OPTION_ARENA_WINDOW_SIZE_X] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 350, 185, 10000, 6,
                        false, false, "Initial Arena window width" );

  all_long_options[OPTION_ARENA_WINDOW_SIZE_Y] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 350, 120, 10000, 6,
                        false, false, "Initial Arena window height" );

  all_long_options[OPTION_ARENA_WINDOW_POS_X] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 0, 0, 10000, 6,
                        false, false, "Initial Arena window x position" );

  all_long_options[OPTION_ARENA_WINDOW_POS_Y] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 0, 0, 10000, 6,
                        false, false, "Initial Arena window y position" );

  all_long_options[OPTION_CONTROL_WINDOW_POS_X] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 0, 0, 10000, 6,
                        false, false, "Initial Control window x position" );

  all_long_options[OPTION_CONTROL_WINDOW_POS_Y] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 0, 0, 10000, 6,
                        false, false, "Initial Control window y position" );

  all_long_options[OPTION_MESSAGE_WINDOW_SIZE_X] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 394, 300, 10000, 6,
                        false, false, "Initial Message window width" );

  all_long_options[OPTION_MESSAGE_WINDOW_SIZE_Y] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 210, 110, 10000, 6,
                        false, false, "Initial Message window height" );

  all_long_options[OPTION_MESSAGE_WINDOW_POS_X] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 0, 0, 10000, 6,
                        false, false, "Initial Message window x position" );

  all_long_options[OPTION_MESSAGE_WINDOW_POS_Y] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 0, 0, 10000, 6,
                        false, false, "Initial Message window y position" );

  all_long_options[OPTION_SCORE_WINDOW_SIZE_X] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 394, 175, 10000, 6,
                        false, false, "Initial Score window width" );

  all_long_options[OPTION_SCORE_WINDOW_SIZE_Y] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 374, 80, 10000, 6,
                        false, false, "Initial Score window height" );

  all_long_options[OPTION_SCORE_WINDOW_POS_X] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 0, 0, 10000, 6,
                        false, false, "Initial Score window x position" );

  all_long_options[OPTION_SCORE_WINDOW_POS_Y] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 0, 0, 10000, 6,
                        false, false, "Initial Score window y position" );

  all_long_options[OPTION_STATISTICS_WINDOW_SIZE_X] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 499, 320, 10000, 6,
                        false, false, "Initial Statistics window width" );

  all_long_options[OPTION_STATISTICS_WINDOW_SIZE_Y] = 
    option_info_t<long>(ENTRY_INT, PAGE_SIZE_OF_WINDOWS, 428, 130, 10000, 6,
                        false, false, "Initial Statistics window height" );

#ifndef NO_GRAPHICS
  optionswindow_p = NULL;
#endif
}

void
Options::broadcast_opts()
{
  realtime_arena.broadcast( GAME_OPTION, ROBOT_MAX_ROTATE, get_d(OPTION_ROBOT_MAX_ROTATE));
  realtime_arena.broadcast( GAME_OPTION, ROBOT_CANNON_MAX_ROTATE, get_d(OPTION_ROBOT_CANNON_MAX_ROTATE));
  realtime_arena.broadcast( GAME_OPTION, ROBOT_RADAR_MAX_ROTATE, get_d(OPTION_ROBOT_RADAR_MAX_ROTATE));

  realtime_arena.broadcast( GAME_OPTION, ROBOT_MAX_ACCELERATION, get_d(OPTION_ROBOT_MAX_ACCELERATION));
  realtime_arena.broadcast( GAME_OPTION, ROBOT_MIN_ACCELERATION, get_d(OPTION_ROBOT_MIN_ACCELERATION));

  realtime_arena.broadcast( GAME_OPTION, ROBOT_START_ENERGY, get_d(OPTION_ROBOT_START_ENERGY));
  realtime_arena.broadcast( GAME_OPTION, ROBOT_MAX_ENERGY, get_d(OPTION_ROBOT_MAX_ENERGY));
  realtime_arena.broadcast( GAME_OPTION, ROBOT_ENERGY_LEVELS, (double)get_l(OPTION_ROBOT_ENERGY_LEVELS));

  realtime_arena.broadcast( GAME_OPTION, SHOT_SPEED, get_d(OPTION_SHOT_SPEED));
  realtime_arena.broadcast( GAME_OPTION, SHOT_MIN_ENERGY, get_d(OPTION_SHOT_MIN_ENERGY));
  realtime_arena.broadcast( GAME_OPTION, SHOT_MAX_ENERGY, get_d(OPTION_SHOT_MAX_ENERGY));
  realtime_arena.broadcast( GAME_OPTION, SHOT_ENERGY_INCREASE_SPEED, get_d(OPTION_SHOT_ENERGY_INCREASE_SPEED));

  realtime_arena.broadcast( GAME_OPTION, TIMEOUT, get_d(OPTION_TIMEOUT));  

  realtime_arena.broadcast( GAME_OPTION, DEBUG_LEVEL, (double)the_arena.get_debug_level());  
}

void
Options::log_all_options()
{
  for(int i=0;i<LAST_DOUBLE_OPTION;i++)
    if( all_double_options[i].log_option )
      realtime_arena.print_to_logfile( 'O', 'D', 
                                  (all_double_options[i].label + ":").chars(),
                                  all_double_options[i].value );
                        
  for(int i=0;i<LAST_LONG_OPTION;i++)
    if( all_long_options[i].log_option )
      {
        if( all_long_options[i].datatype == ENTRY_INT )
          realtime_arena.print_to_logfile( 'O', 'L',
                                      (all_long_options[i].label + ":").chars(),
                                      all_long_options[i].value );
        if( all_long_options[i].datatype == ENTRY_HEX )
          realtime_arena.print_to_logfile( 'O', 'H',
                                      (all_long_options[i].label + ":").chars(),
                                      all_long_options[i].value );
      }
  for(int i=0;i<LAST_STRING_OPTION;i++)
    if( all_string_options[i].log_option )
      realtime_arena.print_to_logfile( 'O', 'S',
                                  (all_string_options[i].label + ":").chars(),
                                  all_string_options[i].value.chars() );
}

void
Options::get_options_from_rtbrc()
{
  char* home_dir;
  if( NULL == ( home_dir = getenv("HOME") ) )
    return;

  String resource_file = String(home_dir) + "/.rtbrc";
  read_options_file(resource_file,true);
}

void
Options::read_options_file(String file_string, const bool as_default)
{
  ifstream file(file_string.chars());
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
      String option_name(buffer);
      if(option_name == "")
        break;

      for(int i=0;i<LAST_DOUBLE_OPTION;i++)
        if(option_name == all_double_options[i].label )
          {
            double option_value;
            file >> option_value;
            file.get(buffer,100,'\n');
            option_value = min( option_value,
                                all_double_options[i].max_value );
            option_value = max( option_value,
                                all_double_options[i].min_value );
            all_double_options[i].value = option_value;
            if(as_default)
              all_double_options[i].default_value = option_value;
            option_found_flag = true;
          }

      for(int i=0;i<LAST_LONG_OPTION;i++)
        if(option_name == all_long_options[i].label )
          {
            long option_value = 0;
            if( all_long_options[i].datatype == ENTRY_INT )
              file >> option_value;
            if( all_long_options[i].datatype == ENTRY_HEX )
              {
                String temp_string;
                file >> temp_string;
                while( temp_string[0] == ' ' )
                  temp_string = get_segment(temp_string,1,-1);
                option_value = str2hex(temp_string);
              }
            file.get(buffer,100,'\n');
            option_value = min( option_value,
                                all_long_options[i].max_value );
            option_value = max( option_value,
                                all_long_options[i].min_value );
            all_long_options[i].value = option_value;
            if(as_default)
              all_long_options[i].default_value = option_value;
            option_found_flag = true;
          }

      for(int i=0;i<LAST_STRING_OPTION;i++)
        if(option_name == all_string_options[i].label )
          {
            String option_value;
            file >> option_value;
            while( option_value[0] == ' ' )
              option_value = get_segment(option_value,1,-1);
            file.get(buffer,100,'\n');
            all_string_options[i].value = option_value;
            if(as_default)
              all_string_options[i].default_value = option_value;
            option_found_flag = true;
          }
      if(!option_found_flag)
        file.get(buffer,1000,'\n');        
    }
}

void
Options::save_all_options_to_file(String filename, const bool as_default)
{
  if(as_default)
    {
      char* home_dir;
      if( NULL == ( home_dir = getenv("HOME") ) )
        return;

      filename = String(home_dir) + "/.rtbrc";
    }

  ofstream file(filename.chars(), ios::out);

  if( !file )
    {
      cerr << "Couldn't open save file" << endl;
      return;
    }

#ifndef NO_GRAPHICS
  if( !no_graphics && is_optionswindow_up() )
    optionswindow_p->set_all_options();
#endif

  for(int i=0;i<LAST_DOUBLE_OPTION;i++)
    file << all_double_options[i].label << ": " << all_double_options[i].value << endl;

  for(int i=0;i<LAST_LONG_OPTION;i++)
    {
      if(all_long_options[i].datatype == ENTRY_INT)
        file << all_long_options[i].label << ": " << all_long_options[i].value << endl;
      if(all_long_options[i].datatype == ENTRY_HEX)
        file << all_long_options[i].label << ": " << hex2str(all_long_options[i].value) << endl;
    }

  for(int i=0;i<LAST_STRING_OPTION;i++)
    file << all_string_options[i].label << ": " << all_string_options[i].value << endl;
}

#ifndef NO_GRAPHICS

bool
Options::is_optionswindow_up()
{
  if( NULL == optionswindow_p )
    return false;

  return true;
}

void
Options::open_optionswindow()
{
  if((the_arena.get_game_mode() == Arena_Base::COMPETITION_MODE &&
      (the_arena.get_state() == NO_STATE ||
       the_arena.get_state() == NOT_STARTED ||
       the_arena.get_state() == FINISHED)) ||
     (the_arena.get_game_mode() != Arena_Base::COMPETITION_MODE))
    if( NULL == optionswindow_p )
      optionswindow_p = 
        new OptionsWindow( -1, -1, -1, -1 );
}

void
Options::close_optionswindow()
{
  if( NULL != optionswindow_p )
    {
      delete optionswindow_p;
      optionswindow_p = NULL;
    }
}

#endif NO_GRAPHICS
