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
#include <list>
#include <strstream>
#include "ClientSocket.h"
#include "ServerSocket.h"
#include <unistd.h>

list<string> a_list;

class ASmallServerPacketFactory : public PacketFactory{
public:
  Packet* MakePacket( string & s, NetConnection* nc)
  {
    cout<<"Print "<<s;
    if(s.substr(0, 2) == "OM") //This is a message from the row packet factory
      {
        cout<<"Print "<<s.substr(2, s.length() - 2 );
        cout<<s.substr(2, s.length() - 2 );
      }
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
    cout<<"Print "<<s;
    return NULL;
  }
};

class RawSocketClient : public SocketClient {
public:
  void say_hello_to_server(int port_number) {
    char buffer[16];
    sprintf( buffer, "%i", port_number);
    send_to_server( "local " + string(buffer) );
    server_connection->close_socket();
  }
protected:
  void handle_stdin( char* buffer )
  {
    a_list.push_back(string(buffer));
  }
};

class RawSocketServer : public SocketServer {
protected:
  void handle_stdin( char * buffer )
  {
    //cout<<"Print "<<buffer<<endl;
    //Broadcast old messages in the list;
    while( a_list.size() != 0 ) {
       cout<<"Sending "<<*(a_list.begin())<<endl;
       broadcast( *(a_list.begin()) );
       a_list.erase( a_list.begin() );
    }

    //broadcast the incoming message
    broadcast( (string)buffer );
  }

  void broadcast( string s )
  {
    list<NetConnection*>::iterator i;
    for( i = all_connections.begin(); i != all_connections.end(); i ++)
      (**i).send_data( string("RI") + s );
  }
};

int main()
{
  RawSocketClient sc;
  RawSocketServer the_server;

  the_server.open_socket( 4148 );

  sc.connect_to_server( "localhost", 4147 );

  sc.set_packet_factory( new ASmallClientPacketFactory );
  the_server.set_packet_factory( new ASmallServerPacketFactory );
  sc.say_hello_to_server( the_server.port_num() );

  //while(sc.check_socket() == 0)  ;     //should be necessary !!!
  cout<<"Print ready\n";
  while(the_server.check_socket() == 0);
  cout<<"Print done\n";
}
