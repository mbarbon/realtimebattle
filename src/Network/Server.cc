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

#include <string>

#include "Socklib.h"

string robotname;
string ififo_name;
string ofifo_name;


void server(int fd);
void exit_cleanly(int fd);

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
  const int uport = 19073;
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

  signal(SIGPIPE, exit_cleanly);
  signal(SIGTERM, exit_cleanly);
  signal(SIGINT,  exit_cleanly);
  signal(SIGPIPE, exit_cleanly);


  ifstream in_socket (socket_fd);
  ofstream out_socket(socket_fd);

  char buffer[80];

  in_socket.get(buffer, 20, '\n');

  robotname = buffer;

  ififo_name = robotname + ".ofifo";  // The input fifo for the server is
  ofifo_name = robotname + ".ififo";  // the output fifo for rtb.



  // Create the fifos

  // TODO: Better error checking needed !!!

  if (  mkfifo( ififo_name.c_str(), S_IWUSR | S_IRUSR ) != 0 )
    {
      cerr << "Server: couldn't create " << ififo_name << ": ";
      perror( NULL );
      ififo_name = "";  // don't remove the fifos when cleaning up
      ofifo_name = "";
      exit_cleanly( SIGPIPE );
    }

  if(  mkfifo( ofifo_name.c_str(), S_IWUSR | S_IRUSR ) != 0 )
    {
      cerr << "Server: couldn't create " << ofifo_name << ": ";
      perror( NULL );
      ofifo_name = "";
      exit_cleanly( SIGPIPE );
    }



  // Open the fifos for read/write
  
  int ififo_fd = open( ififo_name.c_str(), O_RDONLY );

  if( ififo_fd == -1 )
    {
      cerr << "Server: couldn't open " << ififo_name << ": ";
      perror( NULL );
      exit_cleanly( SIGPIPE );
    }

  int ofifo_fd = open( ofifo_name.c_str(), O_WRONLY );

  if( ofifo_fd == -1 )
    {
      cerr << "Server: couldn't open " << ofifo_name << ": ";
      perror( NULL );
      exit_cleanly( SIGPIPE );

    }


  ofstream ofifo_stream(ofifo_fd);
  ifstream ififo_stream(ififo_fd);


  // Main loop

  fd_set fifo_and_socket;

  struct timeval time_to_wait;

  while( true )
    {
      time_to_wait.tv_sec = 0;
      time_to_wait.tv_usec = 300000;

      FD_ZERO( &fifo_and_socket );
      FD_SET( socket_fd, &fifo_and_socket );
      FD_SET( ififo_fd, &fifo_and_socket );

      select(FD_SETSIZE, &fifo_and_socket, NULL, NULL, &time_to_wait);


      if( FD_ISSET( socket_fd, &fifo_and_socket) )
        {      
          in_socket >> ws;
          in_socket.clear();
          in_socket.get(buffer, 80, '\n');

          cout << buffer;      
          ofifo_stream << buffer << endl;      
          cout << "!" << flush;   
        }

      if( FD_ISSET( ififo_fd, &fifo_and_socket) )
        {      
          ififo_stream >> ws;
          ififo_stream.clear();
          ififo_stream.get(buffer, 80, '\n');

          if( ififo_stream.fail() )
            {
              cerr << "Reading ififo failed. Reopen!" << endl;
              sleep(3);
            }

          cout << buffer;      
          out_socket << buffer << endl;
          cout << "%" << flush;   
        }
      
      cout << "." << flush;      
      
      out_socket << "Message_from_server_" << robotname << endl;
      //      sleep(1);

    }

}





void
exit_cleanly(int signum)
{
  int sig_to_send = SIGUSR2;

  if( signum == SIGINT || signum == SIGTERM )
    sig_to_send = SIGTERM;

  cerr << "Recieved signal " << signum 
       << ", sending children " 
       <<  ( sig_to_send == SIGUSR2 ? "SIGUSR2" : "SIGTERM" )
       << " before exiting.\n" << endl;

  kill(0, sig_to_send);


  cout << "Server " << robotname << ": Cleaning up";


  

  //  TODO: Check if file is a fifo before removing
  if( ! ififo_name.empty() && 
      remove( ififo_name.c_str() ) != 0 )
    {
      cerr << "Couldn't remove " << ofifo_name.c_str() << ": ";
      perror( NULL );
    }

  if( ! ififo_name.empty() && 
      remove( ofifo_name.c_str() ) != 0 )
    {
      cerr << "Couldn't remove " << ofifo_name.c_str() << ": ";
      perror( NULL );
    }
  
  cout << " and leaving" << endl;

  exit(EXIT_SUCCESS);
}
