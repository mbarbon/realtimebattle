/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2001  Erik Ouchterlony and Ragnar Ouchterlony

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
# include <config.h>
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

#include <list>

#include "IntlDefs.h"
//#include "OptionHandler.h"

#include "Arena.h"
#include "Various.h"
#include "ArenaController.h"
#include "EventHandler.h"

#include "GuiInterface.h"

#include "Gadgets/Gadget.h"

#ifndef WAIT_ANY
#define WAIT_ANY (pid_t)-1
#endif

//class ArenaRealTime the_arena;
class ArenaController the_arena_controller;
class EventHandler the_eventhandler;
class Arena* the_arenap;

list<unsigned int> GI_exit_list;
pthread_mutex_t the_mutex;

int Gadget::last_id_used = 0;

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

int 
main ( int argc, char* argv[] )
{

#ifdef FLOATINGPOINT_H
  fpsetmask ( ~ (FP_X_INV | FP_X_DZ | FP_X_IMP | FP_X_OFL | FP_X_UFL | FP_X_DNML) );
  //fpsetmask ( ~ (FP_X_INV | FP_X_DZ | FP_X_IMP) );
#endif

#ifdef HAVE_LOCALE_H
  setlocale( LC_MESSAGES, "" );
  setlocale( LC_NUMERIC, "POSIX" );
#endif
#ifdef RTB_LOCALEDIR
  bindtextdomain( "RealTimeBattle", RTB_LOCALEDIR );
#else
  bindtextdomain( "RealTimeBattle", "/usr/local/share/locale" );
#endif
  textdomain( "RealTimeBattle" );

  the_arena_controller.init( argc, argv );
  pthread_mutex_init( &the_mutex, NULL );

  //  parse_command_line(argc, argv);


  signal(SIGCHLD, sig_handler);
  signal(SIGPIPE, sig_handler);
  signal(SIGFPE, sigfpe_handler);
  
  the_eventhandler.main_loop();

  pthread_mutex_destroy( &the_mutex );

  return EXIT_SUCCESS;
}
