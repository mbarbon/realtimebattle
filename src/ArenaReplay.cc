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
#include "Options.h"
#include "Extras.h"
#include "Shot.h"
#include "MessageWindow.h"
#include "ArenaWindow.h"
#include "Robot.h"

extern class String global_replay_fname;
extern int global_game_mode;

ArenaReplay::ArenaReplay()
{
  reset_timer();
  state = NOT_STARTED;
  next_check_time = 0.0;

  game_mode = (ArenaBase::game_mode_t)global_game_mode;
  set_filenames( global_replay_fname );
}

ArenaReplay::~ArenaReplay()
{
}

bool
ArenaReplay::timeout_function()
{
  if( state == GAME_IN_PROGRESS || state == BEFORE_GAME_START )
    update_timer();

  switch( state )
    {
    case NOT_STARTED:
      start_tournament();
      break;
    case GAME_IN_PROGRESS:
      parse_this_interval();
      break;

    case BEFORE_GAME_START:
      parse_this_interval();
      the_gui.get_arenawindow_p()->drawing_area_scale_changed();      
      the_gui.get_arenawindow_p()->draw_everything();      
      set_state( GAME_IN_PROGRESS );
      break;

    case FINISHED:
      return false;
      break;

    default:
      break;
    }
  return true;
}

void
ArenaReplay::parse_this_interval()
{
  while( !log_file.eof() && total_time >= next_check_time )
    {
      if( next_check_time > last_replay_time )
        {
          move_shots_no_check( next_check_time - last_replay_time );
          // check if robots have died and set thier points
          the_gui.get_arenawindow_p()->draw_moving_objects( true );
        }

      last_replay_time = next_check_time;
      parse_log_line();
    }
  
  if( log_file.eof() ) set_state( FINISHED );
}
  
