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
#include <map>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <assert.h>

#include "OptionHandler.h"
#include "Option.h"
#include "String.h"

// Optionarrays are deleted when OptionHandler is destructed.
OptionHandler::OptionHandler( const string& _section_name,
                              map<string,Option*>& opts,
                              vector<string>& _group_names )
  : section_name(_section_name), all_options(opts), group_names(_group_names)
{
}

OptionHandler::~OptionHandler()
{
  map<string,Option*>::iterator mi;
  for( mi = all_options.begin(); mi != all_options.end(); mi++ )
    delete (mi->second);
  all_options.clear();
}

void
OptionHandler::log_all_options() const
{
  // TODO: Log all options in a correct way!!!

//    for( int i = 0; i < number_of_long_options; i++ )
//      if( all_long_options[i].log() )
//        {
//          if( all_long_options[i].is_value_hexadecimal() )
//            the_arena.print_to_logfile
//              ( 'O', (int)'H', (all_long_options[i].get_rc_label() + ":").c_str(),
//                all_long_options[i]() );
//          else
//            the_arena.print_to_logfile
//              ( 'O', (int)'L', (all_long_options[i].get_rc_label() + ":").c_str(),
//                all_long_options[i]() );
//        }
//    for( int i = 0; i < number_of_double_options; i++ )
//      if( all_double_options[i].log() )
//        the_arena.print_to_logfile
//          ( 'O', (int)'D', (all_double_options[i].get_rc_label() + ":").c_str(),
//            all_double_options[i]() );
//    for( int i = 0; i < number_of_string_options; i++ )
//      if( all_string_options[i].log() )
//        the_arena.print_to_logfile
//          ( 'O', (int)'S', (all_string_options[i].get_rc_label() + ":").c_str(),
//            all_string_options[i]().c_str() );
}

// Try to read all options from the default_rc.
// TODO: If default_rc does not exist, create it.
void
OptionHandler::read_options_from_rtbrc()
{
  string resource_file = get_default_rc_file();
  read_options_file(resource_file,true);
}

void
OptionHandler::set_option_from_value( const string& option, const long int val,
                                      const bool as_default )
{
  map<string,Option*>::iterator mi;
  mi = all_options.find( option );
  assert( mi != all_options.end() &&
          mi->second->get_value_type() == OPTION_VALUE_LONG );
  ((LongOption*)mi->second)->change_value( val, as_default );
}

void
OptionHandler::set_option_from_value( const string& option, const double val,
                                      const bool as_default )
{
  map<string,Option*>::iterator mi;
  mi = all_options.find( option );
  assert( mi != all_options.end() &&
          mi->second->get_value_type() == OPTION_VALUE_DOUBLE );
  ((DoubleOption*)mi->second)->change_value( val, as_default );
}

void
OptionHandler::set_option_from_value( const string& option, const string& val,
                                      const bool as_default )
{
  set_option( option, val, as_default );
}

void
OptionHandler::set_option( const string& option, const string& val,
                           const bool as_default )
{
  map<string,Option*>::iterator mi;
  mi = all_options.find( option );
  assert( mi != all_options.end() );
  mi->second->change_value( val, as_default );
}

const bool
OptionHandler::is_option_existing( const string& option_name ) const
{
  return( all_options.end() != all_options.find(option_name) );
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

      remove_initial_whitespace( tempstr );
      
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
                                      const string& option_name, Option& option,
                                      const bool as_default )
{
  string::size_type pos = 0;
  if( locate_option_in_file( strfile, section_pos, option_name, pos ) )
    {
      string tempstr = strfile.substr( pos, strfile.find_first_of( '\n', pos ) - pos );
      while( tempstr.length() > 0 && isspace( tempstr[0] ) )
        tempstr = tempstr.substr( 1, string::npos );
      option.change_value( tempstr, as_default );
    }
}

void
OptionHandler::save_option_to_file( string& strfile, string::size_type& section_pos,
                                    const string& option_name, 
                                    const Option& option ) const
{
  string::size_type pos = 0;
  if( locate_option_in_file( strfile, section_pos, option_name, pos ) )
    strfile.replace( pos, strfile.find_first_of( '\n', pos ) - pos,
                     " " + option.get_string_val() );
  else
    {
      strfile += "\n;" + option.get_description() + "\n"
        + option_name + " = " + option.get_string_val() + "\n";
    }
}

bool
OptionHandler::read_options_file( const string& file_string, const bool as_default )
{
  string option_string = "";
  if( !load_file_into_string( option_string, file_string ) )
    return false;

  string::size_type section_pos = 0;
  locate_section_in_file( option_string, section_name, section_pos, false );

  map<string,Option*>::iterator mi;
  for( mi = all_options.begin(); mi != all_options.end(); mi++ )
    read_option_from_file( option_string, section_pos,
                           mi->first, *(mi->second), as_default );

  return true;
}

bool
OptionHandler::save_options_to_file( const string& fname, const bool as_default ) const
{
  string filename = fname;
  if( as_default )
    {
      filename = get_default_rc_file();
    }

  if( filename.empty() )
    return false;

  string option_string = "";
  if( !load_file_into_string( option_string, filename ) )
    option_string = "";

  string::size_type section_pos = 0;
  locate_section_in_file( option_string, section_name, section_pos, true );

  map<string,Option*>::const_iterator mci;
  for( mci = all_options.begin(); mci != all_options.end(); mci++ )
    save_option_to_file( option_string, section_pos, mci->first, *(mci->second) );

  if( !dump_string_to_file( option_string, filename ) )
    return false;

  return true;
}

string
OptionHandler::get_default_rc_file() const
{
  string filename;

  char* home_dir;
  if( NULL == ( home_dir = getenv("HOME") ) )
    return filename;

  filename = string(home_dir) + "/.rtb_v2/default_rc";
  return filename;
}
