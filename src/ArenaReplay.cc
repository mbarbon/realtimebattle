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
#include "ArenaController.h"
#include "Various.h"
#include "Options.h"
#include "Extras.h"
#include "Shot.h"
#include "MessageWindow.h"
#include "ArenaWindow.h"
#include "ScoreWindow.h"
#include "ControlWindow.h"
#include "Robot.h"

extern class ControlWindow* controlwindow_p;

ArenaReplay::ArenaReplay()
{
  reset_timer();
  speed = PLAY;
  state = NOT_STARTED;
  current_replay_time = 0.0;
  game_position_in_log = NULL;
  time_position_in_log = NULL;

  set_game_mode( (ArenaBase::game_mode_t)the_arena_controller.game_mode );
  set_filenames( the_arena_controller.replay_filename,
                 the_arena_controller.message_filename,
                 the_arena_controller.statistics_filename,
                 the_arena_controller.option_filename );
}

ArenaReplay::~ArenaReplay()
{
  if( game_position_in_log != NULL )
    {
      for( int i=0; i<sequences_in_tournament; i++ )
        delete [] game_position_in_log[i];
      delete [] game_position_in_log;
    }

  if( time_position_in_log != NULL )
    delete [] time_position_in_log;

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
      {
#ifndef NO_GRAPHICS
        int old_total = (int)current_replay_time;
#endif NO_GRAPHICS

        parse_this_interval();

#ifndef NO_GRAPHICS
        if((int)current_replay_time > old_total && !no_graphics)
          {
            the_gui.get_scorewindow_p()->set_window_title();
            if( !log_from_stdin )
              controlwindow_p->set_progress_time( current_replay_time );
          }
#endif
      }
      break;

    case BEFORE_GAME_START:
      parse_this_interval();
#ifndef NO_GRAPHICS
      if( !no_graphics )
        {
          if( controlwindow_p->get_displayed() != ControlWindow::REPLAY_WIDGETS &&
              !log_from_stdin )
            controlwindow_p->display_replay_widgets();
          the_gui.get_arenawindow_p()->drawing_area_scale_changed();      
          the_gui.get_arenawindow_p()->draw_everything();      
          the_gui.get_scorewindow_p()->add_robots();
          print_message
            ( "RealTimeBattle", (String)"Game " + String( game_nr )
              + " of sequence " + String( sequence_nr ) );
        }
#endif NO_GRAPHICS
      set_state( GAME_IN_PROGRESS );
      break;

    case FINISHED:
      if( the_arena_controller.auto_start_and_end )
        {
          if( statistics_file_name != "" )
            save_statistics_to_file( statistics_file_name );

          Quit();
        }
      break;

    default:
      break;
    }
  return true;
}

void
ArenaReplay::parse_this_interval()
{
  if( speed != REWIND )
    while( !log_file.eof() && total_time >= current_replay_time )
      {
        parse_log_line();
      }
  else
    {
      //      search_backwards( "T" );
    }
  
  if( log_file.eof() )
    {
      set_state( FINISHED );
#ifndef NO_GRAPHICS
      if( !no_graphics )
        {
          controlwindow_p->remove_replay_widgets();
          the_gui.close_arenawindow();
        }
#endif
    }
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

  if( !log_from_stdin )
    make_statistics_from_file();

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
  //  cerr << first_letter;
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
        last_replay_time = current_replay_time;
        log_file >> current_replay_time;
        //        cerr << current_replay_time << "  " << total_time << endl;

        move_shots_no_check( current_replay_time - last_replay_time );

#ifndef NO_GRAPHICS
        if( !no_graphics )
          the_gui.get_arenawindow_p()->draw_moving_objects( true );
#endif NO_GRAPHICS

        // check if robots have died and set their points
        if( robots_killed_this_round > 0 )
          {
            ListIterator<Shape> li;
            Robot* robotp;
            for( object_lists[ROBOT].first(li); li.ok(); li++ )
              {                
                robotp = (Robot*)li();
                if( robotp->is_alive() )
                  {
                    robotp->add_points(robots_killed_this_round);
#ifndef NO_GRAPHICS
                    if( robots_left < 15 && !no_graphics ) 
                      robotp->display_score();
#endif
                  }
              }
            robots_left -= robots_killed_this_round; 
            robots_killed_this_round = 0;
          }
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
        if( li.ok() ) 
          {
            Robot* robotp = (Robot*)li();
            print_message( robotp->get_robot_name(), (String)message );
          }
        // else: robot sent a message before first game of sequences.
        // Robot data is not yet known to us, thus ignore!
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
              int pos_this_game;
              log_file >> points_received >> pos_this_game;
              ListIterator<Shape> li;             
              find_object_by_id( object_lists[ROBOT], li, object_id );
              if( !li.ok() ) 
                Error(true, "Dying robot not in list", "ArenaReplay::parse_log_line");
              Robot* robotp = (Robot*) li();
              robotp->die();
              robots_killed_this_round++;
              robotp->set_stats( points_received, pos_this_game,
                                 current_replay_time);
              robotp->change_energy( -robotp->get_energy() );
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
        current_replay_time = 0;
        robots_killed_this_round = 0;
        robots_left = robots_per_game;

        log_file >> sequence_nr >> game_nr;

        ListIterator<Shape> li;
        for( object_lists[ROBOT].first(li); li.ok(); li++ )
          ((Robot*)li())->
            set_values_before_game(Vector2D(infinity,infinity), 0.0);

        arena_scale = the_opts.get_d(OPTION_ARENA_SCALE);
        arena_succession = 1;
        set_state( BEFORE_GAME_START );
      }
      break;
    case 'H': // Header
      {
        log_file >> games_per_sequence >> robots_per_game 
                 >> sequences_in_tournament >> number_of_robots;
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
        object_lists[ROBOT].insert_last(robotp); // array better?
        all_robots_in_tournament.insert_last(robotp); // used by statistics
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
      char buffer[400];
      log_file.get( buffer, 400, '\n' );
      log_file >> ws;
      log_file.clear();
      break;
    }

  return first_letter;
}

