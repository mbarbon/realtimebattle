/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-1999  Erik Ouchterlony and Ragnar Ouchterlony

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

#include "ArenaReplay.h"
#include "Various.h"
#include "Extras.h"
#include "Shot.h"

ArenaReplay::ArenaReplay()
{
  reset_timer();
  state = NOT_STARTED;
  next_check_time = 0.0;
}

ArenaReplay::~ArenaReplay()
{
}

bool
ArenaReplay::timeout_function()
{
  update_timer();

  while( !log_file.eof() && total_time >= next_check_time )
    {
      if( next_check_time > last_replay_time )
        move_shots( next_check_time - last_replay_time );

      last_replay_time = next_check_time;
      parse_log_line( log_file );
    }

  return true;
}
  
void 
ArenaReplay::start_tournament()
{
  do
    {
      parse_log_line( log_file );
    } while( !log_file.eof() );
}
  
void 
ArenaReplay::end_game()
{
}

void 
ArenaReplay::update()
{
}

void 
ArenaReplay::start_game()
{
}

void 
ArenaReplay::start_sequence()
{
}

void 
ArenaReplay::end_sequence()
{
}

void 
ArenaReplay::end_tournament()
{
}

char
ArenaReplay::parse_log_line( ifstream& file )
{
  char first_letter = '?';

  file >> ws;
  file.get( first_letter );

  cout << "first letter: " << first_letter << endl;
  //  bool get_the_rest_of_the_line = true;
  switch( first_letter )
    {
    case 'R': // Robot pos
      {
        int robot_id;
        double x, y, cannon_angle, radar_angle, energy;
        file >> robot_id >> x >> y >> cannon_angle >> radar_angle >> energy;
        // change_robot_position( robot_id, x, y, cannon_angle, radar_angle, energy );
      }
      break;
    case 'T': // Time
      {
        file >> next_check_time;
        cout << next_check_time << "  " << total_time << endl;
      }
      break;
    case 'P': // Print a robot message
      {
        int robot_id;
        char message[200];
        file >> robot_id;
        file.get( message, 200, '\n' );
        // add_message( robot_id, message );
        //        get_the_rest_of_the_line = false;
      }
      break;
    case 'C': // Cookie
      {
        int cookie_id;
        double x, y;
        file >> cookie_id >> x >> y;
        Cookie* cookiep = new Cookie( Vector2D(x,y), 0.0, cookie_id);
        object_lists[COOKIE].insert_last( cookiep );
      }
      break;
    case 'M': // Mine
      {
        int mine_id;
        double x, y;
        file >> mine_id >> x >> y;
        Mine* minep = new Mine( Vector2D(x,y), 0.0, mine_id);
        object_lists[MINE].insert_last( minep );
      }
      break;
    case 'S': // Shot
      {
        int shot_id;
        double x, y, dx, dy;
        file >> shot_id >> x >> y >> dx >> dy;
        Shot* shotp = new Shot( Vector2D(x,y), Vector2D(dx,dy), 0, shot_id );
        object_lists[SHOT].insert_last( shotp );
      }
      break;
    case 'D': // Die
      {
        char object_type = '?';
        int object_id;
        file.get( object_type );
        file >> object_id;
        switch( object_type )
          {
          case 'R':
            {
              double points_received;
              file >> points_received;
              // robot_die( object_id, points_received );
            }
            break;
          case 'C':
            // cookie_die( object_id );
            break;
          case 'M':
            // mine_die( object_id );
            break;
          case 'S':
            // shot_die( object_id );
            break;
          case '?':
          default:
            Error( false, "Unknown object type is dead", "ArenaReplay::parse_log_line" );
            break;
          }
      }
      break;
    case 'G': // Game begins
      {
        int sequence, game;
        file >> sequence >> game;
        // game_starts( sequence, game );
      }
      break;
    case 'H': // Header
      {
        int gps, rps, seq, robots;
        file >> gps >> rps >> seq >> robots;
        // init( gps, rps, seq, robots );
      }
      break;
    case 'L': // List of robot properties
      {
        int robot_id;
        char robot_colour[7];
        char name[200];
        file >> robot_id;
        file.get( robot_colour, 6, ' ');
        file.get( name, 200, '\n' );
        // Robot* robotp = new Robot( robot_id, robot_colour, name );
        // object_lists[ROBOT].insert_last(robotp); // array bättre?
        //        get_the_rest_of_the_line = false;
      }
      break;
    case 'A': // Arena file line
      {
        char arena_line[400];
        file.get( arena_line, 400, '\n' );
        // parse_arena_line( arena_line );
      }
      break;
    case 'O': // Option
      {
        char temp, option_type = '?';
        char label[200];
        file.get( option_type );
        file.get( label, 200, ':');
        file.get( temp );

//          switch( option_type )
//            {
//            case 'D':
//              {
//                double value;
//                file >> value;
//                // set_double_opt( label, value, ENTRY_DOUBLE );
//              }
//              break;
//            case 'L':
//              {
//                long value;
//                file >> value;
//                // set_long_opt( label, value, ENTRY_LONG );
//              }              
//              break;
//            case 'H':
//              {
//                long value;
//                file >> value;
//                // set_long_opt( label, value, ENTRY_HEX );
//              }
//              break;
//            case 'S':
//              {
//                char text[400];
//                file.get( text, 400, '\n' );
//                // set_string_opt( label, value, ENTRY_STRING );
//                //              get_the_rest_of_the_line = false;
//              }
//              break;
//            case '?':
//            default:
//              Error( false, "Unknown option type", "ArenaReplay::parse_log_line" );
//              break;
//      }
        char line[400];
        file.get( line, 400, '\n' );

      }
      break;

    case '?':
    default:
      Error( false, "Unrecognized first letter in logfile: " + 
             String(first_letter), "ArenaReplay::parse_log_line" );
      break;
    }

  //  if( get_the_rest_of_the_line )
  //    {
  //      char buffer[200];
  //      file.get( buffer, 200, '\n' ); // Ignore the rest of the line
  //    }

  return first_letter;
}

void
ArenaReplay::set_filenames( String& replay_file )
{
  if( replay_file != "-" )
    log_file.open( replay_file.chars() );
  else
    log_file.attach( STDIN_FILENO );
}
