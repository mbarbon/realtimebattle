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

#include <stack>
#include <fstream>

#include "Arena.h"
#include "Gadget.h"
#include "Vector2D.h"

void
Arena::load_arena_file( const string& filename, Gadget& hierarchy )
{
  char buffer[400];
  string tempstr;

  stack<ifstream*> file_stack;
  file_stack.push( new ifstream( filename.c_str() ) );

  while( !file_stack.empty() )
    {
      ifstream* current_file = file_stack.top();
      if( current_file->fail() || current_file->eof() )
        {
          delete current_file;
          file_stack.pop();
          continue;
        }
      current_file->get( buffer, 400, '\n' );
      current_file->get();
      string str( buffer );
      string keyword = get_first_word_from_string( str, str );
      if( keyword == "Include" )
        file_stack.push
          ( new ifstream( get_first_word_from_string( str, tempstr ).c_str() ) );
    }
  
}

// Returns the next word in the string
string
Arena::get_first_word_from_string( const string str, string& remainder )
{
  remainder = str;
  while( remainder.length() > 0 && isspace( remainder[0] ) )
    remainder = remainder.substr( 1, string::npos );
  string word = remainder.substr( 0, remainder.find( ' ', string::npos ) );
  remainder =  remainder.substr( remainder.find( ' ', string::npos ), string::npos );
  return word;
}
