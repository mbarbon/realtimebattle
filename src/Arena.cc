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

#include <math.h>

#include "Arena.h"
#include "Gadgets/AllGadgets.h"
#include "Vector2D.h"
#include "String.h"
#include "Messagetypes.h"
#include "Various.h"
#include "Shot.h"
#include "Robot.h"

void
Arena::add_shot(Shot* s)
{
  if(  shots.insert(s).second == false )
    Error(true, "Couldn't add element", "Arena::add_shot");
}


void
Arena::find_subsquares_for_shape( const Shape& s )
{
  for(int i=0; i< number_of_subsquares_x; i++)
    for(int j=0; j< number_of_subsquares_y; j++)
      {        
        if( s.is_inside_subsquare( subsquares[i][j] ) )
          {
            subsquares[i][j].add(&s);
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
                             const class Robot* the_robot = NULL )
{
  int x;
  int y;

  get_subsquare_at( pos, x, y );

  // this is the fraction moved into the current subsquare
 double p_x = pos[0] / subsquare_size[0] - double(x);
 double p_y = pos[1] / subsquare_size[1] - double(y);

 // Find which is the main direction and the 'side'-direction. This is
 // used later to find the next subsquare to look in.

 int main_x, main_y;
 int side_x, side_y;


 // The side-direction coordinate. When s >= 1.0 move one subsquare
 // in the side-direction.
 double s;

 // (Fraction of) distance to the next subsquare in the main-direction.
 double l;

 // How much added to the side-direction when moving one
 // subsquare in the main direction
 double d;



 if( fabs(vel[0]) > fabs(vel[1]) )
   {
     main_y = side_x = 0;

     main_x = ( vel[0] >= 0.0 ? 1 : -1 );
     side_y = ( vel[1] >= 0.0 ? 1 : -1 );

     d = fabs( vel[1] / subsquare_size[1] / ( vel[0] / subsquare_size[0] ) );

     s = ( vel[1] >= 0.0 ? p_y : 1-p_y );
     l = ( vel[0] >= 0.0 ? 1-p_x : 1-p_x );
   }
 else
   {
     main_x = side_y = 0;
     
     main_y = ( vel[1] >= 0.0 ? 1 : -1 );
     side_x = ( vel[0] >= 0.0 ? 1 : -1 );

     d = fabs( vel[0] / subsquare_size[0] / ( vel[1] / subsquare_size[1] ) );

     s = ( vel[0] >= 0.0 ? p_x : 1-p_x );
     l = ( vel[1] >= 0.0 ? 1-p_y : 1-p_y );
   }


 // Now we can step through the subsquares until we find something

 double dist = 0.0;
 
 assert( d >= 0.0 && d <= 1.0 );
 assert( l >= 0.0 && l <= 1.0 );
 assert( s >= 0.0 && s <= 1.0 );

 do
   {
     s += d;
     if( s >= 1.0 )
       {
         s -= 1.0;
         x += side_x;
         y += side_y;

         if( x < 0 || x >= number_of_subsquares_x || 
             y < 0 || y >= number_of_subsquares_y ) return DBL_MAX; 

         dist = subsquares[x][y].get_shortest_distance(pos, vel, size, closest_shape, 
                                                        colliding_object, the_robot );

         if( dist < DBL_MAX/2.0 ) break;
       }
     
     x += main_x;
     y += main_y;
     
     if( x < 0 || x >= number_of_subsquares_x || 
         y < 0 || y >= number_of_subsquares_y ) return DBL_MAX; 

     dist = subsquares[x][y].get_shortest_distance(pos, vel, size, closest_shape, 
                                                    colliding_object, the_robot );

     if( dist < DBL_MAX/2.0 ) break;
   }
 while( true );

 return dist;
}

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
          if( equal_strings_nocase( wordlist[0], "Include" ) )
            file_stack.push( new ifstream( wordlist[1].c_str() ) );
          else if( equal_strings_nocase( wordlist[0], "Define" ) &&
                   wordlist.size() > 2 )
            {
              int i = 0;
              for( ;i < LAST_GADGET; i++ )
                if( equal_strings_nocase( wordlist[1], gadget_types[i] ) )
                  break;
              switch( gadget_t(i) )
                {
                case GAD_WEAPON:
                  {
                    // TODO: Remember to delete this somewehere!!!!!!!!!
                    WeaponGadget* gadget =
                      new WeaponGadget( wordlist[2].c_str(), current_gadget );
                    current_gadget->get_my_gadgets().add( gadget->get_info() );
                    current_gadget = (Gadget*)gadget;
                    break;
                  }
                case GAD_SHOT: 
                  {
                    // TODO: Remember to delete this somewehere!!!!!!!!!
                    ShotGadget* gadget =
                      new ShotGadget( wordlist[2].c_str(), current_gadget );
                    current_gadget->get_my_gadgets().add( gadget->get_info() );
                    current_gadget = (Gadget*)gadget;
                    break;
                  }
                case GAD_EXPLOSION: 
                  {
                    // TODO: Remember to delete this somewehere!!!!!!!!!
                    ExplosionGadget* gadget =
                      new ExplosionGadget( wordlist[2].c_str(), current_gadget );
                    current_gadget->get_my_gadgets().add( gadget->get_info() );
                    current_gadget = (Gadget*)gadget;
                    break;
                  }
                case GAD_DEFENSE: 
                  {
                    // TODO: Remember to delete this somewehere!!!!!!!!!
                    DefenseGadget* gadget =
                      new DefenseGadget( wordlist[2].c_str(), current_gadget );
                    current_gadget->get_my_gadgets().add( gadget->get_info() );
                    current_gadget = (Gadget*)gadget;
                    break;
                  }
                case GAD_ENGINE: 
                  {
                    // TODO: Remember to delete this somewehere!!!!!!!!!
                    EngineGadget* gadget =
                      new EngineGadget( wordlist[2].c_str(), current_gadget );
                    current_gadget->get_my_gadgets().add( gadget->get_info() );
                    current_gadget = (Gadget*)gadget;
                    break;
                  }
                case GAD_SENSOR: 
                  break;
                case GAD_COMMUNICATION:
                  {
                    // TODO: Remember to delete this somewehere!!!!!!!!!
                    CommunicationGadget* gadget =
                      new CommunicationGadget( wordlist[2].c_str(), current_gadget );
                    current_gadget->get_my_gadgets().add( gadget->get_info() );
                    current_gadget = (Gadget*)gadget;
                    break;
                  }
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
                  {
                    // TODO: Remember to delete this somewehere!!!!!!!!!
                    RobotBodyGadget* gadget =
                      new RobotBodyGadget( wordlist[2].c_str(), current_gadget );
                    current_gadget->get_my_gadgets().add( gadget->get_info() );
                    current_gadget = (Gadget*)gadget;
                    break;
                  }
                case GAD_SCORING: 
                  break;
                case GAD_ACTION:
                  break;
                case GAD_VARIABLE: 
                  {
                    // TODO: Remember to delete this somewehere!!!!!!!!!
                    Variable* gadget =
                      new Variable( wordlist[2].c_str(), current_gadget );
                    current_gadget->get_my_gadgets().add( gadget->get_info() );
                    current_gadget = (Gadget*)gadget;
                    break;
                  }
                case GAD_FUNCTION:
                  {
                    // TODO: Remember to delete this somewehere!!!!!!!!!
                    Function* gadget =
                      new Function( wordlist[2].c_str(), current_gadget );
                    current_gadget->get_my_gadgets().add( gadget->get_info() );
                    current_gadget = (Gadget*)gadget;
                    break;
                  }
                default:
                  Error( true, "Non existing gadget. Should not happen.",
                         "Arena::load_arena_file" );
                  break;
                }
            }
          else if( equal_strings_nocase( wordlist[0], "EndDefine" ) )
            {
              if( wordlist.size() > 2 && wordlist[2] != current_gadget->get_name() )
                Error( true, "Ending wrong definition", "Arena::load_arena_file" );
              current_gadget = current_gadget->get_parent();
            }
          else if( equal_strings_nocase( wordlist[0], "Function" ) &&
                   wordlist.size() > 2 )
            {
              Gadget* gadp = (*(current_gadget->get_my_gadgets().
                find_by_name( GadgetInfo( NULL, 0, wordlist[1].c_str() ) ))).gadgetp;
              if( typeid(gadp) == typeid(Function*) )// TODO: A better way to do this.
                {
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
            }
          else
            {
              Gadget* gadp = (*(current_gadget->get_my_gadgets().
                find_by_name( GadgetInfo( NULL, 0, wordlist[0].c_str() ) ))).gadgetp;
              if( typeid(gadp) == typeid(Variable*)// TODO: A better way to do this.
                  && wordlist.size() > 1 )
                {
                  if( isdigit( wordlist[0][0] ) || wordlist[0][0] == '.' )
                    ((Variable*)gadp)->operator=( string2double(wordlist[1]) );
                  else
                    {
                      // TODO: Do something when not a value
                      //       e.g. it is another variable.
                    }
                }
            }
        }
    }
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
