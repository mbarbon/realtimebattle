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


#include <stdio.h>
#include <arpa/inet.h>
#include <iostream>
#include <cctype>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "RobotPackets.h"
#include "ClientSocket.h"

void
SocketClient::check_connection()
{
  fd_set readfs;
  fd_set exceptfs;

  FD_ZERO( &readfs );
  FD_ZERO( &exceptfs );

  FD_SET( nc.the_socket, &readfs );
  FD_SET( nc.the_socket, &exceptfs );
  FD_SET( 0 , &readfs ); 
  
  my_cons.prompt_off();

  if( select( nc.the_socket + 1, &readfs, NULL, &exceptfs, NULL ) < 0 )
    {
      my_cons.write(C_FAIL,  "select failed." );
      exit(1);
    }

  if( FD_ISSET( nc.the_socket, &exceptfs ) )
    {
      my_cons.write(C_OK, "Exception for client." );
      nc.close_socket();
    }

  if( FD_ISSET( 0, &readfs ) )
    {
      char buffer[256];
      bzero(buffer, 256);
      fgets(buffer, 255, stdin);
      my_cons.prompt_enter();
      my_cons.update_prompt();
      string buf = string (buffer);
      if( buf == "quit")   //The quit event (maybe a click for a chat)
	{ 
	  //Use ClientRequest packet instead ...
	  nc.send_data( CommandPacket("Quit").make_netstring() );
	  exit( 0 );
	}
    }

  if( (nc).connected && FD_ISSET( (nc).the_socket, &readfs ) )
    {
      int read = nc.read_data();

      Packet *P;
      while( ! (nc.read_buffers).empty() )
	{
	  //Extract the string for the queue and make a packet with it
	  string data = nc.read_buffers.front();
	  
	  P = make_packet( data );
	  nc.read_buffers.pop_front();
	  
	  if( !P ) continue; //Jump to the next Packet
	  
	  P->get_string_from_netstring( data );
	  
	  switch(P->packet_type()) 
	    {
	    case PACKET_COMMAND :
	    case PACKET_SERVER_COMMAND :
	    case PACKET_SERVER_MESSAGE :
	      P->handle_packet( &nc );
	      break;
	    default : break;
	    }
	  delete P;
	}
    
      if( read < 0 )
	{
	  (nc).close_socket();
	}
    }
  my_cons.prompt_on();
}

void 
SocketClient::ask_for_info_to_metaserver()
{

}



void
SocketClient::initialize( const string& name, const client_t& c )
{
//    string str = (string)( (char)(RTB_NETPROTOCOL_V1 & 0xff) ) + name;

//    int len = str.length();
//    string send_string = (string)( (char)(PACKET_INIT & 0xff) )

  InitializationPacket IP(  RTB_NETPROTOCOL_V1, 
			    c, name );
  nc.name = name;
  nc.send_data( IP.make_netstring() );
}

void
SocketClient::close_connection()
{
  close( nc.the_socket );
  nc.connected = false;
  nc.the_socket = -1;
}

bool
SocketClient::is_connected()
{
  return !nc.is_not_connected();
}

SocketClient* my_socketclient = new SocketClient;

void
exit_cleanly(int Sign)
{
  quit();
}

void
quit()
{
  delete my_socketclient;
  exit( 0 );
}
