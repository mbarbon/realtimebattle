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


#include <arpa/inet.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <strstream>
#include <fstream.h>
#include <stdio.h>

#include "ServerSocket.h"


SocketServer::SocketServer()
{
  my_id = 0;
  next_id = 0;
  open_socket( );
}


void
SocketServer::open_socket( int port_nb = 0 )
{
  struct sockaddr_in src;

  //Set my port number
  if( port_nb == 0 )
    port_number = server_port;
  else
    port_number = port_nb;

  int nb_try = 0;

  while( 1 )
  {
  if( ((++nb_try) % 10) == 0 )
    port_number += 100;
  if( nb_try == 50 ) quit() ;
  cout<<"Opening on port "<<port_number<<endl;
  //Set my host name
  char sn[256];
  gethostname(sn, 256);
  host_name = string(sn);

  //Open my socket

  if( (server_socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
    {
      cout<<( "Failed to open socket." );
      quit();
    }

  int opt=1;
  if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR,
		(char*)&opt, sizeof(opt))) {
    cout<<"setsockopt failed"<<endl;
  }

  src.sin_family = AF_INET;
  src.sin_port = htons( port_number );
  src.sin_addr.s_addr = htonl( INADDR_ANY );

  if( bind( server_socket, (struct sockaddr*) &src, sizeof( src ) ) < 0 )
    {
      cout<<( "Failed to bind socket." );
      continue;
    }

  if( listen( server_socket, max_number_connections ) < 0 )
    {
      cout<<( "Listen to socket failed." );
      continue;
    }
  break;
  }
}

void
SocketServer::close_socket()
{
  close( server_socket );
}

void
SocketServer::check_socket()
{
  struct timeval tv;
  tv.tv_sec  = 0;
  tv.tv_usec = 0; //500000;

  fd_set readfs;
  fd_set exceptfs;


  FD_ZERO( &readfs );
  FD_ZERO( &exceptfs );

  FD_SET( 0 , &readfs ); 
 
  FD_SET( server_socket, &readfs );
  FD_SET( server_socket, &exceptfs );

  int max_desc = server_socket;

  list_It_NetConn li;
  for( li = all_connections.begin(); li != all_connections.end(); li++ )
    if( (**li).connected )
      {
        FD_SET( (**li).the_socket, &readfs );
        FD_SET( (**li).the_socket, &exceptfs );
        max_desc = max( max_desc, (**li).the_socket );
      }

  if( select( max_desc + 1, &readfs, NULL, &exceptfs, &tv ) < 0 )
    {
      cout<<( "select failed." ) << endl;
      quit();
    }

  
  if( FD_ISSET( 0, &readfs ) )
    {
      char buffer[256];
      bzero(buffer, 256);
      fgets(buffer, 255, stdin);
      
      istrstream is(buffer);
      string command;
      is >> command;
      
      if( command == "quit")   //The quit event (maybe a click for a chat)
	{ 
	  cout<<"Ciao\n";
	  exit( 0 );
	}
    }

  if( FD_ISSET( server_socket, &readfs ) )
    {
      cout << "Got new connection."<<endl;
      accept_connection();
    }

  for( li = all_connections.begin(); li != all_connections.end(); li++ )
    if( (**li).connected && FD_ISSET((**li).the_socket, &exceptfs) )
      {
        cout<<( "Exception for client.\n" );
        (**li).close_socket();
      }

  for( li = all_connections.begin(); li != all_connections.end(); li++ )
    {
      if( (**li).connected && FD_ISSET( (**li).the_socket, &readfs ) )
	{
	  //NOTE : Maybe have the connection as a variable in the class.
	  int read = (**li).read_data();

	  while( ! ((**li).read_buffers).empty() )
	    {
	      //Extract the string for the queue and make a packet with it
	      string data = (**li).read_buffers.front();

        if( data  == "remote" )
        {
          cout<<"A new remote client\n";
          remote_clients.push_back( *li );
        }
        else if( data.substr(0, 5) == "local" )
          {
            if( remote_clients.size() != 0 )
            {
              (*(remote_clients.begin()))->send_data(data.substr(5, data.length()-5));
              (*(remote_clients.begin()))->send_data( "@CQuit" );
              (*(remote_clients.begin()))->close_socket();
            }
            else
              cout<<"Not enough remote clients\n";
            (**li).send_data( "@CQuit" );
            (**li).close_socket();
          }
	      (**li).read_buffers.pop_front();
	    }

	  if( read < 0 )
	    {
	      (**li).close_socket();
	    }
	}
    }
  remove_unconnected_sockets();
}

NetConnection*
SocketServer::accept_connection()
{
  int new_socket;
  struct sockaddr_in fromend;

  unsigned int fromlen = sizeof(fromend);

  cout<<"Accepting a new connection\n";

  if( (new_socket = accept( server_socket,
                            (struct sockaddr*) &fromend, &fromlen )) < 0 )
    {
      cout<<( "Accepting connection failed" );
      close( server_socket );
      quit();
    }

  struct NetConnection* nc = new NetConnection;
  nc->id = next_id;
  next_id++;

  nc->the_socket = new_socket;
  nc->make_nonblocking();
  nc->connected = true;

  all_connections.push_back( nc );

  return nc;
}

struct ptr_is_not_connected  //Use in the next function
{
  bool operator()(NetConnection* const p) {return p->is_not_connected();}
};

void
SocketServer::remove_unconnected_sockets()
{
  list_It_NetConn li;


  while( ( li = find_if( remote_clients.begin(), remote_clients.end(),
                         ptr_is_not_connected() ) )
         != remote_clients.end() )
    {
      remote_clients.erase( li );
    }

  while( ( li = find_if( all_connections.begin(), all_connections.end(),
                         ptr_is_not_connected() ) )
         != all_connections.end() )
    {
      delete (*li);
      all_connections.erase( li );
    }
}

SocketServer::~SocketServer()
{
  list_It_NetConn li;

  for( li = all_connections.begin(); li != all_connections.end(); li++ )
    (**li).send_data( "@CQuit" );

  sleep(1);

  //Close all the connections
  for(li = all_connections.begin(); li != all_connections.end();
      li ++)
    {
      (**li).close_socket();
      delete (*li);
    }
  all_connections.clear();

}

void
exit_cleanly(int Sign)
{
  quit();
}

void
quit()
{
  exit( 0 );
}