void
ArenaReplay::set_filenames( String& replay_fname, String& message_fname,
                            const String& statistics_fname,
                            const String& option_fname )
{
  if( replay_fname != "-" )
    {
      log_file.open( replay_fname.chars() );
      log_from_stdin = false;
    }
  else
    {
      log_file.attach( STDIN_FILENO );
      log_from_stdin = true;
    }

  if( message_fname == "" )
    use_message_file = false;
  else if( message_fname == "-" || message_fname == "STDOUT" )
    {
      use_message_file = true;
      message_file.attach( STDOUT_FILENO );
    }
  else
    {
      use_message_file = true;
      message_file.open( message_fname.chars(), ios::out, 0600 );
      if( !message_file )
        {
          Error( false, "Couldn't open message file. Message file disabled",
                 "ArenaRealTime::set_filenames" );
          use_message_file = false;
        }
      
    }

  statistics_file_name = statistics_fname;
  option_file_name = option_fname;
}

void
ArenaReplay::change_speed( const bool forward, const bool fast )
{
  if( !fast || log_from_stdin )
    speed = PLAY;
  else if( forward )
    speed = FAST_FORWARD;
  else
    speed = REWIND;
}

// Searches the log_file for a line beginning with any of the 
// letters in 'search_letters'.
// The file pointer will be directly after the found letter.
// Returns the letter found, or '?' if none found.
char
ArenaReplay::search_forward( const String& search_letters )
{
  if( log_from_stdin )
    return '?';

  char letter;
  char buffer[400];
  

  while( !log_file.eof() )
    {
      log_file.clear();

      log_file >> letter;   // check first letter of line
      if( search_letters.find( letter ) != -1 )
        {
          return letter;
        }

      log_file >> ws;
      log_file.get( buffer, 400, '\n' );  // go to next line
    }
  
  return '?';
}

// Similar to the previous function, but the argument is a list
// of strings (string lengths between 1 and 16) to serach for.
// Returns the string found, or the empty string if none found.
String
ArenaReplay::search_forward( const List<String>& search_strings )
{
  if( log_from_stdin ) return "";

  bool found = false;
  char buffer[400];
  ListIterator<String> li;
  int i;
  int read_letters;
  char letter[16];

 while( !log_file.eof() )
    {
      log_file.clear();

      read_letters = 0;
      for( search_strings.first(li); li.ok() && !found; li++ )
        {
          found = true; 
          for( i=0; i < li()->get_length() && found; i++ )
            {
              if( read_letters < i+1 )
                {
                  log_file >> letter[read_letters];
                  read_letters++;
                }
              if( (*li())[i] != letter[i] ) found = false;
            }

          if( found ) return *li();
        }      
      
      log_file.get( buffer, 400, '\n' );  // go to next line
    }
  
  return "";
}


String
ArenaReplay::search_backwards( const String& search_letters )
{
  if( log_from_stdin )
    return "";

  char letter='?';
  while( search_letters.find( letter ) == -1 && log_file.tellg() != 0 )
    {
      beginning_of_prev_line();
      letter = log_file.peek();
    }

  if( log_file.tellg() == 0 )
    return "";
  else
    {
      char buffer[400];
      log_file.get( buffer, 400, '\n' );
      beginning_of_current_line();
      return (String) buffer;
    }

  return "";
}

