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

#include <string>
#include <string.h>
//#include <iostream.h>
//#include <strstream.h>
//#include <iomanip.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "String.h"

bool
equal_strings_nocase( const string& s, const string& t )
{
  string::const_iterator li_s = s.begin();
  string::const_iterator li_t = t.begin();

  while( li_s != s.end() && li_t != t.end() )
    if( toupper(*li_s) != toupper(*li_t) )
      return false;

  return( s.size() == t.size() );
}

// Remember to delete[] the copied string
char*
copy_to_c_string( const string& str )
{
  char* cstr_p = new char[str.length() - 1];
  str.copy( cstr_p, string::npos );
  cstr_p[str.length()] = 0;
  return cstr_p;
}


char*
int2chars( const int n )
{
  char* array = new char[15];
  array[15] = '\0';
  int pos = 14;

  int k = ( n>0 ? n : -n );

  if( k == 0 ) 
    array[pos--] = '0';
  else 
    for(; k > 0; k/=10) array[pos--] = '0' + k%10;

  if( n < 0 ) 
    {
      array[0] = '-';
      strcpy(&array[1], &array[1+pos]); // ???
      //      erase(1, pos);
    }
  else
    strcpy(&array[0], &array[2+pos]); // ???
  //    erase(0, pos+1);

  return array;
}

char*
lint2chars( const long int n )
{
  char* array = new char[15];
  array[15] = '\0';
  int pos = 14;

  long int k = ( n>0 ? n : -n );

  if( k == 0 ) 
    array[pos--] = '0';
  else 
    for(; k > 0; k/=10) array[pos--] = '0' + k%10;

  if( n < 0 ) 
    {
      array[0] = '-';
      strcpy(&array[1], &array[1+pos]); // ???
      //      erase(1, pos);
    }
  else
    strcpy(&array[0], &array[1+pos]); // ???
  //    erase(0, pos+1);

  return array;
}

char*
double2chars( const double x, const int digits,
              const string_double_t form )
{
  char* result= new char[32];
  char format[16];

  char c = 'g';

  if( form == STRING_EXP_FORM ) 
    c = 'e';
  else if( form == STRING_FIXED_FORM ) 
    c = 'f';

  sprintf( format, "%%.%d%c", digits, c );
  sprintf( result, format, x );

  return result; 
}


string
int2string( const int n )
{
  char array[15];
  array[15] = '\0';
  int pos = 14;

  int k = ( n>0 ? n : -n );

  if( k == 0 ) 
    array[pos--] = '0';
  else 
    for(; k > 0; k/=10) array[pos--] = '0' + k%10;

  if( n < 0 ) 
    {
      array[0] = '-';
      strcpy(&array[1], &array[1+pos]); // ???
      //      erase(1, pos);
    }
  else
    strcpy(&array[0], &array[1+pos]); // ???
  //    erase(0, pos+1);

  return string(array);
}

string
lint2string( const long int n )
{
  char array[15];
  array[15] = '\0';
  int pos = 14;

  long int k = ( n>0 ? n : -n );

  if( k == 0 ) 
    array[pos--] = '0';
  else 
    for(; k > 0; k/=10) array[pos--] = '0' + k%10;

  if( n < 0 ) 
    {
      array[0] = '-';
      strcpy(&array[1], &array[1+pos]); // ???
      //      erase(1, pos);
    }
  else
    strcpy(&array[0], &array[1+pos]); // ???
  //    erase(0, pos+1);

  return string(array);
}

string
double2string( const double x, const int digits,
               const string_double_t form )
{
  char result[32];
  char format[16];

  char c = 'g';

  if( form == STRING_EXP_FORM ) 
    c = 'e';
  else if( form == STRING_FIXED_FORM ) 
    c = 'f';


  sprintf( format, "%%.%d%c", digits, c );
  sprintf( result, format, x );


  return string(result);  
}

int
chars2int( const char* p )
{
  return atoi( p );
}

long int
chars2lint( const char* p )
{
  return atol( p );
}

double
chars2double( const char* p )
{
  return atof( p );
}

int
string2int( const string& s )
{
  return atoi( s.c_str() );
}

long int
string2lint( const string& s )
{
  return atol( s.c_str() );
}

double
string2double( const string& s )
{
  return atof( s.c_str() );
}

long int
string2hex(const string& str)
{
  int pos = 0;
  long sign = 1;
  char c;
  long number = 0;

  if( str[pos] == '-' )
    {
      pos++;
      sign = -1;
    }

  for( ; pos < (int)str.length(); pos++)
    {
      c = str[pos];
      if( c >= '0' && c <= '9' ) 
        number=number*16+(c-'0');
      else if( c >='a' && c <= 'f' ) 
        number=number*16+10+(c-'a');
      else if( c >='A' && c <= 'F' ) 
        number=number*16+10+(c-'A');
      else break;
    }

  return sign*number;
}

string
hex2string(const long int n)
{
  char* array = new char[15];
  array[15] = '\0';
  int pos = 14;

  int k = ( n>0 ? n : -n );

  if( k == 0 ) 
    array[pos--] = '0';
  else 
    for(; k > 0; k/=16) 
      {
        if( k%16 < 10 ) 
          array[pos--] = '0' + k%16;
        else
          array[pos--] = 'A' - 10 + k%16;
      }

  if( n < 0 ) 
    {
      array[0] = '-';
      strcpy(&array[1], &array[1+pos]); // ???
      //      erase(1, pos);
    }
  else
    strcpy(&array[0], &array[1+pos]); // ???
  //    erase(0, pos+1);  

  return string(array);
}

long int
chars2hex(const char* str)
{
  int pos = 0;
  long sign = 1;
  char c;
  long number = 0;

  int length_of_str = strlen(str);

  if( str[pos] == '-' )
    {
      pos++;
      sign = -1;
    }

  for( ; pos < length_of_str; pos++)
    {
      c = str[pos];
      if( c >= '0' && c <= '9' ) 
        number=number*16+(c-'0');
      else if( c >='a' && c <= 'f' ) 
        number=number*16+10+(c-'a');
      else if( c >='A' && c <= 'F' ) 
        number=number*16+10+(c-'A');
      else break;
    }

  return sign*number;
}

char*
hex2chars(const long int n)
{
  char* array = new char[15];
  array[15] = '\0';
  int pos = 14;

  int k = ( n>0 ? n : -n );

  if( k == 0 ) 
    array[pos--] = '0';
  else 
    for(; k > 0; k/=16) 
      {
        if( k%16 < 10 ) 
          array[pos--] = '0' + k%16;
        else
          array[pos--] = 'A' - 10 + k%16;
      }

  if( n < 0 ) 
    {
      array[0] = '-';
      strcpy(&array[1], &array[1+pos]); // ???
      //      erase(1, pos);
    }
  else
    strcpy(&array[0], &array[1+pos]); // ???
  //    erase(0, pos+1);  

  return array;
}
