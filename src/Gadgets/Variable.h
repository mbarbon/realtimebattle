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

#ifndef __VARIABLE_GADGET__
#define __VARIABLE_GADGET__

#include "Gadget.h"
#include "Value.h"

enum variable_type { BOOL_V, INT_V, DOUBLE_V };

struct VariableDefinition
{
  char* name;
  enum variable_type type;
  double value;
  double min_val;
  double max_val;
  double inaccuracy;
  bool random;
  bool readable;
  bool writable;
};

class Variable : public Gadget
{

public:

  Variable() {}
  Variable( const char* name, Gadget* const p ) : Gadget(name, p, GAD_VARIABLE) {}
  ~Variable() {}

  static Gadget* new_variable( const char* name, Gadget* const p )
  {
    return new Variable( name, p );
  }

  void make_bool( const double val=double(false) );

  void make_int ( const double val, 
                  const double min_val = INT_MIN, 
                  const double max_val= INT_MAX );

  void make_double( const double val, const double min_val, const double max_val,
                    const double inaccuracy = 0.0 );

  void make_random ( const double min_val, const double max_val, const bool is_double=true );  


  void set_robot_permissions( const bool read, const bool write );



  // conversions to/from Value

  operator const Value() { return get_value(); }
  operator const double() const  { return double(get_value()); }
  operator const int() { return int(get_value()); }

  const Variable& operator= (const Value& val) { assign(val); return *this; }
  void assign(const Value& val);

  Variable& operator+=(int n);
  Variable& operator-=(int n);
  Variable& operator*=(int n);
  Variable& operator/=(int n);

private:

  Value& get_value() const;

  variable_type my_type;

  mutable Value value;

  bool public_readable;  // Whether it can be sent to robots
  bool public_writable;  // Whether it can be set by robots

  Value minimum;
  Value maximum;


  // A constant is not random and has no inaccuracy
  bool constant;


  bool random;   // completely random

  double inaccuracy;  // a normal-distributed
  double mean;        // error
};


#endif // __VARIABLE_GADGET__
