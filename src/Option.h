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

#ifndef __OPTION__
#define __OPTION__

#include "Various.h"
#include <string>

enum option_group_t
{
  GROUP_ENVIRONMENT,
  GROUP_ROBOT,
  GROUP_SHOT,
  GROUP_EXTRAS,
  GROUP_TIME,
  GROUP_SIZE_OF_WINDOWS,
  GROUP_MISC,
  LAST_GROUP
};

// ---------------------------------------------------------------------------
// class Option
// ---------------------------------------------------------------------------
// Base class for different options
// ---------------------------------------------------------------------------

class Option
{
public:
  Option::Option                       () {}
  Option::Option                       ( option_group_t g, const bool br,
                                         const bool lo, const char* r,
                                         const char* t ) :
    group(g), broadcast_option(br), log_option(lo), rc_label(r),
    description(t) {}
  Option::~Option                      () {}

  const option_group_t get_group       () const { return group; }
  const bool broadcast                 () const { return broadcast_option; }
  const bool log                       () const { return log_option; }
  const string& get_rc_label           () const { return rc_label; }
  const string& get_description   () const { return description; }
  
private:

  option_group_t group;

  bool broadcast_option;
  bool log_option;

  string rc_label;
  string description;
};

// ---------------------------------------------------------------------------
// class IntOption
// ---------------------------------------------------------------------------
// Options that consists of an integer.
// User might see the value as hexadecimal.
// ---------------------------------------------------------------------------

class LongOption : public Option
{
public:
  LongOption::LongOption               () {}
  LongOption::LongOption               ( option_group_t g, const long int val,
                                         const long int mn, const long int mx,
                                         const bool br, const bool lo,
                                         const char* r, const char* t,
                                         const bool hex = false ) :
    Option(g,br,lo,r,t), value(val), default_value(val),
    min_value(mn), max_value(mx), hexadecimal(hex) {}
  LongOption::~LongOption              () {}

  const int operator()                 () const { return value; }
  void reset_value                     ();
  const bool is_value_accepted         ( const long int ) const;
  const bool change_value              ( const long int, const bool def = false );
  const bool is_value_hexadecimal      () const { return hexadecimal; }
  const long int get_min_value         () const { return min_value; }
  const long int get_max_value         () const { return max_value; }

private:
  long int value;
  long int default_value;
  long int min_value;
  long int max_value;
  bool hexadecimal;
};

// ---------------------------------------------------------------------------
// class DoubleOption
// ---------------------------------------------------------------------------
// Options that consists of a double.
// ---------------------------------------------------------------------------

class DoubleOption : public Option
{
public:
  DoubleOption::DoubleOption           () {}
  DoubleOption::DoubleOption           ( option_group_t g, const double val,
                                         const double mn, const double mx,
                                         const bool br, const bool lo,
                                         const char* r, const char* t ) :
    Option(g,br,lo,r,t), value(val), default_value(val),
    min_value(mn), max_value(mx) {}
  DoubleOption::~DoubleOption          () {}

  const double& operator()             () const { return value; }
  void reset_value                     ();
  const bool is_value_accepted         ( const double ) const;
  const bool change_value              ( const double, const bool def = false );
  const double get_min_value           () const { return min_value; }
  const double get_max_value           () const { return max_value; }

private:
  double value;
  double default_value;
  double min_value;
  double max_value;
};

// ---------------------------------------------------------------------------
// class StringOption
// ---------------------------------------------------------------------------
// Options that consists of a string.
// ---------------------------------------------------------------------------

class StringOption : public Option
{
public:
  StringOption::StringOption           () {}
  StringOption::StringOption           ( option_group_t g, const string& val,
                                         const unsigned int mc,
                                         const bool br, const bool lo,
                                         const char* r, const char* t ) :
    Option(g,br,lo,r,t), value(val), default_value(val), max_chars(mc) {}
  StringOption::~StringOption          () {}

  const string& operator()             () const { return value; }
  void reset_value                     ();
  void change_value                    ( const string&, const bool def = false );

private:
  string value;
  string default_value;

  unsigned int max_chars;
};

#endif __OPTION__
