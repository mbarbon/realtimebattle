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
# include <config.h>
#endif

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

#define NO_GRAPHICS

#include <unistd.h>
#include <fstream.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>

#include "Socklib.h"
#include "SimpleProcess.h"

const debug_level = 0;

void debug_msg(const char* by, const char* msg, const int lvl);

void usage(int err_code)
{
  cout << endl
       << "  Usage:  RTBClient Robot server " << endl
       << endl;

  exit( err_code );
}

int 
main ( int argc, char* argv[] )
{
  const int port = 19074;
  const int dbug = 1;

  char buffer[80];
  char server[80];

  if( argc != 3  ) usage( EXIT_FAILURE);

  strncpy( server, argv[2], sizeof(server) - 1 );
  server[80] = '\0';


  int socket_fd = connectto( server, port, dbug );

  if( socket_fd == -1 )
    exit(EXIT_FAILURE);



  signal(SIGPIPE,  SIG_IGN);





  ifstream in_socket (socket_fd);
  ofstream out_socket(socket_fd);
        
  string robotname = string(argv[1]);

  out_socket << robotname << endl; // send robot name;

  // Make socket non_blocking

  int pd_flags;
  if( (pd_flags = fcntl(socket_fd, F_GETFL, 0)) == -1 ) 
    {
      cerr << "Client: couldn't get pd_flags for socket: ";
      perror( NULL );
    }

  pd_flags |= O_NONBLOCK;
  if( fcntl(socket_fd, F_SETFL, pd_flags) == -1 ) 
    {
      cerr << "Client: couldn't change pd_flags for socket: ";
      perror( NULL );
    }


  SimpleProcess robot_process( robotname );





  // Main loop

  fd_set pipe_and_socket;

  struct timeval current_time;
  struct timeval time_to_wait;

  int last_ping_time;

  while( true )
    {
      time_to_wait.tv_sec = 0;
      time_to_wait.tv_usec = 300000;


      
      FD_ZERO( &pipe_and_socket );
      FD_SET( socket_fd, &pipe_and_socket );
      
      if( robot_process.is_running() )
        FD_SET( robot_process.in_pipe, &pipe_and_socket );


      select(FD_SETSIZE, &pipe_and_socket, NULL, NULL, &time_to_wait);



      if( FD_ISSET( socket_fd, &pipe_and_socket) )
        {      
          in_socket >> ws;
          in_socket.clear();
          in_socket.get(buffer, 80, '\n');

          if( in_socket.fail() )
            {
              cerr << "Reading in_socket failed!" << endl;
              goaway(0);
            }

          debug_msg( "<<<s", buffer, 4);

          if( buffer[0] == '@' )  // special message
            {
              switch( buffer[1] )
                {
                case 'R':  // Run process
                  if( robot_process.is_running() )
                    robot_process.kill_forcefully();
                  robot_process.start();
                  break;

                case 'K':  // Kill process
                  robot_process.kill_forcefully();
                  break;

                case 'B':  // Blocking
                  robot_process.set_non_blocking_state( false );
                  break;

                case 'N':  // Non-Blocking
                  robot_process.set_non_blocking_state( true );
                  break;

                case 'S':   // Signal
                  {
                    int signr = (int)( buffer[2] - '0' );
                    if( buffer[3] <= '9' && buffer[3] >= '0' )
                      signr = 10*signr + (int)( buffer[3] - '0' );
                    
                    robot_process.send_signal(signr);
                  }
                  break;

                case ' ':   // Just checking if messages get through
                  break;
                }
              
            }
          else if( robot_process.is_running() )
            {
              *robot_process.opipe_streamp << buffer << endl;

              debug_msg( ">>>p", buffer, 5);

              if( robot_process.opipe_streamp->fail() )
                {
                  cerr << "Writing to opipe failed!" << endl;
                  //goaway(0);
                }
            }
          //          cout << "!" << flush;
        }

      if( robot_process.is_running() && 
          FD_ISSET( robot_process.in_pipe, &pipe_and_socket) )
        {      
          *robot_process.ipipe_streamp >> ws;
          robot_process.ipipe_streamp->clear();
          robot_process.ipipe_streamp->peek();
          while( !robot_process.ipipe_streamp->eof() )
            {
              robot_process.ipipe_streamp->get(buffer, 80, '\n');
              
              if( robot_process.ipipe_streamp->fail() )
                {
                  cerr << "Reading ipipe failed!" << endl;
                  goaway(0);
                }

              debug_msg( "<<<p", buffer, 4);

              out_socket << buffer << endl;

              debug_msg( ">>>s", buffer, 5);

              //              cout << "%" << flush;  

              if( out_socket.fail() )
                {
                  cerr << "Writing to out_socket failed!" << endl;
                  goaway(0);
                }


              *robot_process.ipipe_streamp >> ws;
              robot_process.ipipe_streamp->clear();
              robot_process.ipipe_streamp->peek();
            }

          //          robot_process.ipipe_streamp->clear();
        }



      gettimeofday(&current_time, NULL);

      if( current_time.tv_sec != last_ping_time )
        {
          out_socket << "@" << endl;
          if( out_socket.fail() )
            {
              cerr << "Writing to out_socket failed!" << endl;
              goaway(0);
            }

          last_ping_time = current_time.tv_sec;
        }

      //      cout << "." << flush;      
    }
}

void
debug_msg(const char* by, const char* msg, const int lvl)
{
  if( debug_level >= lvl )
    {
      struct timeval current_time;
      gettimeofday(&current_time, NULL);
      cout << current_time.tv_sec << "." << current_time.tv_usec << ": ";
      cout << "client " << by << ": " << msg << endl;
    }
}
