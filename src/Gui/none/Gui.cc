/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony

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
    else if(s.substr(0, 10) == "TCRunRobot")
      {
	//TODO : put this code in src/Clients/Gui/Common
	istrstream data( s.substr(10, s.length() - 1).c_str() );
	char name[256], dir[256], unique_id[8], rand_id[8];
	data >> name >> dir >> unique_id >> rand_id;
	pid_t pid = fork();
	if(pid  == 0 )
	  {
	    execl("/tmp/roussebe/RealTimeBattle/src/RobotClient/rtb_RobotClient",
		  "rtb_RobotClient", "localhost", "4147", Robot_Protocol.c_str(), Robot_Channel.c_str(),
		  dir, name,  unique_id, rand_id, NULL);
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
	my_connection->send_data( "@IConnect RTB_NET_Agree_0.1 2" );
	my_connection->send_data( "TCAddRob /tmp/roussebe/RealTimeBattle/Robots/empty/ empty.robot 0 0");
	my_connection->send_data( "TCAddRob /tmp/roussebe/RealTimeBattle/Robots/empty/ empty.robot 0 0");
	my_connection->send_data( "TCAddArn  /tmp/roussebe/RealTimeBattle/Arenas/ Original.arena 0");
      }
    else if(command == "2")
      {
	my_connection->send_data( "ST1" );
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
  sc.set_packet_factory( new ASmallFactory );
  while(1)
    {
      sc.check_socket();
    }
}
