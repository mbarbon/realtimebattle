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

#include <stack>
#include <fstream>
#include <typeinfo>
#include <set>
#include <algorithm>
#include <vector>

#include <math.h>
#include <assert.h>

#include "Arena.h"
#include "Gadgets/AllGadgets.h"
#include "Vector2D.h"
#include "String.h"
#include "Messagetypes.h"
#include "Various.h"
#include "Shot.h"
#include "Robot.h"
#include "SubSquareIterators.h"
#include "IntlDefs.h"

void
Arena::add_shot(Shot* s)
{
  if(  shots.insert(s).second == false )
    Error(true, "Couldn't add element", "Arena::add_shot");
}


void
Arena::find_subsquares_for_shape( Shape* s )
{
  for(int i=0; i< number_of_subsquares_x; i++)
    for(int j=0; j< number_of_subsquares_y; j++)
      {        
        if( s->is_inside_subsquare( subsquares[i][j] ) )
          {
            subsquares[i][j].add( s );
          }
            
      }
}

SubSquare*
Arena::get_subsquare_at( const Vector2D& pos ) const
{
  int x = (int)((pos[0] - top_left[0] ) / subsquare_size[0]);
  int y = (int)((pos[1] - top_left[1] ) / subsquare_size[1]);

  assert( x >= 0 && x < number_of_subsquares_x );
  assert( y >= 0 && y < number_of_subsquares_y );
  
  return( &subsquares[x][y] );
}

void
Arena::get_subsquare_at( const Vector2D& pos, int& x, int& y ) const
{
  x = (int)((pos[0] - top_left[0] ) / subsquare_size[0]);
  y = (int)((pos[1] - top_left[1] ) / subsquare_size[1]);
}


double
Arena::get_shortest_distance(const Vector2D& pos, const Vector2D& vel, 
                             const double size, enum object_type& closest_shape, 
                             Shape*& colliding_object, 
                             const class Shape* from_shape = NULL )
{
  
  SubSquareLineIterator ss_it(pos, vel);
  double dist;

  for( ; ss_it.ok(); ss_it++ )
   {
     
     dist = subsquares[ss_it.x()][ss_it.y()].
       get_shortest_distance(pos, vel, size, closest_shape, colliding_object, from_shape );
     
     if( dist < DBL_MAX/2.0 ) return dist;
   }     

 return DBL_MAX;
}




