/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998  Erik Ouchterlony and Ragnar Ouchterlony

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

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <getopt.h>

#ifdef BSD_FP
#include <floatingpoint.h>
#endif


#include "Gui.h"
#include "MovingObject.h"
#include "Options.h"
#include "Error.h"
#include "Vector2D.h"


#include "../VERSION"

#ifndef WAIT_ANY
#define WAIT_ANY (pid_t)-1
#endif

class Options the_opts;
class Arena the_arena;
class Gui the_gui;

void 
print_help_message()
{
  cout << endl;
  cout << " Usage: RealTimeBattle [options] " << endl << endl;
  cout << " Options: --debug_mode,         -d   debug mode" << endl;
  cout << "          --normal_mode,        -n   normal mode (default)" << endl;
  cout << "          --competition_mode,   -c   competition mode" << endl ;
  cout << "          --option_file [file], -o   selects option-file " << endl;
  cout << "                                     (default: $HOME/.rtbrc)"  << endl;
  cout << endl;
  cout << "          --help,               -h   prints this message" << endl;
  cout << "          --version,            -v   prints the version number" << endl;
  cout << endl;
}

gint
update_function(gpointer data)
{  
  gint res = 0;
  try
    {
      res = the_arena.timeout_function();
    }
  catch ( Error the_error )
	 {
		the_error.print_message();
      the_gui.quit_event();
	 }

  return res;
}

void
sig_handler (int signum)
{
  int pid;
  int status;
  while (1)
    {
      pid = waitpid (WAIT_ANY, &status, WNOHANG);
      if (pid < 0)
        {
          if( errno != ECHILD ) throw Error("waidpid failed", "RealTimeBattle.cc:sigchld_handler");
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
  int version_flag=false, help_flag=false;
  int game_mode=Arena::NORMAL_MODE;
  int c;

  static struct option long_options[] =
  {
    //option, argument?, flag, value
    {"version", 0, &version_flag, true},
    {"help", 0, &help_flag, true},

    {"debug_mode", 0, &game_mode, Arena::DEBUG_MODE},
    {"normal_mode", 0, &game_mode, Arena::NORMAL_MODE},
    {"competition_mode", 0, &game_mode, Arena::COMPETITION_MODE},

    {"option_file", 1, 0, 0},

    {0, 0, 0, 0}
  };

  for(;;)
    {
      int option_index = 0;
     
      c = getopt_long (argc, argv, "dncvho:", long_options, &option_index);

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
              // options_file = (String)optarg; ?
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
          // options_file = (String)optarg; ?    
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

  the_arena.set_game_mode((Arena::game_mode_t)game_mode);
}


int 
main ( int argc, char* argv[] )
{


#ifdef BSD_FP
  fpsetmask ( ~ (FP_X_INV | FP_X_DZ | FP_X_IMP) );
#endif

  gtk_init (&argc, &argv);

  parse_command_line(argc, argv);

  gint timeout_tag;

  the_arena.set_colours();

  srand(time(0));

  signal(SIGCHLD, sig_handler);
  signal(SIGPIPE, sig_handler);
    
  the_gui.setup_control_window();
  
  timeout_tag = gtk_timeout_add( 40, GtkFunction(update_function), (gpointer) NULL);

  gtk_main();

  return EXIT_SUCCESS;
}
