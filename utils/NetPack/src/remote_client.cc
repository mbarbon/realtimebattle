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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <string>
#include <strstream>
#include "ClientSocket.h"
#include "SocketHandler.h"
#include <unistd.h>

class ASmallFactory : public PacketFactory{
 public:
  Packet* MakePacket( string & s, NetConnection*) 
  {

    if( s == "@CQuit" )
      exit( 0 );
    else if(s.substr(0, 10) == "TCRunRobot")
      {
        //TODO : put this code in src/Clients/Gui/Common
        istrstream data( s.substr(10, s.length() - 1).c_str() );
        char name[256], dir[256], unique_id[8], rand_id[8];
        data >> name >> dir >> unique_id >> rand_id;
        pid_t pid = fork();
        if(pid  == 0 )
        {
        }
      }
    else
      cout<<s<<endl; 

    return NULL;
  };
protected:
  string Robot_Protocol, Robot_Channel;
};

class RawSocketClient : public SocketClient {
public:
  void handle_stdin(char * buffer)
  {
    istrstream is(buffer);
    string command;
    is >> command;
    
    if( command == "quit")   //The quit event (maybe a click for a chat)
      { 
        cout<<"Ciao\n";
        exit( 0 );
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
  sc.connect_to_server( "localhost", 4147 );
  sc.send_to_server( "remote" );
  sc.set_packet_factory( new ASmallFactory );
  while(1)
    {
      sc.check_socket();
    }
}
