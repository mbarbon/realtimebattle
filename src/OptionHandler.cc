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
#include <iostream>
#include <fstream>
#include <ctype.h>

#include "IntlDefs.h"
#include "OptionHandler.h"
#include "Option.h"
#include "ArenaRealTime.h"
#include "ArenaController.h"
#include "String.h"

// Optionarrays are deleted when OptionHandler is destructed.
OptionHandler::OptionHandler( const string& _section_name,
                              DoubleOption* double_opts, LongOption* long_opts,
                              StringOption* string_opts,
                              const int num_double, const int num_long,
                              const int num_string )
{
  section_name = _section_name;

  all_double_options = double_opts;
  all_long_options = long_opts;
  all_string_options = string_opts;

  number_of_double_options = num_double;
  number_of_long_options = num_long;
  number_of_string_options = num_string;

  initialize_groups();
  //  initialize_options();
}

OptionHandler::~OptionHandler()
{
  delete[] all_double_options;
  delete[] all_long_options;
  delete[] all_string_options;
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

//TODO: Move broadcast_opts to a better place
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
  for( int i = 0; i < number_of_long_options; i++ )
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
  for( int i = 0; i < number_of_double_options; i++ )
    if( all_double_options[i].log() )
      realtime_arena.print_to_logfile
        ( 'O', (int)'D', (all_double_options[i].get_rc_label() + ":").c_str(),
          all_double_options[i]() );
  for( int i = 0; i < number_of_string_options; i++ )
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

  //TODO: this should be .rtbrc in the released version
  // It is like this to not overwrite the old .rtbrc
  string resource_file = string(home_dir) + "/.rtbrc";
  read_options_file(resource_file,true);
}

void
OptionHandler::set_long_option( const int option, const long int val )
{
  long int temp_val = min( val, all_long_options[option].get_max_value() );
  temp_val = max( temp_val, all_long_options[option].get_min_value() );
  
  all_long_options[option].change_value( temp_val );
}

void
OptionHandler::set_double_option( const int option, const double val )
{
  double temp_val = min( val, all_long_options[option].get_max_value() );
  temp_val = max( temp_val, all_long_options[option].get_min_value() );
  
  all_double_options[option].change_value( temp_val );
}

void
OptionHandler::set_string_option( const int option, const string& val )
{
  all_string_options[option].change_value( val );
}

option_return_t
OptionHandler::get_option_from_string( const string& option_name )
{
  for( int i=0; i<number_of_double_options; i++ )
    if( option_name == all_double_options[i].get_rc_label() )
      return option_return_t( OPTIONTYPE_LONG, i );
  for( int i=0; i<number_of_long_options; i++ )
    if( option_name == all_long_options[i].get_rc_label() )
      return option_return_t( OPTIONTYPE_DOUBLE, i );
  for( int i=0; i<number_of_string_options; i++ )
    if( option_name == all_string_options[i].get_rc_label() )
      return option_return_t( OPTIONTYPE_STRING, i );

  return option_return_t( OPTIONTYPE_NOTFOUND, -1 );
}

bool
OptionHandler::load_file_into_string( string& str, const string& filename ) const
{
  char buffer[400];

  ifstream file( filename.c_str() );
  if( !file )
    return false;

  str = "";

  while( !file.eof() )
    {
      file.get( buffer, 400, '\n' );
      file.get();
      str += string(buffer) + '\n';
    }

  return true;
}

bool
OptionHandler::dump_string_to_file( string& str, const string& filename ) const
{
  ofstream file( filename.c_str() );
  if( !file )
    return false;

  file << str;
  return true;
}