void 
ArenaReplay::start_tournament()
{
#ifndef NO_GRAPHICS
  if( !no_graphics )
    {
      if( the_gui.is_messagewindow_up() )
        MessageWindow::clear_clist( NULL, the_gui.get_messagewindow_p() );
      else //if( !use_message_file )
        the_gui.open_messagewindow();

      if( !the_gui.is_scorewindow_up() ) the_gui.open_scorewindow();
      if( !the_gui.is_arenawindow_up() ) the_gui.open_arenawindow();
    }
#endif

  set_state( BEFORE_GAME_START );
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
ArenaReplay::parse_log_line()
{
  char first_letter = '?';

  log_file >> ws;
  log_file.get( first_letter );

  if( log_file.eof() ) return '?';
  //  cerr << "first letter: " << first_letter << endl;
  switch( first_letter )
    {
    case 'R': // Robot pos
      {
        int robot_id;
        double x, y, robot_angle, cannon_angle, radar_angle, energy;
        log_file >> robot_id >> x >> y >> 
          robot_angle >> cannon_angle >> radar_angle >> energy;
        ListIterator<Shape> li;
        find_object_by_id( object_lists[ROBOT], li, robot_id );
        if( !li.ok() ) Error(true, "Robot not in list", "ArenaReplay::parse_log_line");

        Robot* robotp = (Robot*)li();

        robotp->change_position( x, y, robot_angle, cannon_angle, radar_angle, energy );
        robotp->live();
      }
      break;
    case 'T': // Time
      {
        log_file >> next_check_time;
        //        cerr << next_check_time << "  " << total_time << endl;
      }
      break;
    case 'P': // Print a robot message
      {
        int robot_id;
        char message[200];
        log_file >> robot_id;
        log_file.get( message, 200, '\n' );
        ListIterator<Shape> li;
        find_object_by_id( object_lists[ROBOT], li, robot_id );
        if( !li.ok() ) Error(false, "Robot not in list", "ArenaReplay::parse_log_line");
        else
          {
        Robot* robotp = (Robot*)li();
        the_gui.get_messagewindow_p()->        
          add_message( robotp->get_robot_name(), (String)message );
          }
      }
      break;
    case 'C': // Cookie
      {
        int cookie_id;
        double x, y;
        log_file >> cookie_id >> x >> y;
        Cookie* cookiep = new Cookie( Vector2D(x,y), 0.0, cookie_id);
        object_lists[COOKIE].insert_last( cookiep );
      }
      break;
    case 'M': // Mine
      {
        int mine_id;
        double x, y;
        log_file >> mine_id >> x >> y;
        Mine* minep = new Mine( Vector2D(x,y), 0.0, mine_id);
        object_lists[MINE].insert_last( minep );
      }
      break;
    case 'S': // Shot
      {
        int shot_id;
        double x, y, dx, dy;
        log_file >> shot_id >> x >> y >> dx >> dy;
        Shot* shotp = new Shot( Vector2D(x,y), Vector2D(dx,dy), 0, shot_id );
        object_lists[SHOT].insert_last( shotp );
      }
      break;
    case 'D': // Die
      {
        char object_type = '?';
        int object_id;
        log_file.get( object_type );
        log_file >> object_id;
        switch( object_type )
          {
          case 'R':
            {
              double points_received;
              log_file >> points_received;
              // robot_die( object_id, points_received );
              ListIterator<Shape> li;             
              find_object_by_id( object_lists[ROBOT], li, object_id );
              if( !li.ok() ) 
                Error(true, "Dying robot not in list", "ArenaReplay::parse_log_line");
              ((Robot*)li())->die();
            }
            break;
          case 'C':
            {
              ListIterator<Shape> li;             
              find_object_by_id( object_lists[COOKIE], li, object_id );
              if( !li.ok() ) 
                Error(true, "Cookie not in list", "ArenaReplay::parse_log_line");
              ((Cookie*)li())->die();
              object_lists[COOKIE].remove(li);
            }
            break;
          case 'M':
            {
              ListIterator<Shape> li;
              find_object_by_id( object_lists[MINE], li, object_id );
              if( !li.ok() ) 
                Error(true, "Mine not in list", "ArenaReplay::parse_log_line");
              ((Mine*)li())->die();
              object_lists[MINE].remove(li);
            }
            break;
          case 'S':
            {
              ListIterator<Shape> li;
              find_object_by_id( object_lists[SHOT], li, object_id );
              if( !li.ok() )
                Error(true, "Shot not in list", "ArenaReplay::parse_log_line");
              ((Shot*)li())->die();
              object_lists[SHOT].remove(li);
            }
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
        delete_lists(false, false, false, false, false);
        reset_timer();
        next_check_time = 0.0;
        int sequence, game;
        log_file >> sequence >> game;
        arena_scale = the_opts.get_d(OPTION_ARENA_SCALE);
        arena_succession = 1;
        set_state( BEFORE_GAME_START );
      }
      break;
    case 'H': // Header
      {
        int gps, rps, seq, robots;
        log_file >> gps >> rps >> seq >> robots;
        // init( gps, rps, seq, robots );
      }
      break;
    case 'L': // List of robot properties
      {
        int robot_id;
        char robot_colour[7];
        char name[200];
        log_file >> robot_id >> ws;
        log_file.get( robot_colour, 7, ' ');
        long int col = str2hex( (String)robot_colour );
        log_file.get( name, 200, '\n' );
        Robot* robotp = new Robot( robot_id, col, (String)name );
        object_lists[ROBOT].insert_last(robotp); // array bättre?
      }
      break;
    case 'A': // Arena file line
      {
        parse_arena_line( log_file, arena_scale, arena_succession );
      }
      break;
    case 'O': // Option
      {
        char temp;
        char label[200];
        log_file.get( label, 200, ':');
        log_file.get( temp );
        option_return_t opt = the_opts.get_option_from_string( String( label ) );
        switch( opt.datatype )
          {
          case ENTRY_INT:
          case ENTRY_HEX:
            {
              long val;
              log_file >> val;
              //              cerr << label << ": " << val << endl;
              the_opts.get_all_long_options()[opt.option_number].value = val;
            }
            break;
          case ENTRY_DOUBLE:
            {
              double val;
              log_file >> val;
              //              cerr << label << ": " << val << endl;
              the_opts.get_all_double_options()[opt.option_number].value = val;
            }
            break;
          case ENTRY_CHAR:
            {
              char val[400];
              log_file.get( val, 400, '\n' );
              //              cerr << label << ": " << val << endl;
              the_opts.get_all_string_options()[opt.option_number].value = val;
            }
            break;
          case ENTRY_BOOL:
            break;
          default:
            Error( true, "Unknown datatype", "ArenaReplay::parse_log_line" );
            break;
          }
      }
      break;

    case '?':
    default:
      Error( false, "Unrecognized first letter in logfile: " + 
             String(first_letter), "ArenaReplay::parse_log_line" );
      break;
    }

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
