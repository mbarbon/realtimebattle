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

#include <iostream.h>
#include <stdio.h>
#include "Error.h"
  
Error::Error(const String& str, const String& func) 
  : error_string(str), error_string2(""), function_string(func), c(' ') {}


Error::Error(const String& str1, const String& str2, const String& func) 
  : error_string(str1), error_string2(str2), function_string(func), c(' ') {}

Error::Error(const String& str, char ch, const String& func) 
  : error_string(str), error_string2(""), function_string(func), c(ch) {}

void 
Error::print_message() 
{ 
  cerr << "RealTimeBattle: Error in " << function_string << ": " << error_string << " " 
       << error_string2 << " " << c << endl;

  perror("RealTimeBattle: errno message");
}

