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
  Packet* MakePacket( string & s, NetConnection* nc)
  {
    if(s.substr(0, 2) == "OM") //This is a message from the ro
	cout<<s.substr(2, s.length() - 2 )<<endl;
    else
	cout<<s<<endl;
    if( s == "@CQuit" ) nc->close_socket();

    return NULL;
  }
};

class ASmallClientPacketFactory : public PacketFactory {
public:
  Packet* MakePacket( string & s, NetConnection* nc)
  {
    if(s.substr(0, 2) == "0M") //This is a message from the robot
	cout<<s.substr(2, s.length() - 2 )<<endl;
    else
      cout<<s<<endl;
    if( s == "@CQuit" ) nc->close_socket();
    return NULL;
  }
};

class RawSocketClientServer : public SocketClient {
public:
  void check_socket() { SocketClient::check_socket(); };

  void say_hello_to_server(int port_number) {
    char buffer[16];
    sprintf( buffer, "%i", port_number);
    send_to_server( "local " + string(buffer) );
  }

protected:
  //Functions needed by SocketHandler
  void handle_stdin( char * buffer )
  {
	cout<<"Print "<<buffer<<endl;
     	send_to_server( (string)buffer );
  }
};

int main()
{
  RawSocketClientServer sc;
  SocketServer the_server;

  the_server.open_socket( 4148 );

  sc.connect_to_server( "localhost", 4147 );

  sc.set_packet_factory( new ASmallClientPacketFactory );
  the_server.set_packet_factory( new ASmallServerPacketFactory );
  sc.say_hello_to_server( the_server.port_num() );

  while(1)
    {
      sc.check_socket();
      the_server.check_socket();
    }
}
