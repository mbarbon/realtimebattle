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
#include "String.h"

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
      vector<string> wordlist = split_string( buffer, wordlist );
      if( wordlist.size() > 0 )
        {
          if( wordlist[0] == "Include" )
            file_stack.push( new ifstream( wordlist[1].c_str() ) );
          else if( wordlist[0] == "Define" && wordlist.size() > 2 )
            {
              // TODO: Create new Gadget in the hierarchy, might be:
              // initialize_gadget( wordlist[1], wordlist[2] );
            }
          else if( wordlist[0] == "EndDefine" )
            {
              // TODO: Gadgetdefinition finished
            }
          else
            {
              // TODO: Check if this sets a function or variable in the current Gadget
            }
        }
    }
}
