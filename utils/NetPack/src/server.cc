/*
NetPack for RealTimeBattle
Copyright (C) 2002  Benoit Rousseau

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
#include <signal.h>

#ifdef FLOATINGPOINT_H
#include <floatingpoint.h>
#endif

#include <list>

#include "ServerSocket.h"

#ifndef WAIT_ANY
#define WAIT_ANY (pid_t)-1
#endif

class SocketServer my_socketserver;

class TheRealServerPacketFactory : public PacketFactory {
public:
  Packet* MakePacket( string & data, NetConnection* nc)
  {
    if( data  == "remote" )
    {
      cout<<"A new remote client\n";
      remote_clients.push_back( nc );
    }
    else if( data.substr(0, 5) == "local" )
    {
      if( remote_clients.size() != 0 )
      {
        cout<<"I have something for you\n";
        (*(remote_clients.begin()))->send_data(data.substr(5, data.length()-5));
        (*(remote_clients.begin()))->send_data( "@CQuit" );
        (*(remote_clients.begin()))->close_socket();
      }
      else
        cout<<"Not enough remote clients\n";
      cout<<"Tell him to quit \n";
      nc->send_data( "@CQuit" );
      //nc->send_data("Hello");
      cout<<"Message sent\n";
      nc->close_socket();
      cout<<"Done\n";
    }
    return NULL;
  }
  void remove_connection( NetConnection* nc )
  {
    for(list<NetConnection*>::iterator li = remote_clients.begin();
        li != remote_clients.end() ; li ++ )
    {
      if( *li == nc )
      {
        remote_clients.erase( li );
        break;
      }
    }
  }

protected:
  list<NetConnection*> remote_clients;
};


/*
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
          if( errno != ECHILD ) 
            Error(true, "waitpid failed", "server.cc:sig_handler");
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
sigfpe_handler(int signum)
{
  signal(signum, sigfpe_handler);
}
*/

int
main( int argc, char* argv[] )
{

/*
  signal(SIGCHLD, sig_handler);
  signal(SIGPIPE, sig_handler);
  signal(SIGFPE, sigfpe_handler);

  signal(SIGALRM, exit_cleanly);
  signal(SIGTERM, exit_cleanly);
  signal(SIGINT,  exit_cleanly);
*/
  my_socketserver.open_socket();

  my_socketserver.set_packet_factory( new TheRealServerPacketFactory );

  while( 1 )
    my_socketserver.check_socket();


  my_socketserver.close_socket();
  quit();
  return EXIT_SUCCESS;
}
