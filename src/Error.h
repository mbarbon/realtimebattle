/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998  Erik Ouchterlony and Ragnar Ouchterlony

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

#ifndef __ERROR__
#define __ERROR__

#include "String.h"

class Error
{
public:
  Error() {}
  Error(const String& str, const String& func);
  Error(const String& str1, const String& str2, const String& func);
  Error(const String& str, char ch, const String& func);
  ~Error() {}
  void print_message();
  
  friend class Arena;
  friend class ArenaObject;
private:

  String error_string;
  String error_string2;
  String function_string;
  char c;
};

#endif __ERROR__
