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

#include <unistd.h>
#include <fcntl.h>
#include <fstream.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <fcntl.h>

#include <string>

#include "Socklib.h"

string robotname;
string ififo_name;
string ofifo_name;

volatile bool finish;

const debug_level = 0;

void server(int fd);
void exit_cleanly(int fd);
void debug_msg(const char* by, const char* msg, const int lvl);

void usage(int err_code)
{
  cout << endl
       << "  Usage:  RTBServer [hostname] " << endl
       << endl;

  exit( err_code );
}

int 
main ( int argc, char* argv[] )
{
  const int port = 19074;
  const int uport = 19074;
  const int dbug = 1;
  
  char hostname[80];
  
  if( argc > 2 ) usage( EXIT_FAILURE );

  if( argc == 1 )
    {
      if( gethostname( hostname, sizeof(hostname) ) != 0 )
        {
          cerr << "Error: Couldn't get hostname" << endl;
          usage( EXIT_FAILURE );
        }
    }
  else if( argc == 2 )
    {
      strncpy( hostname, argv[1], sizeof(hostname) - 1 );
      hostname[80] = '\0';
    }

  cout << "Server hostname: " << hostname << endl;

  return   server_init(hostname, port, uport, &server, dbug);

  

}

void
server(int socket_fd)
{  

  signal(SIGALRM, exit_cleanly);
  signal(SIGTERM, exit_cleanly);
  signal(SIGINT,  exit_cleanly);
  //  signal(SIGPIPE, exit_cleanly);


 

  ifstream in_socket (socket_fd);
  ofstream out_socket(socket_fd);

  char buffer[80];

  in_socket.get(buffer, 80, '\n');

  finish = false;

 // Make socket non_blocking

  int pd_flags;
  if( (pd_flags = fcntl(socket_fd, F_GETFL, 0)) == -1 ) 
    {
      cerr << "Server: couldn't get pd_flags for socket: ";
      perror( NULL );
    }

  pd_flags |= O_NONBLOCK;
  if( fcntl(socket_fd, F_SETFL, pd_flags) == -1 ) 
    {
      cerr << "Server: couldn't change pd_flags for socket: ";
      perror( NULL );
    }

  robotname = buffer;

  ififo_name = robotname + ".ofifo";  // The input fifo for the server is
  ofifo_name = robotname + ".ififo";  // the output fifo for rtb.



  // Create the fifos

  // TODO: Better error checking needed !!!

  if (  mkfifo( ififo_name.c_str(), S_IWUSR | S_IRUSR ) != 0 )
    {
      // TODO: If fifo exists find another name!
      cerr << "Server: couldn't create " << ififo_name << ": ";
      perror( NULL );
      ififo_name = "";  // don't remove the fifos when cleaning up
      ofifo_name = "";
      exit_cleanly( SIGALRM );
      return;
    }


  // TODO: We should let RTB create the ofifo, due to the blocking problem.

  if(  mkfifo( ofifo_name.c_str(), S_IWUSR | S_IRUSR ) != 0 )
    {
      cerr << "Server: couldn't create " << ofifo_name << ": ";
      perror( NULL );
      ofifo_name = "";
      exit_cleanly( SIGALRM );
      return;
    }

  // Open the fifos for read/write
  
  int ififo_fd = open( ififo_name.c_str(), O_RDONLY | O_NONBLOCK);

  if( ififo_fd == -1 )
    {
      cerr << "Server: couldn't open " << ififo_name << ": ";
      perror( NULL );
      exit_cleanly( SIGALRM );
      return;
    }


  // The ofifo is blocking, so the program will stop here 
  // until RealTimeBattle (or another program) opens it for reading.
  int ofifo_fd = open( ofifo_name.c_str(), O_WRONLY );

  if( finish ) 
    return;

  if( ofifo_fd == -1 )
    {
      cerr << "Server: couldn't open " << ofifo_name << ": ";
      perror( NULL );
      exit_cleanly( SIGALRM );
      return;
    }

  ofstream ofifo_stream(ofifo_fd);
  ifstream ififo_stream(ififo_fd);


  // Main loop

  fd_set fifo_and_socket;

  struct timeval current_time;
  struct timeval time_to_wait;

  int last_ping_time;

  while( !finish )
    {
      time_to_wait.tv_sec = 1;
      time_to_wait.tv_usec = 0;

      FD_ZERO( &fifo_and_socket );
      FD_SET( socket_fd, &fifo_and_socket );
      FD_SET( ififo_fd, &fifo_and_socket );

      select(FD_SETSIZE, &fifo_and_socket, NULL, NULL, &time_to_wait);

      if( FD_ISSET( socket_fd, &fifo_and_socket) )
        {      
          in_socket >> ws;
          in_socket.clear();
          in_socket.peek();
          while( !in_socket.eof() )
            {
                
              in_socket.get(buffer, 80, '\n');


              if( in_socket.fail() )
                {
                  cerr << "Reading in_socket failed!" << endl;
                  exit_cleanly( SIGALRM );
                }
              
              debug_msg( "<<<s", buffer, 4);

              if( buffer[0] != '@' ) 
                ofifo_stream << buffer << endl;      


              debug_msg( ">>>f", buffer, 5);

              //          cout << "!" << flush;   
              in_socket >> ws;
              in_socket.clear();
              in_socket.peek();
            }
        }

      if( FD_ISSET( ififo_fd, &fifo_and_socket) )
        {      
          ififo_stream >> ws;
          ififo_stream.clear();
          ififo_stream.peek();
          while( !ififo_stream.eof() )
            {
              ififo_stream.get(buffer, 80, '\n');

              if( ififo_stream.fail() )
                {
                  cerr << "Reading ififo failed. Reopen!" << endl;
                  sleep(3);
                }
              
              debug_msg( "<<<f", buffer, 4);

              out_socket << buffer << endl;

              debug_msg( ">>>s", buffer, 5);
              
              if( out_socket.fail() )
                {
                  cerr << "Writing to out_socket failed!" << endl;
                  exit_cleanly( SIGALRM );
                }


              ififo_stream >> ws;
              ififo_stream.clear();
              ififo_stream.peek();
            }
        }


      gettimeofday(&current_time, NULL);

      if( current_time.tv_sec != last_ping_time )
        {
          out_socket << "@" << endl;
          if( out_socket.fail() )
            {
              cerr << "Writing to out_socket failed!" << endl;
              exit_cleanly( SIGALRM );
            }

          last_ping_time = current_time.tv_sec;
        }


      
      //      cout << "." << flush;      

      in_socket.clear();
      in_socket.get(buffer, 80, '\n');



      if( in_socket.fail() )
        {
          cerr << "Reading in_socket failed!" << endl;
          exit_cleanly( SIGALRM );
        }
      
    }
}





