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

class ASmallFactory : public PacketFactory{
 public:
  Packet* MakePacket( string & s, NetConnection*) 
  {

    if( s == "@CQuit" )
      exit( 0 );
    else if( s.substr(0, 2) == "FI" )
      {
	istrstream data( s.substr(2, s.length() - 1).c_str() );
	string protocol; string channel;
	data >> protocol >> channel;
	if(protocol.substr(0, 12) == "RTB_NET_View" )
	  cout<<"this is informationa about view protocol\n";
	else if(protocol.substr(0, 13) == "RTB_NET_Robot") {
	  Robot_Protocol = protocol;
	  Robot_Channel  = channel;
	}
      }
  return NULL;
}

protected:
  string Robot_Protocol, Robot_Channel;
};

class RawSocketClient : public SocketClient {
public:
  void handle_input_stream(char * buffer)
  {
    istrstream is(buffer);
    string command;
    is >> command;
    
    if( command == "quit")   //The quit event (maybe a click for a chat)
      { 
	cout<<"Ciao\n";
	exit( 0 );
      }
    else if( command == "1")
      {
	my_connection->send_data( "local 1123" );
      }
    else
      {
	my_connection->send_data( (string)buffer );
      }
  }
};

int main()
{
  RawSocketClient sc;
  SocketServer the_server; //just to open a socket to allow the remote client to connect on me !
  the_server.open_socket( );
  sc.connect_to_server( "localhost", 4147 );
  sc.set_packet_factory( new ASmallFactory );
  while(1)
    {
      the_server.check_socket();
      sc.check_socket();
    }
}
