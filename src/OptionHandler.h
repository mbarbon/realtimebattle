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

#ifndef __OPTIONHANDLER__
#define __OPTIONHANDLER__

#include <string>
#include <map>
#include <list>
#include <vector>
#include <fstream.h>
#include <assert.h>

#include "Option.h"

// ---------------------------------------------------------------------------
// class OptionHandler
// ---------------------------------------------------------------------------
// All Option*:s within the map supplied to Optionhandler are deleted
// when OptionHandler is destructed.
// ---------------------------------------------------------------------------
class OptionHandler
{
public:
  OptionHandler                          ( const string&, map<string,Option*>&,
                                           vector<string>& );

  ~OptionHandler                         ();

  inline const long int get_l            ( const string& option ) const;
  inline const double   get_d            ( const string& option ) const;
  inline const string   get_s            ( const string& option ) const;
  inline const bool     get_b            ( const string& option ) const;

  void log_all_options                   () const;

  bool save_options_to_file              ( const string&, const bool ) const;
  void read_options_from_rtbrc           ();
  bool read_options_file                 ( const string&, const bool );

  // set_option_from_value() uses the real value
  void set_option_from_value             ( const string&, const long int,
                                           const bool as_default = false );
  void set_option_from_value             ( const string&, const double,
                                           const bool as_default = false );
  void set_option_from_value             ( const string&, const string&,
                                           const bool as_default = false );
  // set_option() uses the stringified version of the value
  void set_option                        ( const string&, const string&,
                                           const bool as_default = false );

  const map<string,Option*>& get_options () const { return all_options; }
  const vector<string>& get_group_names  () const { return group_names; }

  string get_section_name                () const { return section_name; }

  const bool is_option_existing          ( const string& option_name ) const;

private:
  void initialize_groups                 ();

  // Optionfile functions
  bool load_file_into_string             ( string&, const string& ) const;
  bool dump_string_to_file               ( string&, const string& ) const;
  bool locate_section_in_file            ( string&, const string&,
                                           string::size_type&, const bool ) const;
  bool locate_option_in_file             ( const string&, const string::size_type,
                                           const string&, string::size_type& ) const;
  void read_option_from_file             ( string& strfile, string::size_type&,
                                           const string&, Option&, const bool );
  void save_option_to_file               ( string& strfile, string::size_type&,
                                           const string&, const Option& ) const;

  string get_default_rc_file             () const;

  string section_name;

  map<string,Option*> all_options;
  vector<string> group_names;
};

void Error(const bool fatal, const string& error_msg, const string& function_name);

inline const long int
OptionHandler::get_l( const string& option ) const
{
  map<string,Option*>::const_iterator mci;
  mci = all_options.find( option );
  assert( mci != all_options.end() &&
          ((mci->second)->get_value_type() == OPTION_VALUE_LONG) );
  return (((LongOption*)mci->second)->get_value());
}

inline const double
OptionHandler::get_d( const string& option ) const
{
  map<string,Option*>::const_iterator mci;
  mci = all_options.find( option );
  assert( mci != all_options.end() &&
          ((mci->second)->get_value_type() == OPTION_VALUE_DOUBLE) );
  return (((DoubleOption*)mci->second)->get_value());
}

inline const string
OptionHandler::get_s( const string& option ) const
{
  map<string,Option*>::const_iterator mci;
  mci = all_options.find( option );
  assert( mci != all_options.end() &&
          ((mci->second)->get_value_type() == OPTION_VALUE_STRING) );
  return (((StringOption*)mci->second)->get_value());
}

inline const bool
OptionHandler::get_b( const string& option ) const
{
  map<string,Option*>::const_iterator mci;
  mci = all_options.find( option );
  assert( mci != all_options.end() &&
          ((mci->second)->get_value_type() == OPTION_VALUE_BOOLEAN) );
  return (((BooleanOption*)mci->second)->get_value());
}

#endif __OPTIONHANDLER__
