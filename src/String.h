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

#ifndef __RTB_STRING__
#define __RTB_STRING__

#include <vector>
#include <list>

enum string_double_t { STRING_NORMAL_FORM, STRING_EXP_FORM, STRING_FIXED_FORM };

bool equal_strings_nocase( const string&, const string& );
void remove_initial_whitespace( string& );

vector<string>& split_string( const string& str , vector<string>& strlist,
                              const string& separators = " \n\t\v\r\f" );

string spaced_string( const char* );

// Remember to delete[] the copied string
char* copy_to_c_string( const string& str );

char* int2chars   ( const int );
char* lint2chars  ( const long int );
char* double2chars( const double, const int digits = 10,
                    const string_double_t form = STRING_NORMAL_FORM );

string int2string   ( const int );
string lint2string  ( const long int );
string double2string( const double, const int digits = 10,
                      const string_double_t form = STRING_NORMAL_FORM );

int chars2int      ( const char* );
long int chars2lint( const char* );
double chars2double( const char* );

int string2int      ( const string& );
long int string2lint( const string& );
double string2double( const string& );

long int string2hex( const string& );
string hex2string  ( const long );

long int chars2hex( const char* );
char* hex2chars   ( const long );

#endif __RTB_STRING__