void
Arena::load_arena_file( const string& filename, Gadget& hierarchy )
{
  enum load_file_mode_t { LAF_DEFINING_MODE, LAF_VARIABLE_MODE,
                          LAF_SCRIPT_MODE, LAF_GEOMETRY_MODE };

  char buffer[400];
  load_file_mode_t mode = LAF_DEFINING_MODE;

  string top_filename = filename;
  if( !find_full_arena_filename( top_filename ))
    Error( true, "Couldn't find this arena: " + filename, "Arena::load_arena_file" );

  string top_file_path = top_filename.substr( 0, top_filename.rfind( '/' ) );

  stack<ifstream*> file_stack;
  file_stack.push( new ifstream( top_filename.c_str() ) );

  bool first_line = true;

  Gadget* current_gadget = &hierarchy;

  while( !file_stack.empty() )
    {
      ifstream* current_file = file_stack.top();
      string str_buffer;
      bool first_time = true;
      while( ((count( str_buffer.begin(), str_buffer.end(), '"' ) % 2) != 0) ||
             first_time )
        {
          if( first_time )
            first_time = false;
          if( current_file->fail() || current_file->eof() )
            {
              delete current_file;
              file_stack.pop();
              continue;
            }
          current_file->get( buffer, 400, '\n' );
          current_file->get();
          str_buffer += buffer;
          if( ((count( str_buffer.begin(), str_buffer.end(), '"' ) % 2) != 0) )
            str_buffer += '\n';
        }
      vector<string> wordlist = special_split_string( buffer, wordlist );
      remove_comments( wordlist );
      if( wordlist.size() > 0 )
        {
          if( first_line && wordlist[0][0] == '!' )
            {
              if( !sufficient_arena_version( wordlist ) )
              first_line = false;
            }
          if( equal_strings_nocase( wordlist[0], "Include" ) )
            {
              if( !find_full_arena_filename( wordlist[1], top_file_path, true ) )
                Error( false, "Couldn't find this include file: " + wordlist[1],
                       "Arena::load_arena_file" );
              file_stack.push( new ifstream( wordlist[1].c_str() ) );
            }
          else if( mode == LAF_DEFINING_MODE &&
                   equal_strings_nocase( wordlist[0], "Define" ) &&
                   wordlist.size() > 2 )
            {
              for( int i = 0;i < LAST_GADGET; i++ )
                if( equal_strings_nocase( wordlist[1], gadget_types[i] ) )
                  {
                    // TODO: Remember to delete this somewehere!!!!!!!!!
                    Gadget* gadget = create_gadget( gadget_t(i), wordlist[2],
                                                    current_gadget );
                    assert( gadget != NULL );
                    current_gadget->get_my_gadgets().add( gadget->get_info() );
                    current_gadget = (Gadget*)gadget;
                    if( gadget_t(i) == GAD_SCRIPT )
                      mode = LAF_SCRIPT_MODE;
                    break;
                  }
            }
          else if( equal_strings_nocase( wordlist[0], "EndDefine" ) )
            {
              if( wordlist.size() > 2 && wordlist[2] != current_gadget->get_name() )
                Error( true, "Ending wrong definition", "Arena::load_arena_file" );
              current_gadget = current_gadget->get_parent();
              mode = LAF_DEFINING_MODE;
            }
          else if( mode == LAF_DEFINING_MODE &&
                   equal_strings_nocase( wordlist[0], "Function" ) &&
                   wordlist.size() > 2 )
            {
              Gadget* gadp = (*(current_gadget->get_my_gadgets().
                find_by_name( GadgetInfo( NULL, 0, wordlist[1].c_str() ) ))).gadgetp;
              assert( typeid(gadp) == typeid(Function*) );
              Function* func_p = (Function*)func_p;
              if( equal_strings_nocase( wordlist[2], "default" ) )
                {
                  // TODO: Set Function allowance to default
                }
              else if( equal_strings_nocase( wordlist[2], "true" ) )
                {
                  // TODO: Set Function allowance to true
                }
              else if( equal_strings_nocase( wordlist[2], "false" ) )
                {
                  // TODO: Set Function allowance to false
                }
            }              
          else if( mode == LAF_VARIABLE_MODE )
            {
              assert( typeid(current_gadget) == typeid(Variable*) );
            }
          else if( mode == LAF_SCRIPT_MODE )
            {
              assert( typeid(current_gadget) == typeid(Script*) );
              ((Script*)current_gadget)->add_script_line( wordlist );
            }            
          else if( mode == LAF_GEOMETRY_MODE )
            {
            }
          else if( wordlist.size() > 1 )
            {
              Gadget* gadp = (*(current_gadget->get_my_gadgets().
                                find_by_name( GadgetInfo( NULL, 0,
                                                          wordlist[0].c_str() )
                                              ))).gadgetp;
              assert( typeid(gadp) == typeid(Variable*) );
              // TODO: set value to the found gadget
            }
        }
    }
}

// Returns true if filename found. filename changes to be the full path of the
// supplied filename. If filename is already a full filename nothing is changed.
// Searches the arena path for the arena (searches top_file_path first if this
// is non-empty). 
const bool
Arena::find_full_arena_filename( string& filename, const string& top_file_path,
                                 const bool included ) const
{
  bool (*check_file_function)( const string& ) = check_if_filename_is_arena;
  if( included )
    check_file_function = check_if_filename_is_regular_file;

  if( (*check_file_function)( filename ) )
    return true;
  if( (!top_file_path.empty()) &&
      (filename.length() > 0) &&
      (filename[0] != '/') &&
      ((*check_file_function)( top_file_path + filename )) )
    {
      filename = top_file_path + filename;
      return true;
    }
  list<string> arenadirs;
  read_arenadirs_from_system( arenadirs );
  for( list<string>::const_iterator lci = arenadirs.begin();
       lci != arenadirs.end(); lci++ )
    if( (*check_file_function)( *lci + filename ) )
      {
        filename = top_file_path + filename;
        return true;
      }
  return false;
}

const bool
Arena::sufficient_arena_version( vector<string>& wordlist ) const
{
  // Note: Is it necessary to specify arena-version?
  return true;
}

