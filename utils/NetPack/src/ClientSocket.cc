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

#include "ClientSocket.h"


SocketClient::~SocketClient()
{
  if(client_packet_factory) delete client_packet_factory;
  if(server_connection)      delete server_connection;
}

void 
SocketClient::set_packet_factory( PacketFactory* pf )
{
  if(client_packet_factory) delete client_packet_factory;
  client_packet_factory = pf;
}

NetConnection*
SocketClient::connect_to_server( string hostname, int port_nb = 0 )
{
  //if( port_nb == 0 )
  //  port_nb = meta_server_port;

  struct sockaddr_in src;
  struct hostent* hostinfo;
  long address;
  int the_socket;

  if( hostname.empty() )
    hostname = "localhost";

  if( isdigit( hostname[0] ) )
    {
      if( (address = inet_addr( hostname.c_str() )) == (long)INADDR_NONE )
        {
          cout<< "Invalid hostname\n" ;
          return NULL;
        }
      src.sin_addr.s_addr = address;
      src.sin_family = AF_INET;
    }
  else if( (hostinfo = gethostbyname( hostname.c_str() )) == NULL )
    {
      cout<<"Failed looking up host\n" ;
      return NULL;
    }
  else
    {
      src.sin_family = hostinfo->h_addrtype;
      src.sin_addr = *((struct in_addr*) hostinfo->h_addr);
    }
  
  src.sin_port = htons( port_nb );

  if( (the_socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
    {
      cout<<"Socket failed\n";
      return NULL;
    }

  if( connect( the_socket, (struct sockaddr*) &src, sizeof(src) ) < 0 )
    {
      cout<<"Connect failed\n";
      return NULL;
    }

  if(server_connection ) {
    server_connection->close_socket();
    delete server_connection;
  }

  server_connection = new NetConnection;

  server_connection->the_socket = the_socket;
  server_connection->connected = true;
  server_connection->make_nonblocking();

  return server_connection;
}

void
SocketClient::set_fd( )
{
  if( server_connection->connected )
    add_fd( server_connection->the_socket );
}

void
SocketClient::check_fd( )
{
  if( server_connection->connected )
  {
    if( is_fd_except( server_connection->the_socket ) )
    {
      cout<<"Exception, close it \n";
      server_connection->close_socket();
      return;
    }
    if( is_fd_read( server_connection->the_socket ) )
    {
      int read = server_connection->read_data();
      Packet *P;
      while( ! (server_connection->read_buffers).empty() )
      {
        //Extract the string for the queue and make a packet with it
        string data = server_connection->read_buffers.front();
        cout<<"data in client : "<<data<<endl;
        P = client_packet_factory->MakePacket( data, server_connection );
        server_connection->read_buffers.pop_front();

        if( !P ) continue; //Jump to the next Packet

        P->handle_packet( );
        delete P;
      }

      if( read < 0 )
      {
        server_connection->close_socket();
        return;
      }
    }
  }
}

void
SocketClient::send_to_server( string s )
{
  server_connection->send_data( s );
}
