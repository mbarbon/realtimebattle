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

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#else
# include "getopt.h"
#endif
#include <stdlib.h>

#include <pthread.h>

#include "ArenaController.h"
#include "ArenaRealTime.h"
#include "ArenaReplay.h"
#include "Options.h"
#include "IntlDefs.h"
#include "GuiInterface.h"
#include "Structs.h"

extern class Options the_opts;
extern bool no_graphics;

// TODO: These should probably be moved to a better place.


// ArenaController constructor

ArenaController::ArenaController()
{
  started = false;

  option_filename = "";
  statistics_filename = "";
  log_filename = "";
  tournament_filename = "";
  message_filename = "";
  replay_filename = "";

  game_mode = NORMAL_MODE;
  debug_level = 1;
  auto_start_and_end = false;
}

ArenaController::~ArenaController()
{
  if( started ) close_arena();
}

int
ArenaController::rtb_main( int argc, char** argv )
{
  if( tournament_filename != "" )
    start_realtime_arena();
  else if( replay_filename != "" )
    start_replay_arena();

  GIInit( argc, argv );

  gi = new GuiInterface;

  return EXIT_SUCCESS; // TODO: what should be returned here?
}

void
ArenaController::start_realtime_arena()
{
  if( started ) close_arena();

  my_arena = new ArenaRealTime();
  
  started = true;
  realtime = true;
}

void
ArenaController::start_replay_arena()
{
  if( started ) close_arena();

  my_arena = new ArenaReplay();
  
  started = true;
  realtime = false;
}

void
ArenaController::close_arena()
{
  if( started )
    {
      delete my_arena;
    }

  started = false;
}

void
ArenaController::parse_command_line( int argc, char** argv )
{
  int version_flag=false, help_flag=false, graphics_flag=true;
  int c;

  extern char* optarg;
  extern int optind;

  static struct option long_options[] =
  {
    //option, argument?, flag, value
    {"version", 0, &version_flag, true},
    {"help", 0, &help_flag, true},

    {"debug_mode", 0, &game_mode, DEBUG_MODE},
    {"debug_level", 1, 0, 0},
    {"normal_mode", 0, &game_mode, NORMAL_MODE},
    {"competition_mode", 0, &game_mode, COMPETITION_MODE},
    {"option_file", 1, 0, 0},
    {"log_file", 1, 0, 0},
    {"statistics_file", 1, 0, 0},
    {"tournament_file", 1, 0, 0},
    {"message_file", 1, 0, 0},
    {"replay", 1, 0, 0},

    {"no_graphics", 0, &graphics_flag, false},

    {0, 0, 0, 0}
  };

  for(;;)
    {
      int option_index = 0;
     
      c = getopt_long( argc, argv, "dncD:vho:l:s:t:m:r:g",
                       long_options, &option_index );

      // Detect the end of the options.
      if (c == -1)
        break;
     
      switch (c)
        {

        case 0:
          // If this option set a flag, do nothing else now.
          // This is not the case for debug_mode
          if( long_options[option_index].flag != 0 &&
              option_index != 2 )
            break;
          
          switch( option_index )
            {
            case 3:
              debug_level = str2int( optarg );
              game_mode = DEBUG_MODE;
              break;
            case 6: 
              option_filename = (String)optarg;
              break;
            case 7:
              log_filename = (String)optarg;
              break;
            case 8:
              statistics_filename = (String)optarg;
              break;
            case 9:
              tournament_filename = (String)optarg;
              auto_start_and_end = true;
              break;
            case 10:
              message_filename = (String)optarg;
              break;
            case 11:
              replay_filename = (String)optarg;
              break;
            default:
              Error( true, "Bad error, this shouldn't happen",
                     "RealTimeBattle.cc:parse_command_line" );
              exit( EXIT_FAILURE );
            }
          break;


        case 'd':
          game_mode = DEBUG_MODE;
          break;

        case 'D':
          debug_level = str2int( optarg );
          game_mode = DEBUG_MODE;
          break;

        case 'n':
          game_mode = NORMAL_MODE;
          break;

        case 'c':
          game_mode = COMPETITION_MODE;
          break;

        case 'v':
          version_flag = true;
          break;

        case 'h':
          help_flag = true;
          break;

        case 'o':
          option_filename = (String)optarg;
          break;

        case 'l':
          log_filename = (String)optarg;
          break;

        case 's':
          statistics_filename = (String)optarg;
          break;

        case 't':
          tournament_filename = (String)optarg;
          auto_start_and_end = true;
          break;

        case 'm':
          message_filename = (String)optarg;
          break;

        case 'r':
          replay_filename = (String)optarg;
          break;

        case 'g':
          graphics_flag = false;
          break;

        default:
          print_help_message();
          exit( EXIT_FAILURE );
        }
    }

  if( debug_level > max_debug_level )
    debug_level = 5;
  if( debug_level < 0 )
    debug_level = 0;

  if(optind != argc) 
    {
      print_help_message();
      exit( EXIT_FAILURE );
    }

  if( version_flag )
      cout << "RealTimeBattle " << VERSION << endl;

  if( help_flag )
    print_help_message();

  if( help_flag || version_flag ) exit( EXIT_SUCCESS );

  if( option_filename == "" )
    {
      the_opts.get_options_from_rtbrc();
      option_filename = ".rtbrc";
    }
  else
    the_opts.read_options_file( option_filename,true );

  auto_start_and_end
    = ( ( tournament_filename != "" ) ||
        ( replay_filename == "-" ) );

  no_graphics = !graphics_flag;
}


void 
ArenaController::print_help_message()
{
  cout << endl;
  cout << _(" Usage: RealTimeBattle [options] ") << endl << endl;
  cout << _(" Options:") << endl;
  cout << _("    --debug_mode,                -d   debug mode") << endl;
  cout << _("    --debug_level [0-5],         -D   sets the initial debug level. Implies -d") << endl;
  cout << _("    --normal_mode,               -n   normal mode (default)") << endl;
  cout << _("    --competition_mode,          -c   competition mode") << endl ;
  cout << endl;
  cout << _("    --no_graphics,               -g   no graphics will be displayed") << endl ;
  cout << _("    --option_file [file],        -o   selects option-file (default: $HOME/.rtbrc)")  << endl;
  cout << endl;
  cout << _("    --log_file [file],           -l   make log file, if 'file' is '-'\n"
            "                                      the log is sent to STDOUT") << endl;
  cout << endl;
  cout << _("    --tournament_file [file],    -t   specify a tournament file to\n"
            "                                      autostart a tournament") << endl;
  cout << endl;
  cout << _("    --statistics_file [file],    -s   file to print the statistics to\n"
            "                                       when autostarting") << endl;
  cout << endl;
  cout << _("    --message_file [file],       -m   redirect messages to 'file'.\n"
            "                                      With '-' as 'file', STDOUT is used.\n"
            "                                      If both log and messages are sent\n"
            "                                      to STDOUT, '-m' will be ignored") << endl;
  cout << endl;
  cout << _("    --replay [file]              -r   a log file to replay.\n"
            "                                      if '-' is specified as file,\n"
            "                                      input is taken from STDIN") << endl;
  cout << endl;
  cout << _("    --help,                      -h   prints this message") << endl;
  cout << _("    --version,                   -v   prints the version number") << endl;
  cout << endl;
}