void
ArenaReplay::beginning_of_prev_line()
{
  char letter='?';
  bool already_found_one_line = false;
  while( ( letter != '\n' || !already_found_one_line ) &&
         log_file.tellg() != 0 )
    {
      log_file.seekg( -1, ios::cur );
      letter = log_file.peek();
      if( letter == '\n' && !already_found_one_line )
        {
          already_found_one_line = true;
          letter = '?';
        }
    }
  log_file.get( letter );
}

void
ArenaReplay::beginning_of_current_line()
{
  char letter='?';
  while( letter != '\n' && log_file.tellg() != 0 )
    {
      log_file.seekg( -1, ios::cur );
      letter = log_file.peek();
    }
  log_file.get( letter );
}

void
ArenaReplay::make_statistics_from_file()
{
  List<String> str_list;
  str_list.insert_last( new String("DR") );
  str_list.insert_last( new String("L") );
  str_list.insert_last( new String("G") );
  str_list.insert_last( new String("T") );
  str_list.insert_last( new String("H") );

  ListIterator<Shape> li;             
  double points_received;
  int pos_this_game, object_id;
  char buffer[400];
  String letters;

  streampos old_pos = log_file.tellg();

  cout << "make_statistics_from_file" << endl;

  while(  (letters = search_forward( str_list ))  != "" )
    {
      switch( letters[0] )
        {
        case 'D':
          log_file >> object_id >> points_received >> pos_this_game;
            
          find_object_by_id( object_lists[ROBOT], li, object_id );

          if( !li.ok() ) Error(true, "Dying robot not in list", 
                               "ArenaReplay::make_statistics_from_file");
          ((Robot*)li())->set_stats( points_received, pos_this_game, 
                                     current_replay_time);
          break;
            
        case 'L':
          {
            int robot_id;
            char robot_colour[7];
            char name[200];
            log_file >> robot_id >> ws;
            log_file.get( robot_colour, 7, ' ');
            long int col = str2hex( (String)robot_colour );
            log_file >> ws;
            log_file.get( name, 200, '\n' );
            Robot* robotp = new Robot( robot_id, col, (String)name );
            object_lists[ROBOT].insert_last(robotp); // array better?
            all_robots_in_tournament.insert_last(robotp); // used by statistics
          }
          break;
            
        case 'G':
          log_file >> sequence_nr >> game_nr;
          game_position_in_log[sequence_nr-1][game_nr-1] = log_file.tellg();
          break;
            
        case 'T':
          log_file >> current_replay_time;
          break;

        case 'H':
          if( game_position_in_log != NULL )
            {
              log_file.seekg( old_pos );
              return;
            }
              
          log_file >> games_per_sequence >> robots_per_game 
                   >> sequences_in_tournament >> number_of_robots;
              
          game_position_in_log = new streampos*[sequences_in_tournament];

          for( int i=0; i<sequences_in_tournament; i++ )
            game_position_in_log[i] = new streampos[games_per_sequence];
              
          break;
          
        default:
          Error(true, "Wrong log line found", 
                "ArenaReplay::make_statistics_from_file");
          break;
        }             
      log_file.get( buffer, 400, '\n' );
    }

  log_file.seekg( old_pos );
  log_file.clear();
}

void
ArenaReplay::get_time_positions_in_game()
{  
  String letter_list = "TGH";
  char letter;
  char buffer[400];
  int time_pos_index = 0;
  int max_time_infos = 16384;
  float cur_time;
  if( time_position_in_log != NULL ) delete [] time_position_in_log;

  time_position_in_log = new time_pos_info_t[max_time_infos];

  streampos old_pos = log_file.tellg();

  cout << "get_time_positions_in_game" << endl;

  while(  (letter = search_forward( letter_list ))  != '?' )
    {
      switch( letter )
        {
        case 'T':
          log_file >> cur_time;
          time_position_in_log[time_pos_index].pos = log_file.tellg();
          time_position_in_log[time_pos_index].time = cur_time;
              
          time_pos_index++;
          if( time_pos_index >= max_time_infos )
            Error(false, "Too many time-info entries", 
                  "ArenaReplay::get_time_positions_in_game");

          break;

        case 'G':
        case 'H':
          last_time_info = time_pos_index - 1;
          log_file.seekg( old_pos );
          return;
          break;
          
        default:
          Error(true, "Wrong log line found", 
                "ArenaReplay::get_time_positions_in_game");
          break;
              
        }
      log_file.get( buffer, 400, '\n' );
    }

  log_file.seekg( old_pos );
}
