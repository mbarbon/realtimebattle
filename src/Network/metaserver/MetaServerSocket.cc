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

#include "Console.h"
#include "MetaServerPackets.h"
#include "MetaServerSocket.h"
#include "MetaServerNetConnection.h"


void
MetaServerSocket::open_socket()
{
  struct sockaddr_in src;
  
  if( (server_socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
    {
      my_cons.write(C_FAIL, string("Failed to open socket."));
      quit();
    }

  int opt=1; 
  if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, 
		(char*)&opt, sizeof(opt))) {
    my_cons.write(C_FAIL, string("setsockopt failed."));
  }

  src.sin_family = AF_INET;
  src.sin_port = htons( meta_server_port );
  src.sin_addr.s_addr = htonl( INADDR_ANY );

  if( bind( server_socket, (struct sockaddr*) &src, sizeof( src ) ) < 0 )
    {
      my_cons.write(C_FAIL, string("Failed to bind socket."));
      quit();
    }

  if( listen( server_socket, max_number_connections ) < 0 )
    {
      my_cons.write(C_FAIL, string("Listen to socket failed."));
      quit();
    }
}

void
MetaServerSocket::close_socket()
{
  close( server_socket );
}

void
MetaServerSocket::handle_console_command(string buf)
{
  if( buf == "help" )
    {
      my_cons.write(C_COMMENT, "This the list of the command who can run : ");
      my_cons.write(C_COMMENT, "  help : display this help");
      my_cons.write(C_COMMENT, "  list : list the courant connected servers");
      my_cons.write(C_COMMENT, "  kick <so> : kick a server by giving his name");
      my_cons.write(C_COMMENT, "  quit : kill the metaserver");
      my_cons.write(C_COMMENT, "");
    }
  else if( buf == "quit" )
    {
      quit();
    }
  else if( buf == "list" )
    {
      my_cons.write(C_COMMENT, "Here are the courant connections");
      list<MetaServerNetConnection*>::iterator li;
      for( li = connected_servers.begin(); li != connected_servers.end(); li++ )
	if( (**li).connected && (**li).is_server )
	  {
	    my_cons.write(C_COMMENT, string(" -
 ") + (**li).name 
			  + string ("(") + (**li).language + string(")") 
			  + string(" from ") + (**li).address);
	  }
    }
  else if( buf.substr(0, 4) == "kick" )
    {
      my_cons.write(C_COMMENT, "Wanna kick someone?!?!");
    }
  else
    {
      my_cons.write(C_FAIL, string("<") + buf + string("> : Invalid command"));
    }
}

void
MetaServerSocket::check_socket()
{
  struct timeval tv;
  tv.tv_sec  = 0;
  tv.tv_usec = 500000;

  fd_set readfs;
  fd_set exceptfs;

  FD_ZERO( &readfs );
  FD_ZERO( &exceptfs );

  FD_SET( 0, &readfs );
  FD_SET( server_socket, &readfs );
  FD_SET( server_socket, &exceptfs );
  int max_desc = server_socket;

  list<MetaServerNetConnection*>::iterator li;

  my_cons.prompt_off();

  for( li = connected_servers.begin(); li != connected_servers.end(); li++ )
    if( (**li).connected )
      {
        FD_SET( (**li).the_socket, &readfs );
        FD_SET( (**li).the_socket, &exceptfs );
        max_desc = max( max_desc, (**li).the_socket );
      }

  if( select( max_desc + 1, &readfs, NULL, &exceptfs, &tv ) < 0 )
    {
      my_cons.write(C_FAIL, string("select failed."));
      quit();
    }

  if( FD_ISSET( 0, &readfs ) )
    {
      char buffer[256];
      int n = read( 0, buffer, 256 );
      my_cons.prompt_enter();
      handle_console_command( string(buffer, n-1) );
      my_cons.update_prompt();
    }

  if( FD_ISSET( server_socket, &readfs ) )
    {
      accept_connection();
    }

  for( li = connected_servers.begin(); li != connected_servers.end(); li++ )
    if( (**li).connected && FD_ISSET((**li).the_socket, &exceptfs) )
      {
        my_cons.write(C_FAIL, string("Exception for client."));
        (**li).close_socket();
      }

  for( li = connected_servers.begin(); li != connected_servers.end(); li++ )
    if( (**li).connected && FD_ISSET( (**li).the_socket, &readfs ) )
      {
	int read = (**li).read_data();
	Packet *P;
	while( ! ((**li).read_buffers).empty() )
	  {
	    //Extract the string for the queue and make a packet with it
	    string data = (**li).read_buffers.front();
	    
	    P = make_packet( data );
	    (**li).read_buffers.pop_front();
	    
	    if( !P ) continue; //Jump to the next Packet
	    
	    P->get_string_from_netstring( data );
	    
	    P->handle_packet( *li );
	    //Delete this old packet...
	    delete P;
	  }

	if( read < 0 )
	  {
	    my_cons.write(C_REJECTED, string("I have to close it..."));
	    (**li).close_socket();
	  }
      }
  remove_unconnected_sockets();
  my_cons.prompt_on();
}

void
MetaServerSocket::accept_connection()
{
  int new_socket;
  struct sockaddr_in fromend;

  unsigned int fromlen = sizeof(fromend);

  if( (new_socket = accept( server_socket,
                            (struct sockaddr*) &fromend, &fromlen )) < 0 )
    {
      my_cons.write(C_GENFAIL, string("Accepting connection failed"));
      close( server_socket );
      quit();
    }

  struct MetaServerNetConnection* nc = new MetaServerNetConnection;
  nc->id = next_id;
  next_id++;

  nc->the_socket = new_socket;

  nc->make_nonblocking();

  struct hostent* from = gethostbyaddr( (char*)&fromend.sin_addr,
                                        sizeof(fromend.sin_addr),
                                        AF_INET );

  if( from )
    nc->address = (string)from->h_name;
  else
    nc->address = (string)inet_ntoa( fromend.sin_addr );

  my_cons.write(C_CONNECTION, string("New connection from ") + nc->address);

  nc->connected = true;

  connected_servers.push_back( nc );
}

void
MetaServerSocket::go_through_read_buffers()
{
}


//TODO : find a better way to do it
struct ptr_server_is_not_connected  //Use in the next function
{
  bool operator()(MetaServerNetConnection* const p) {return p->is_not_connected();}
};

void
MetaServerSocket::remove_unconnected_sockets()
{
  list<MetaServerNetConnection*>::iterator li;
  while( ( li = find_if( connected_servers.begin(), connected_servers.end(),
                         ptr_server_is_not_connected() ) )
         != connected_servers.end() )
    {
      delete (*li);
      connected_servers.erase( li );
    }
}

MetaServerSocket::~MetaServerSocket()
{
  list<MetaServerNetConnection*>::iterator li;
  for(li = connected_servers.begin(); li != connected_servers.end();
      li ++)
    {
      (**li).close_socket();
      delete (*li);
    }
  connected_servers.clear();
}

MetaServerSocket* my_metaserversocket;

void
exit_cleanly(int Sign)
{
  quit();
}

void
quit()
{
  delete my_metaserversocket;
  exit( 0 );
}

#define RTB_NAME_VERSION "RTB v2"
#define WEBSITE_URL      "http://"

int
main( int argc, char* argv[] )
{
  my_metaserversocket = new MetaServerSocket;

  my_metaserversocket->open_socket();
  
  signal(SIGALRM, exit_cleanly);
  signal(SIGTERM, exit_cleanly);
  signal(SIGINT,  exit_cleanly);
  

  my_metaserversocket->my_cons.write(C_VERSION, "This is the metaserver for %s", RTB_NAME_VERSION);
  my_metaserversocket->my_cons.write(C_COMMENT, "You can learn a lot about RealTimeBattle at %s",
				     WEBSITE_URL);

  my_metaserversocket->my_cons.prompt_on();


  //my_metaserversocket->my_cons.write(C_COMMENT, "Welcome on RealTimeBattle 2.0.0 metaserver");

  while(1)
    {
      my_metaserversocket->check_socket();
    }

  my_metaserversocket->my_cons.prompt_off();

  my_metaserversocket->close_socket();
  quit();
  return 0;
}
