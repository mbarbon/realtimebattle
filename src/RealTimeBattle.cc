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

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <pthread.h>

#include <sys/types.h>
#ifdef TIME_WITH_SYS_TIME 
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

#include <errno.h>
#ifdef HAVE_GETOPT_H
# include <getopt.h>
#else
# include "getopt.h"
#endif

#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif

#include <iostream.h>

#ifdef FLOATINGPOINT_H
#include <floatingpoint.h>
#endif

#include "IntlDefs.h"
#include "Options.h"
#include "ArenaRealTime.h"
#include "ArenaReplay.h"
#include "Various.h"
#include "ArenaController.h"
#include "String.h"

#include "GuiInterface.h"

#ifndef WAIT_ANY
#define WAIT_ANY (pid_t)-1
#endif

class Options the_opts;
//class ArenaRealTime the_arena;
class ArenaController the_arena_controller;

bool no_graphics;

void 
print_help_message()
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

void
update_function(const long int interval_usec)
{
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = interval_usec;
  bool res = true;

  do
    {
      timeout.tv_sec = 0;
      timeout.tv_usec = interval_usec;
      select(FD_SETSIZE, NULL, NULL, NULL, &timeout);
      if( the_arena_controller.is_started() )
        res = the_arena.timeout_function();
    } 
  while( res );

}

RETSIGTYPE
sig_handler (int signum)
{
  int pid;
  int status;
  while (1)
    {
      pid = waitpid (WAIT_ANY, &status, WNOHANG);
      if (pid < 0)
        {
          if( errno != ECHILD ) 
            Error(true, "waitpid failed", "RealTimeBattle.cc:sig_handler");
          break;
        }
      if (pid == 0)
        break;
    }
  //  if( signum == SIGCHLD ) cerr << "Sigchld caught!" << endl;
  //  if( signum == SIGPIPE ) cerr << "Sigpipe caught!" << endl;
  signal(signum, sig_handler);
}

RETSIGTYPE
sigfpe_handler(int signum)
{
  signal(signum, sigfpe_handler);
}

void
parse_command_line(int argc, char **argv)
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

    {"debug_mode", 0, &the_arena_controller.game_mode,
     ArenaBase::DEBUG_MODE},
    {"debug_level", 1, 0, 0},
    {"normal_mode", 0, &the_arena_controller.game_mode,
     ArenaBase::NORMAL_MODE},
    {"competition_mode", 0, &the_arena_controller.game_mode,
     ArenaBase::COMPETITION_MODE},
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
              the_arena_controller.debug_level = str2int( optarg );
              the_arena_controller.game_mode = ArenaBase::DEBUG_MODE;
              break;
            case 6: 
              the_arena_controller.option_filename = (String)optarg;
              break;
            case 7:
              the_arena_controller.log_filename = (String)optarg;
              break;
            case 8:
              the_arena_controller.statistics_filename = (String)optarg;
              break;
            case 9:
              the_arena_controller.tournament_filename = (String)optarg;
              the_arena_controller.auto_start_and_end = true;
              break;
            case 10:
              the_arena_controller.message_filename = (String)optarg;
              break;
            case 11:
              the_arena_controller.replay_filename = (String)optarg;
              break;
            default:
              Error( true, "Bad error, this shouldn't happen",
                     "RealTimeBattle.cc:parse_command_line" );
              exit( EXIT_FAILURE );
            }
          break;


        case 'd':
          the_arena_controller.game_mode = ArenaBase::DEBUG_MODE;
          break;

        case 'D':
          the_arena_controller.debug_level = str2int( optarg );
          the_arena_controller.game_mode = ArenaBase::DEBUG_MODE;
          break;

        case 'n':
          the_arena_controller.game_mode = ArenaBase::NORMAL_MODE;
          break;

        case 'c':
          the_arena_controller.game_mode = ArenaBase::COMPETITION_MODE;
          break;

        case 'v':
          version_flag = true;
          break;

        case 'h':
          help_flag = true;
          break;

        case 'o':
          the_arena_controller.option_filename = (String)optarg;
          break;

        case 'l':
          the_arena_controller.log_filename = (String)optarg;
          break;

        case 's':
          the_arena_controller.statistics_filename = (String)optarg;
          break;

        case 't':
          the_arena_controller.tournament_filename = (String)optarg;
          the_arena_controller.auto_start_and_end = true;
          break;

        case 'm':
          the_arena_controller.message_filename = (String)optarg;
          break;

        case 'r':
          the_arena_controller.replay_filename = (String)optarg;
          break;

        case 'g':
          graphics_flag = false;
          break;

        default:
          print_help_message();
          exit( EXIT_FAILURE );
        }
    }

  if( the_arena_controller.debug_level > max_debug_level )
    the_arena_controller.debug_level = 5;
  if( the_arena_controller.debug_level < 0 )
    the_arena_controller.debug_level = 0;

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

  if( the_arena_controller.option_filename == "" )
    {
      the_opts.get_options_from_rtbrc();
      the_arena_controller.option_filename = ".rtbrc";
    }
  else
    the_opts.read_options_file( the_arena_controller.option_filename,true );

  the_arena_controller.auto_start_and_end
    = ( ( the_arena_controller.tournament_filename != "" ) ||
        ( the_arena_controller.replay_filename == "-" ) );

  no_graphics = !graphics_flag;
}

#ifndef NO_GRAPHICS
void*
GIMain_temp( void* arg )
{
  int res = GIMain( (ArenaController*) arg );
  // Change this!!!!!
  return NULL;
}
#endif

int 
main ( int argc, char* argv[] )
{

#ifdef FLOATINGPOINT_H
  fpsetmask ( ~ (FP_X_INV | FP_X_DZ | FP_X_IMP | FP_X_OFL | FP_X_UFL | FP_X_DNML) );
  //fpsetmask ( ~ (FP_X_INV | FP_X_DZ | FP_X_IMP) );
#endif

#ifdef HAVE_LOCALE_H
  setlocale( LC_MESSAGES, "" );
#endif
  bindtextdomain( "RealTimeBattle", RTB_LOCALEDIR );
  textdomain( "RealTimeBattle" );

  parse_command_line(argc, argv);

  if( the_arena_controller.tournament_filename != "" )
    the_arena_controller.start_realtime_arena();
  else if( the_arena_controller.replay_filename != "" )
    the_arena_controller.start_replay_arena();

  signal(SIGCHLD, sig_handler);
  signal(SIGPIPE, sig_handler);
  signal(SIGFPE, sigfpe_handler);

#ifndef NO_GRAPHICS
  GIInit( argc, argv );
  pthread_t gui_thread;
  pthread_create(&gui_thread, NULL, GIMain_temp, (void*)&the_arena_controller);
#endif

  update_function( (long int)(the_opts.get_d( OPTION_UPDATE_INTERVAL ) * 1000000.0) );
  
  return EXIT_SUCCESS;
}
