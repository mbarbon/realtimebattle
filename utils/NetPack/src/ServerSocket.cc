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
      cout<<( "Failed to open socket." )<<endl;
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
      cout<<( "Failed to bind socket." )<<endl;
      port_number ++;
      continue;
    }

  if( listen( server_socket, max_number_connections ) < 0 )
    {
      cout<<( "Listen to socket failed." )<<endl;
      port_number ++;
      continue;
    }
    cout<<"Opened on socket "<< server_socket<<endl;
  break;
  }
}

void
SocketServer::close_socket()
{
  close( server_socket );
}

void
SocketServer::set_fd( )
{
  add_fd( server_socket );
  list_It_NetConn li;
  for( li = all_connections.begin(); li != all_connections.end(); li++ )
    if( (**li).connected )
    {
      add_fd( (**li).the_socket );
    }
}

void
SocketServer::handle_stdin( char * buffer )
{
  istrstream is(buffer);
  string command;
  is >> command;

  if( command == "quit")   //The quit event
  {
    cout<<"Ciao\n";
    exit( 0 );
  }
}

void
SocketServer::check_fd( )
{
  //See if no exception occured
  list_It_NetConn li;

  for( li = all_connections.begin(); li != all_connections.end(); li++ )
    if( (**li).connected && is_fd_except((**li).the_socket) )
      {
        cout<<( "Exception for client.\n" );
        (**li).close_socket();
      }

  //New comer.
  if( is_fd_read( server_socket ) )
    {
      cout << "Got new connection."<<endl;
      accept_connection();
    }


  for( li = all_connections.begin(); li != all_connections.end(); li++ )
    {
      if( (**li).connected && is_fd_read( (**li).the_socket ) )
	{
	  //NOTE : Maybe have the connection as a variable in the class.
	  int read = (**li).read_data();

	  while( ! ((**li).read_buffers).empty() )
	    {
	      //Extract the string for the queue and make a packet with it
	      string data = (**li).read_buffers.front();
        cout<<"data in server : "<<data<<endl;
        Packet * P;
        P = server_packet_factory->MakePacket( data, *li );
        (*li)->read_buffers.pop_front();

        if( !P ) continue; //Jump to the next Packet

        P->handle_packet( );
        delete P;


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

  if( (new_socket = accept( server_socket,
                            (struct sockaddr*) &fromend, &fromlen )) < 0 )
    {
      cout<<( "Accepting connection failed" )<<endl;
      close( server_socket );
      quit();
    }

  struct NetConnection* nc = new NetConnection;
  nc->id = next_id;
  next_id++;

  cout<<"New connection on "<<new_socket<<endl;
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



  while( ( li = find_if( all_connections.begin(), all_connections.end(),
                         ptr_is_not_connected() ) )
         != all_connections.end() )
    {
      server_packet_factory->remove_connection( *li );
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

  delete server_packet_factory;

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