// This funciton searches for the correct section.
// If found: returns position at the start of the seciton.
// If not found: may add the section, if not returns the last position in strfile.
bool
OptionHandler::locate_section_in_file( string& strfile, const string& section,
                                       string::size_type& return_pos,
                                       const bool add_section_if_not_found ) const
{
  string::size_type pos = 0, findpos = 0;
  bool continue_loop = true;

  while( continue_loop )
    {
      string tempstr;
      if( ( findpos = strfile.find_first_of( '\n', pos ) ) != string::npos )
        {
          tempstr = strfile.substr( pos, findpos - pos - 1 );
          continue_loop = true;
        }
      else
        {
          tempstr = strfile.substr( pos, string::npos );
          continue_loop = false;
        }

      if( tempstr[0] == '<' )
        if( tempstr.length() > section.length()  &&
            equal_strings_nocase( tempstr.substr( 1, section.length() ), section ) )
          {
            return_pos = findpos+1;
            return true;
          }
      pos = findpos+1;
    }

  if( add_section_if_not_found )
    strfile += "\n<" + section + ">\n";

  return_pos = strfile.length();
  return false;
}


// This function assumes that the supplied section is at the
// start of the right section.
//
// The function does sets the iterator at the last position in the section
// if it returns false and to the right after the label and the first = on the line
// of the found option if it returns true.
bool
OptionHandler::locate_option_in_file( const string& strfile,
                                      const string::size_type section_pos,
                                      const string& option_label,
                                      string::size_type& return_pos ) const
{
  string::size_type pos = section_pos, findpos = 0;
  bool continue_loop = true;

  while( continue_loop )
    {
      string tempstr;
      if( ( findpos = strfile.find_first_of( '\n', pos ) ) != string::npos )
        {
          tempstr = strfile.substr( pos, findpos - pos - 1 );
          continue_loop = true;
        }
      else
        {
          tempstr = strfile.substr( pos, string::npos );
          continue_loop = false;
        }

      while( isspace( tempstr[0] ) )
        tempstr = tempstr.substr( 1, string::npos );

      if( tempstr.length() > 0 )
        switch( tempstr[0] )
          {
          case '<': // New section, return false!
            return_pos = pos+1;
            return false;
            break;
          case ';': // Comment, skip!
            break;
          default: // Check if we have the right option here!
            if( tempstr.length() > option_label.length() &&
                equal_strings_nocase( tempstr.substr( 0, option_label.length() ),
                                      option_label ) )
              {
                string::size_type equal_sign_pos=
                  strfile.find_first_of( '=', pos + option_label.length() );
                if( equal_sign_pos != string::npos )
                  {
                    return_pos = equal_sign_pos+1;
                    return true;
                  }
                else
                  break;
              }
            break;
          }
      pos = findpos+1;
    }

  return_pos = strfile.length();
  return false;
}

void
OptionHandler::read_option_from_file( string& strfile, string::size_type& section_pos,
                                      DoubleOption option, const bool as_default )
{
  string::size_type pos = 0;
  if( locate_option_in_file( strfile, section_pos, option.get_rc_label(), pos ) )
    {
      string tempstr = strfile.substr( pos, strfile.find_first_of( '\n', pos ) - pos );
      while( tempstr.length() > 0 && isspace( tempstr[0] ) )
        tempstr = tempstr.substr( 1, string::npos );
      if( tempstr.length() > 0 )
        option.change_value( string2double( tempstr ), as_default );
    }
}

void
OptionHandler::read_option_from_file( string& strfile, string::size_type& section_pos,
                                      LongOption option, const bool as_default )
{
  string::size_type pos = 0;
  if( locate_option_in_file( strfile, section_pos, option.get_rc_label(), pos ) )
    {
      string tempstr = strfile.substr( pos, strfile.find_first_of( '\n', pos ) - pos );
      while( tempstr.length() > 0 && isspace( tempstr[0] ) )
        tempstr = tempstr.substr( 1, string::npos );
      if( tempstr.length() > 0 )
        {
          if( option.is_value_hexadecimal() )
            option.change_value( string2hex( tempstr ), as_default );
          else
            option.change_value( string2lint( tempstr ), as_default );
        }
    }
}