// Splits the strings into whitespace separated words. Special strings (e.g. strings
// separated by ") is taken as ONE word, this is translated if surrounded by _( )
// TODO: Enable use of backslash-sequences other than \,\n,\t (if needed).
// TODO: It should be possible to write this function in a better way.
vector<string>&
Arena::special_split_string( const string& str, vector<string>& strlist ) const
{
  string::size_type pos = 0;
  string::size_type beg_pos = 0;
  while( (pos = min( str.find( '"', beg_pos ),
                     str.find( "_(\"", beg_pos ) ) ) != string::npos )
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
          if( temp_string.substr( backslash_pos, 2 ) == "\\\n" )
            temp_string.erase( backslash_pos, 2 );
          if( temp_string.substr( backslash_pos, 2 ) == "\\n" )
            temp_string.replace( backslash_pos, 2, "\n" );
          if( temp_string.substr( backslash_pos, 2 ) == "\\t" )
            temp_string.replace( backslash_pos, 2, "\t" );
          temp_pos = backslash_pos;
        }
      if( translatable )
        strlist.push_back( _( temp_string.c_str() ) );
      else
        strlist.push_back( temp_string ) );
    }

  vector<string> tmp_list;
  split_string( str.substr( beg_pos, string::npos ), tmp_list );
  strlist.insert( strlist.end(), tmp_list.begin(), tmp_list.end() );
  return strlist;
}

void
Arena::remove_comments( vector<string>& wordlist ) const
{
  for( vector<string>::iterator vi = wordlist.begin(); vi != wordlist.end(); vi++ )
    if( (*vi).length() > 0 && (*vi)[0] == ';' )
      wordlist.erase( vi, wordlist.end() );
}

// Remember to delete the gadget when not used anymore!
Gadget*
Arena::create_gadget( gadget_t type, const string& name, Gadget* parent ) const
{
  Gadget* gadget = NULL;
  switch( type )
    {
    case GAD_WEAPON:
      gadget = (Gadget*) new WeaponGadget( name.c_str(), parent );
      break;
    case GAD_SHOT: 
      gadget = (Gadget*) new ShotGadget( name.c_str(), parent );
      break;
    case GAD_EXPLOSION: 
      gadget = (Gadget*) new ExplosionGadget( name.c_str(), parent );
      break;
    case GAD_DEFENSE: 
      gadget = (Gadget*) new DefenseGadget( name.c_str(), parent );
      break;
    case GAD_ENGINE: 
      gadget = (Gadget*) new EngineGadget( name.c_str(), parent );
      break;
    case GAD_SENSOR: 
      break;
    case GAD_COMMUNICATION:
      gadget = (Gadget*) new CommunicationGadget( name.c_str(), parent );
      break;
    case GAD_WALL: 
      break;
    case GAD_BALL: 
      break;
    case GAD_EQUIPMENT:
      break;
    case GAD_ENVIRONMENT: 
      break;
    case GAD_ROBOTBODY: 
      gadget = (Gadget*) new RobotBodyGadget( name.c_str(), parent );
      break;
    case GAD_SCORING: 
      break;
    case GAD_VARIABLE: 
      gadget = (Gadget*) new Variable( name.c_str(), parent );
      break;
    case GAD_FUNCTION:
      gadget = (Gadget*) new Function( name.c_str(), parent );
      break;
    case GAD_SCRIPT:
      gadget = (Gadget*) new Script( name.c_str(), parent );
      break;
    default:
      Error( true, "Non existing gadget. Should not happen.",
             "Arena::load_arena_file" );
      break;
    }
  return gadget;
}

void
Arena::garbage_collector()
{
  // The killed objects must be removed from all subsquares first

  // clean subsquares

  for( int i=0; i < number_of_subsquares_x; i++ )
    for( int j=0; j < number_of_subsquares_y; j++ )
      {
        subsquares[i][j].clean();
      }


  //clean lists

  for( set<Shot*>::iterator li = shots.begin(); li != shots.end(); li++ )
    if( (*li)->is_killed() )  { shots.erase(li); delete (*li); }

  for( set<Robot*>::iterator li = robots.begin(); li != robots.end(); li++ )
    if( (*li)->is_killed() )  robots.erase(li);

  for( set<MovingObject*>::iterator 
         li = moving_objects.begin(); li != moving_objects.end(); li++ )
    if( (*li)->is_killed() ) { moving_objects.erase(li);  delete (*li); }

  for( set<Shape*>::iterator
         li = updatable_objects.begin(); li != updatable_objects.end(); li++ )
    if( (*li)->is_killed() ) { updatable_objects.erase(li);  delete (*li); }

  for( set<Shape*>::iterator
         li = static_objects.begin(); li != static_objects.end(); li++ )
    if( (*li)->is_killed() ) { static_objects.erase(li);  delete (*li); }

}
