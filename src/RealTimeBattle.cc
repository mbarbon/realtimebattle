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
#include "Arena.h"
#include "Options.h"
#include "Error.h"
#include "Vector2D.h"
#include "Gui.h"

class Options the_opts;
class Arena the_arena;
class Gui the_gui;

void 
print_help_message()
{
  cout << " Usage: RealTimeBattle [options] " << endl << endl;
  cout << " Options:  -d   debug mode" << endl;
  cout << "           -n   normal mode (default)" << endl;
  cout << "           -c   competition mode" << endl ;
  cout << "           -h   prints this message" << endl;
  cout << "           -v   prints the version number" << endl << endl;
}

gint
update_function(gpointer data)
{  
  gint res;
  try
    {
      res = the_arena.timeout_function();
    }
  catch ( Error the_error )
	 {
		the_error.print_message();
		exit( EXIT_FAILURE );
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

int 
main ( int argc, char* argv[] )
{
  gtk_init (&argc, &argv);

  int option_char;
  while ((option_char = getopt (argc, argv, "dncvh")) != -1)
    {
      switch (option_char)
        {
        case 'd':
          the_arena.set_game_mode(Arena::DEBUG_MODE);
          break;
        case 'n':
          the_arena.set_game_mode(Arena::NORMAL_MODE);
          break;
        case 'c':
          if( !the_arena.get_use_proc() )
            {
              cout << "In competition mode /proc is needed. Changing to normal mode." << endl;
              the_arena.set_game_mode(Arena::NORMAL_MODE);
            }
          else
            the_arena.set_game_mode(Arena::COMPETITION_MODE);
          break;
        case 'v':
          cout << "RealTimeBattle 0.9" << endl;
          return EXIT_SUCCESS;
          break;
        case 'h':
          print_help_message();
          return EXIT_SUCCESS;
        default:
          cerr << "Unknown option: -" << (char)option_char << "." << endl << endl;
          print_help_message();
          return EXIT_FAILURE;
        }
    }

  if(optind != argc) 
    {
      print_help_message();
      return EXIT_FAILURE;
    }
  
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
