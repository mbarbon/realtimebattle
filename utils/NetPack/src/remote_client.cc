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
#include "SimpleProcess.h"

string robot_path;
SimpleProcess* process = NULL;

class RawSocketClient : public SocketClient {
public:
  void handle_stdin(char * buffer)
  {
    istrstream is(buffer);
    string command;

    if( command == "quit")   //The quit event (maybe a click for a chat)
      {
        cout<<"Ciao\n";
        exit( 0 );
      }
    else
      {
        send_to_server( (string)buffer );
      }
  }
};

RawSocketClient sc;

class ClientsLinkPacketFactory : public PacketFactory {
 public:
  Packet* MakePacket( string & s, NetConnection* nc)
  {
	  cout<<">>"<<s<<endl;
	 cout<<"I'm here\n";
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


class ClientServerLinkFactory : public PacketFactory{
 public:
  Packet* MakePacket( string & s, NetConnection* nc)
  {
    int next_port;
    istrstream is ( s.c_str() );
    is >> next_port;
    cout<<"Connecting to the 'local' client on port "<<next_port<<endl;
    sc.connect_to_server( "localhost", next_port );
    cout<<"Done\n";
    cout<<"Setting the packet factory\n";
    sc.set_packet_factory( new ClientsLinkPacketFactory );
    cout<<"Done\n";
    cout<<"Running the robot \n";
    process = new SimpleProcess( robot_path. c_str() );
    process->start(  );
    cout<<"Done\n";
    /*
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
    */
    return NULL;
  };
protected:
  string Robot_Protocol, Robot_Channel;
};


int main(int argc, char* argv[])
{
  if( argc != 3 ) {
    cout<<"Usage : "<<argv[0]<<" <server> <robot_name>"<<endl;
    exit( 0 );
  }

  robot_path = string( argv[2] );
  
  sc.connect_to_server( string(argv[1]), 4147 );
  sc.send_to_server( "remote" );
  sc.set_packet_factory( new ClientServerLinkFactory );
  while(1)
    {
      sc.check_socket();
      if( process )
      {
        process->reset_messages();
        while( process->more_messages()  )
        {
          string message = process->get_next_message();
          cout<<"<<"<<message<<endl;
          sc.send_to_server("OM" + message );
        }
      }
    }

}