void
exit_cleanly(int signum)
{
  signal(SIGALRM, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGINT,  SIG_IGN);

  int sig_to_send = SIGUSR2;

  if( signum == SIGINT || signum == SIGTERM )
    sig_to_send = SIGTERM;

  cerr << "Recieved signal " << signum 
       << ", sending children " 
       <<  ( sig_to_send == SIGUSR2 ? "SIGUSR2" : "SIGTERM" )
       << " before exiting.\n" << endl;

  kill(0, sig_to_send);


      cerr << "Server " << robotname << ": Cleaning up";


  

  //  TODO: Check if file is a fifo before removing
  if( ! ififo_name.empty() && 
      remove( ififo_name.c_str() ) != 0 )
    {
      cerr << "Couldn't remove " << ififo_name.c_str() << ": ";
      perror( NULL );
    }

  if( ! ififo_name.empty() && 
      remove( ofifo_name.c_str() ) != 0 )
    {
      cerr << "Couldn't remove " << ofifo_name.c_str() << ": ";
      perror( NULL );
    }

  //  finish = true;

  cerr << " and leaving" << endl;

  exit(EXIT_SUCCESS);
}

void
debug_msg(const char* by, const char* msg, const int lvl)
{
  if( debug_level >= lvl )
    {
      struct timeval current_time;
      gettimeofday(&current_time, NULL);
      cout << current_time.tv_sec << "." << current_time.tv_usec << ": ";
      cout << "server " << by << ": " << msg << endl;
    }
}
