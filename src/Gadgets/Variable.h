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

#ifndef __VARIABLE_GADGET__
#define __VARIABLE_GADGET__

#include "Gadget.h"

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
  Variable( const char* name, Gadget* const p ) : Gadget(name, p) {}
  ~Variable() {}

  void make_bool( const double val=double(false) );

  void make_int ( const double val, 
                  const double min_val = INT_MIN, 
                  const double max_val= INT_MAX );

  void make_double( const double val, const double min_val, const double max_val,
                    const double inaccuracy = 0.0 );

  void make_random ( const double min_val, const double max_val, const bool is_double=true );  


  void set_robot_permissions( const bool read, const bool write );



  // conversions to/from double

  operator double() const { return get_value(); }
  //  double operator() () { return get_value(); }

  const Variable& operator= (const double val) { assign(val); return *this; }



private:
  
  void assign(const double val);
  double get_value() const;


  variable_type my_type;
  // all types are represented as double
  double value;

  bool public_readable;  // Whether it can be sent to robots
  bool public_writable;  // Whether it can be set by robots

  double minimum;
  double maximum;


  // A constant is not random and has no inaccuracy
  bool constant;


  bool random;   // completely random
  double inaccuracy;  // a normal-distributed error
  
};


#endif __VARIABLE_GADGET__
