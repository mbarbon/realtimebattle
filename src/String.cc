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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string>
#include <string.h>
#include <vector>
//#include <iostream.h>
//#include <strstream.h>
//#include <iomanip.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <algorithm>

#include "IntlDefs.h"
#include "String.h"

bool
equal_strings_nocase( const string& s, const string& t )
{
  string::const_iterator li_s = s.begin();
  string::const_iterator li_t = t.begin();

  while( (li_s != s.end()) && (li_t != t.end()) )
    if( toupper(*li_s) != toupper(*li_t) )
      return false;
    else
      {
        li_s++;
        li_t++;
      }

  return( s.length() == t.length() );
}

string
lowercase( const string& s )
{
  string t = s;
  for( unsigned int i = 0; i < t.length(); i++ )
    t[i] = tolower( t[i] );
  return s;
}

string
uppercase( const string& s )
{
  string t = s;
  for( unsigned int i = 0; i < t.length(); i++ )
    t[i] = toupper( t[i] );
  return s;
}

void
remove_initial_whitespace( string& str )
{
  while( str.length() > 0 && isspace( str[0] ) )
    str = str.substr( 1, string::npos );
}

vector<string>&
split_string( const string& str, vector<string>& strlist, const string& separators )
{
  strlist.clear();
  string::size_type current_pos = 0;
  while( current_pos != string::npos )
    {
      string::size_type next_pos = str.find_first_of( separators, current_pos );
      string word = str.substr( current_pos, next_pos - current_pos );
      if( word != string("") )
        strlist.push_back( word );
      if( current_pos == next_pos )
        current_pos++;
      else
        current_pos = next_pos;
    }
  return strlist;
}

// Splits the strings into whitespace separated words. Special strings (e.g. strings
// separated by ") is taken as ONE word, this is translated if surrounded by _( )
// TODO: Enable use of backslash-sequences other than \\,\n,\t (if needed).
vector<string>&
special_split_string( const string& input_str, vector<string>& strlist )
{
  string str( input_str );
  if( (count( str.begin(), str.end(), '"' ) % 2) != 0 )
    {
      str += "\"";
      cerr << "Arena::special_split_string: input_str contains unterminated strings."
           << endl;
    }
  string::size_type pos = 0;
  string::size_type beg_pos = 0;
  // Note: It is somewhat ugly with two min in a row!
  while( (pos = min( min( str.find( '"', beg_pos ),
                          str.find( "_(\"", beg_pos ) ),
                     str.find( ';', beg_pos ) )) != string::npos )
    {
      bool translatable = false;
      vector<string> tmp_list;
      split_string( str.substr( beg_pos, pos - beg_pos ), tmp_list );
      strlist.insert( strlist.end(), tmp_list.begin(), tmp_list.end() );
      if( str[pos] == '"' )
        {
          translatable = false;
          beg_pos = pos + 1;
        }
      else if( str[pos] == ';' )
        return strlist;
      else
        {
          translatable = true;
          beg_pos = pos + 3;
        }
      string::size_type temp_pos = beg_pos;
      string::size_type end_pos = str.find( '"', beg_pos );
      string temp_string = str.substr( beg_pos, end_pos - beg_pos );
      if( str.substr( end_pos, 2 ) == "\")" && translatable )
        beg_pos = end_pos + 2;
      else
        {
          translatable = false;
          beg_pos = end_pos + 1;
        }
      string::size_type backslash_pos = 0;
      while( (backslash_pos = temp_string.find( '\\', temp_pos)) != string::npos )
        {
          if( temp_string.substr( backslash_pos, 2 ) == "\\n" )
            temp_string.replace( backslash_pos, 2, "\n" );
          if( temp_string.substr( backslash_pos, 2 ) == "\\t" )
            temp_string.replace( backslash_pos, 2, "\t" );
          if( temp_string.substr( backslash_pos, 2 ) == "\\\\" )
            temp_string.replace( backslash_pos, 2, "\\" );
          temp_pos = backslash_pos;
        }
      // Note: Should maybe add this to specify that this is a string.
      //      temp_string = "\"" + temp_string + "\"";
      if( translatable )
        strlist.push_back( _( temp_string.c_str() ) );
      else
        strlist.push_back( temp_string );
    }

  vector<string> tmp_list;
  split_string( str.substr( beg_pos, string::npos ), tmp_list );
  strlist.insert( strlist.end(), tmp_list.begin(), tmp_list.end() );
  return strlist;
}


string
spaced_string( const char* ch )
{
  return( ' ' + string(ch) + ' ' );
}

// Remember to delete[] the copied string
char*
copy_to_c_string( const string& str )
{
  char* cstr_p = new char[str.length() + 1];
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
