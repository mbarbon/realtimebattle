/*
NetPack for RealTimeBattle
Copyright (C) 2002   Benoit Rousseau

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
#include <config.h>
#endif

#include <iostream>
#include <string>
#include <strstream>
#include "ClientSocket.h"
#include "ServerSocket.h"
#include <unistd.h>

class ASmallServerPacketFactory : public PacketFactory{
public:
  Packet* MakePacket( string & s, NetConnection*) 
  {
    cout<<"I receive (server) "<<s<<endl;
    if( s == "@CQuit" )
      exit( 0 );

    return NULL;
  }
};

class ASmallClientPacketFactory : public PacketFactory {
public:
  Packet* MakePacket( string & s, NetConnection* nc)
  {
    cout<<"I receive (client) "<<s<<endl;
    if( s == "@CQuit" ) nc->close_socket();
    return NULL;
  }
};

class RawSocketClientServer : public SocketClient, public SocketServer {
public:
  void check_socket() { SocketServer::check_socket(); };

  void say_hello_to_server() {
    char buffer[16];
    sprintf( buffer, "%i", port_number);
    send_to_server( "local " + string(buffer) );
  }
  void set_packet_factories( PacketFactory* server_pf, PacketFactory* client_pf ) {
    SocketServer::set_packet_factory( server_pf );
    SocketClient::set_packet_factory( client_pf );
  }

protected:
  //Functions needed by SocketHandler
  void handle_stdin( char * buffer )
  {
    istrstream is(buffer);
    string command;
    is >> command;

    if( command == "quit" )   //The quit event (maybe a click for a chat)
      {
        cout<<"Ciao\n";
        exit( 0 );
      }
    else
      {
        send_to_server( (string)buffer );
      }
  }

  void check_fd( ) {
    SocketServer::check_fd();
    SocketClient::check_fd();
  }
  void set_fd( ) {
    SocketServer::set_fd();
    SocketClient::set_fd();
  }

};

int main()
{
  RawSocketClientServer sc;

  sc.open_socket();
  sc.connect_to_server( "localhost", 4147 );

  sc.set_packet_factories( new ASmallServerPacketFactory, new ASmallClientPacketFactory );

  sc.say_hello_to_server();
  while(1)
    {
      sc.check_socket();
    }
}
