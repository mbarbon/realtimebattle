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
#include "WeaponGadget.h"
#include "Vector2D.h"
#include "String.h"
#include "Messagetypes.h"
#include "Various.h"

void
Arena::load_arena_file( const string& filename, Gadget& hierarchy )
{
  char buffer[400];
  string tempstr;

  stack<ifstream*> file_stack;
  file_stack.push( new ifstream( filename.c_str() ) );

  Gadget* current_gadget = &hierarchy;

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
              int i = 0;
              for( ;i < LAST_GADGET; i++ )
                if( equal_strings_nocase( wordlist[1], gadget_types[i] ) )
                  break;
              switch( i )
                {
                case GAD_WEAPON:
                  {
                    // TODO: Remember to delete this somewehere!!!!!!!!!
                    WeaponGadget* gadget =
                      new WeaponGadget( wordlist[2].c_str(), current_gadget );
                    current_gadget->my_gadgets.add( gadget->get_info() );
                    current_gadget = gadget;
                    break;
                  }
                case GAD_SHOT: 
                  break;
                case GAD_EXPLOSION: 
                  break;
                case GAD_DEFENSE: 
                  break;
                case GAD_ENGINE: 
                  break;
                case GAD_SENSOR: 
                  break;
                case GAD_COMMUNICATION: 
                  break;
                case GAD_WALL: 
                  break;
                case GAD_DEVICE: 
                  break;
                case GAD_BALL: 
                  break;
                case GAD_EQUIPMENT: 
                  break;
                case GAD_ENVIRONMENT: 
                  break;
                case GAD_ROBOTBODY: 
                  break;
                case GAD_SCORING: 
                  break;
                case GAD_ACTION: 
                  break;
                case GAD_VARIABLE: 
                  break;
                }
              // TODO: Create new Gadget in the hierarchy, might be:
              // initialize_gadget( wordlist[1], wordlist[2] );
            }
          else if( wordlist[0] == "EndDefine" )
            {
              if( wordlist.size() > 2 && wordlist[2] != current_gadget->get_name() )
                Error( true, "Ending wrong definition", "Arena::load_arena_file" );
              current_gadget = current_gadget->parent;
            }
          else
            {
              // TODO: Check if this sets a function or variable in the current Gadget
            }
        }
    }
}
