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

#ifndef __RTB_VALUE__
#define __RTB_VALUE__

class Value
{
public:
  Value(const int i=0) : i_val(i), is_double(false) {}
  Value(const double v) : d_val(v), is_double(true) {}

  // conversion to double/int
  operator int() const;
  operator double() const;



  // Arithmetics

  Value& operator= (const double d) { d_val = d; is_double = true; return *this; }
  Value& operator= (const int i) { i_val = i; is_double = false; return *this; }

  Value& operator+=(const Value& v);
  Value& operator-=(const Value& v);
  Value& operator*=(const Value& v);
  Value& operator/=(const Value& v);

  Value& invert();
  Value& neg();

  // Comparison operators

  // These operator replaces the content of this object by the result of
  // the comparison

  Value& operator< (const Value& v);
  Value& operator<= (const Value& v);
  Value& operator> (const Value& v);
  Value& operator>= (const Value& v);

  Value& operator== (const Value& v);
  Value& operator!= (const Value& v);

  // Trigonometry

  Value& cos();
  Value& sin();
  Value& tan();
  Value& asin();
  Value& acos();
  Value& atan();
  
public:  // to get faster and easier access

  double d_val;
  int i_val;

  bool is_double;
};

#endif // __RTB_VALUE__