void
OptionHandler::read_option_from_file( string& strfile, string::size_type& section_pos,
                                      StringOption option, const bool as_default )
{
  string::size_type pos = 0;
  if( locate_option_in_file( strfile, section_pos, option.get_rc_label(), pos ) )
    {
      string tempstr = strfile.substr( pos, strfile.find_first_of( '\n', pos ) - pos );
      while( tempstr.length() > 0 && isspace( tempstr[0] ) )
        tempstr = tempstr.substr( 1, string::npos );
      if( tempstr.length() > 0 )
        option.change_value( tempstr, as_default );
    }
}

void
OptionHandler::save_option_to_file( string& strfile, string::size_type& section_pos,
                                    const DoubleOption option ) const
{
  string::size_type pos = 0;
  if( locate_option_in_file( strfile, section_pos, option.get_rc_label(), pos ) )
    strfile.replace( pos, strfile.find_first_of( '\n', pos ) - pos,
                     ' ' + double2string(option()) );
  else
    {
      strfile += "\n;" + option.get_description() + "\n"
        + option.get_rc_label() + " = " + double2string(option()) + "\n";
    }
}

void
OptionHandler::save_option_to_file( string& strfile, string::size_type& section_pos,
                                    const LongOption option ) const
{
  string::size_type pos = 0;
  string optstr;
  if( option.is_value_hexadecimal() )
    optstr = hex2string( option() );
  else
    optstr = lint2string( option() );

  if( locate_option_in_file( strfile, section_pos, option.get_rc_label(), pos ) )
    strfile.replace( pos, strfile.find_first_of( '\n', pos ) - pos, ' ' + optstr );
  else
    {
      strfile += "\n;" + option.get_description() + "\n"
        + option.get_rc_label() + " = " + optstr + "\n";
    }
}

void
OptionHandler::save_option_to_file( string& strfile, string::size_type& section_pos,
                                    const StringOption option ) const
{
  string::size_type pos = 0;
  if( locate_option_in_file( strfile, section_pos, option.get_rc_label(), pos ) )
    strfile.replace( pos, strfile.find_first_of( '\n', pos ) - pos, ' ' + option() );
  else
    {
      strfile += "\n;" + option.get_description() + "\n"
        + option.get_rc_label() + " = " + option() + "\n";
    }
}

void
OptionHandler::read_options_file( const string& file_string, const bool as_default )
{
  string option_string = "";
  if( !load_file_into_string( option_string, file_string ) )
    cerr << "Couldn't open options file for input." << endl;

  string::size_type section_pos = 0;
  locate_section_in_file( option_string, section_name, section_pos, false );

  for( int i=0; i<number_of_double_options; i++ )
    read_option_from_file( option_string, section_pos,
                           all_double_options[i], as_default );

  for( int i=0; i<number_of_long_options; i++ )
    read_option_from_file( option_string, section_pos,
                           all_long_options[i], as_default );

  for( int i=0; i<number_of_string_options; i++ )
    read_option_from_file( option_string, section_pos,
                           all_string_options[i], as_default );
}

void
OptionHandler::save_options_to_file( const string& fname, const bool as_default ) const
{
  string filename = fname;
  if( as_default )
    {
      char* home_dir;
      if( NULL == ( home_dir = getenv("HOME") ) )
        return;

      //TODO: this should be .rtbrc in the released version
      // It is like this to not overwrite the old .rtbrc
      filename = string(home_dir) + "/.rtbrc_v2";
    }

  string option_string = "";
  if( !load_file_into_string( option_string, filename ) )
    option_string = "";

  string::size_type section_pos = 0;
  locate_section_in_file( option_string, section_name, section_pos, true );

  for(int i=0;i<number_of_double_options;i++)
    save_option_to_file( option_string, section_pos, all_double_options[i] );

  for(int i=0;i<number_of_long_options;i++)
    save_option_to_file( option_string, section_pos, all_long_options[i] );

  for(int i=0;i<number_of_string_options;i++)
    save_option_to_file( option_string, section_pos, all_string_options[i] );

  if( !dump_string_to_file( option_string, filename ) )
    {
      cerr << "Couldn't open save file for output" << endl;
      return;
    }
}
