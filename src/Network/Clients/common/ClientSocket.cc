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

#include "ClientSocket.h"

#define USE_INITIALIZATION_PACKET 
#  define SEND_INITIALIZATION_PACKET 

#include "Packets.h"

void
SocketClient::connect_to_server( string hostname, int port )
{
  struct sockaddr_in src;
  struct hostent* hostinfo;
  long address;
  int the_socket;
  
  if( port == 0 )
    port = server_port;
  
  if( hostname.empty() )
    hostname = "localhost";

  if( isdigit( hostname[0] ) )
    {
      if( (address = inet_addr( hostname.c_str() )) == (long)INADDR_NONE )
        {
          my_cons.write(C_FAIL,  "Invalid hostname" );
          return;
        }
      src.sin_addr.s_addr = address;
      src.sin_family = AF_INET;
    }
  else if( (hostinfo = gethostbyname( hostname.c_str() )) == NULL )
    {
      my_cons.write(C_FAIL,  "Failed looking up host" );
      return;
    }
  else
    {
      src.sin_family = hostinfo->h_addrtype;
      src.sin_addr = *((struct in_addr*) hostinfo->h_addr);
    }

  src.sin_port = htons( port );

  if( (the_socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
    {
      my_cons.write(C_FAIL,  "Socket failed" );
      return;
    }

  if( connect( the_socket, (struct sockaddr*) &src, sizeof(src) ) < 0 )
    {
      my_cons.write(C_FAIL,  "Connect failed" );
      return;
    }


  nc.the_socket = the_socket;
  nc.make_nonblocking();
  nc.connected = true;

  s_port = port;
  s_add = hostname;
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
exit_cleanly(int Sign)
{
  quit();
}

