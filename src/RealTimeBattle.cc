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
#include "../config.h"
#endif

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include <sys/types.h>
#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif
//#include <sys/wait.h>

#include <errno.h>
#ifdef HAVE_GETOPT_H
# include <getopt.h>
#else
# include "getopt.h"
#endif

#include <iostream.h>

#ifdef FLOATINGPOINT_H
#include <floatingpoint.h>
#endif


#include "Gui.h"
#include "MovingObject.h"
#include "Options.h"
#include "Vector2D.h"


#ifndef WAIT_ANY
#define WAIT_ANY (pid_t)-1
#endif

class Options the_opts;
class Arena the_arena;
#ifndef NO_GRAPHICS
class Gui the_gui;
#endif

bool no_graphics;

void 
print_help_message()
{
  cout << endl;
  cout << " Usage: RealTimeBattle [options] " << endl << endl;
  cout << " Options: --debug_mode,              -d   debug mode" << endl;
  cout << "          --normal_mode,             -n   normal mode (default)" << endl;
  cout << "          --competition_mode,        -c   competition mode" << endl ;
  cout << "          --no_graphics,             -g   no graphics will be displayed" << endl ;
  cout << "          --option_file [file],      -o   selects option-file " << endl;
  cout << "                                          (default: $HOME/.rtbrc)"  << endl;
  cout << "          --log_file [file],         -l   make log file, if 'file' is '-'" << endl;
  cout << "                                          the log is send to STDOUT" << endl;
  cout << "          --tournament_file [file],  -t   specify a tournament file to" << endl;
  cout << "                                          autostart a tournament" << endl;
  cout << "          --statistics_file [file],  -s   file to print the statistics to" << endl;
  cout << "                                          when autostarting" << endl;
  cout << endl;
  cout << "          --help,                    -h   prints this message" << endl;
  cout << "          --version,                 -v   prints the version number" << endl;
  cout << endl;
}

gint
update_function(gpointer data)
{  
  gint res = 0;
  res = the_arena.timeout_function();

  return res;
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
            Error(true, "waidpid failed", "RealTimeBattle.cc:sigchld_handler");
          break;
        }
      if (pid == 0)
        break;
    }
  //  if( signum == SIGCHLD ) cerr << "Sigchld caught!" << endl;
  //  if( signum == SIGPIPE ) cerr << "Sigpipe caught!" << endl;
  signal(signum, sig_handler);
}

void
parse_command_line(int argc, char **argv)
{
  int version_flag=false, help_flag=false, graphics_flag=true;
  int game_mode=Arena::NORMAL_MODE;
  int c;
  String option_file("");
  String statistics_file("");
  String log_file("");
  String tournament_file("");

  static struct option long_options[] =
  {
    //option, argument?, flag, value
    {"version", 0, &version_flag, true},
    {"help", 0, &help_flag, true},

    {"debug_mode", 0, &game_mode, Arena::DEBUG_MODE},
    {"normal_mode", 0, &game_mode, Arena::NORMAL_MODE},
    {"competition_mode", 0, &game_mode, Arena::COMPETITION_MODE},

    {"option_file", 1, 0, 0},
    {"log_file", 1, 0, 0},
    {"statistics_file", 1, 0, 0},
    {"tournament_file", 1, 0, 0},

    {"no_graphics", 0, &graphics_flag, false},

    {0, 0, 0, 0}
  };

  for(;;)
    {
      int option_index = 0;
     
      c = getopt_long (argc, argv, "dncvho:l:s:t:g", long_options, &option_index);

      /* Detect the end of the options. */
      if (c == -1)
        break;
     
      switch (c)
        {

        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          
          switch (option_index)
            {
            case 5: 
              option_file = (String)optarg;
              break;
            case 6:
              log_file = (String)optarg;
              break;
            case 7:
              statistics_file = (String)optarg;
              break;
            case 8:
              tournament_file = (String)optarg;
              break;
            default:
              cerr << "Bad error in parse_command_line, this shouldn't happen" << endl;
              exit( EXIT_FAILURE );
            }
          break;


        case 'd':
          game_mode = Arena::DEBUG_MODE;
          break;

        case 'n':
          game_mode = Arena::NORMAL_MODE;
          break;

        case 'c':
          game_mode = Arena::COMPETITION_MODE;
          break;

        case 'v':
          version_flag = true;
          break;

        case 'h':
          help_flag = true;
          break;

        case 'o':
          option_file = (String)optarg;
          break;

        case 'l':
          log_file = (String)optarg;
          break;

        case 's':
          statistics_file = (String)optarg;
          break;

        case 't':
          tournament_file = (String)optarg;
          break;

        case 'g':
          graphics_flag = false;
          break;

        default:
          print_help_message();
          exit( EXIT_FAILURE );
        }
    }

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

  if( option_file == "" )
    {
      the_opts.get_options_from_rtbrc();
      option_file = ".rtbrc";
    }
  else
    the_opts.read_options_file(option_file,true);

  the_arena.set_game_mode((Arena::game_mode_t)game_mode);
  no_graphics = !graphics_flag;
  the_arena.set_filenames(log_file, statistics_file, tournament_file, option_file);
}


int 
main ( int argc, char* argv[] )
{

#ifdef FLOATINGPOINT_H
  fpsetmask ( ~ (FP_X_INV | FP_X_DZ | FP_X_IMP | FP_X_OFL | FP_X_UFL | FP_X_DNML) );
  //fpsetmask ( ~ (FP_X_INV | FP_X_DZ | FP_X_IMP) );
#endif

  gtk_init (&argc, &argv);

  parse_command_line(argc, argv);

  gint timeout_tag;

  srand(time(0));

  signal(SIGCHLD, sig_handler);
  signal(SIGPIPE, sig_handler);


#ifndef NO_GRAPHICS
  if( !no_graphics )
    {
      the_arena.set_colours();
      the_gui.setup_control_window();
    }
#endif
      
  timeout_tag = gtk_timeout_add( 40, GtkFunction(update_function), (gpointer) NULL);

  gtk_main();

  return EXIT_SUCCESS;
}
