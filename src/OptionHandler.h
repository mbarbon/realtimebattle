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

#include <string>
#include <vector>
#include <fstream.h>

#include "Option.h"

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

// ---------------------------------------------------------------------------
// class OptionHandler
// ---------------------------------------------------------------------------
// Optionarrays supplied to Optionhandler are deleted when OptionHandler
// is destructed.
// ---------------------------------------------------------------------------

class OptionHandler
{
public:
  OptionHandler::OptionHandler           ( const string&,
                                           DoubleOption*, LongOption*, StringOption*,
                                           const int, const int, const int );

  OptionHandler::~OptionHandler          ();

  inline long int get_l                  ( const int option ) const
    { return all_long_options[option](); }
  inline double get_d                    ( const int option ) const
    { return all_double_options[option](); }
  inline string get_s                    ( const int option ) const
    { return all_string_options[option](); }

  void broadcast_opts                    ();
  void log_all_options                   ();

  void save_options_to_file              ( const string&, const bool ) const;
  void read_options_from_rtbrc           ();
  void read_options_file                 ( const string&, const bool );

  void set_long_option                   ( const int, const long int );
  void set_double_option                 ( const int, const double );
  void set_string_option                 ( const int, const string& );

  option_return_t get_option_from_string ( const string& option_name );

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
                                           DoubleOption, const bool );
  void read_option_from_file             ( string& strfile, string::size_type&,
                                           LongOption, const bool );
  void read_option_from_file             ( string& strfile, string::size_type&,
                                           StringOption, const bool );
  void save_option_to_file               ( string& strfile, string::size_type&,
                                           const DoubleOption ) const;
  void save_option_to_file               ( string& strfile, string::size_type&,
                                           const LongOption ) const;
  void save_option_to_file               ( string& strfile, string::size_type&,
                                           const StringOption ) const;

  string section_name;

  string group_names[LAST_GROUP];

  DoubleOption* all_double_options;
  LongOption*   all_long_options;
  StringOption* all_string_options;

  int number_of_double_options;
  int number_of_long_options;
  int number_of_string_options;
};

#endif __OPTIONHANDLER__
