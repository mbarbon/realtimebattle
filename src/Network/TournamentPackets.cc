#include <string>
#include <iostream.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <strstream>

#include "NetConnection.h"
#include "ServerSocket.h"
#include "ServerPackets.h"

#include "Robot.h"
#include "Arena.h"

#include "String.h"

Packet*
TournamentAgreementPacketFactory::MakePacket( string& netstr )
{
  string type = netstr.substr(0,2);

  if     (type == "CO")    return new InitializationPacket( );
  else if(type == "@C")    return new CommandPacket( );
  else if(type == "SL")    return new SubmitListPacket( );

  //else if(type == "@M")    return new ChatMessagePacket( );
  //else if(type == "@R")    return new RobotMessagePacket( );
  
  return NULL;
}

string 
InitializationPacket::make_netstring() const
{
  ostrstream os;
  os << type_init << " "
     << protocol ;

  if(type_init == "Join")
    {
      os << " " << my_socketserver.host_name 
	 << " " << my_socketserver.port_number;
    }
  else 
    {
      os << " " << channel;
    }
  os<< ends;
    
  string n_str;
  n_str = "@I";

  n_str += string( os.str() );
  return n_str;
}

int 
InitializationPacket::handle_packet(void* p_void)
{
  NetConnection* nc = (NetConnection*) p_void;
  istrstream is(data.c_str());
  string type_init;

  cout<<data<<endl;

  is >> type_init >> protocol;

  if(type_init == "Join" && protocol == RTB_PROTOCOL_VERSION_STR)
    {	      // This is the good protocol and he is looking for a server
      //TODO : Send some info about me (as with a metaserver)
      string his_host_name;
      int port_num, max_friends;
      is >> his_host_name >> port_num >> max_friends;
      
      //TODO : Check if it is not already connected
      if( my_socketserver.accept_new_server() )
	{
	  nc->set_type(SERVER_CONNECTION);
	  my_socketserver. by_type_connections[SERVER_CONNECTION]. push_back(nc);
	  
	  ServerState new_server_info = ServerState(his_host_name, port_num, 1, 1);
	  my_socketserver. server_states[nc] = new_server_info;
	  my_socketserver.nb_conn_friends += 1;
	  
	  ostrstream os1;
	  os1<<"Connection_Confirmed "
	     << my_socketserver.by_type_connections[SERVER_CONNECTION].size() << " "
	     << my_socketserver.nb_conn_friends << ends;
	  
	  CommandPacket P ( os1.str() );
	  nc->send_data( P.make_netstring() );
	  
	  //Send all my friends that I have one more connection
	  CommandPacket To_Servers ( "FreeConn" );
	  my_socketserver. send_packet_by_type(SERVER_CONNECTION, &To_Servers );
	}
      else
	{
	  //Send him some other addresses where he can connect
	  CommandPacket P ("Connection_Refused");
	  nc->send_data( P.make_netstring() );
	  nc->close_socket();
	  //Either accept the new connection (and have more connections than allowed)
	  //Or close the connection and redirect him on an other server
	  //Or close an other server and let it be the link between us two ...
	  //Use a random value to know which one we 
	  
	  //TODO : Remove the connection from the map
	  
	}
      //Find the other servers that will be connected to the new comer.
    }
  
  else if(type_init == "Connect") //He is already connected and he wants to open a other channel
    {
      is >> channel;
      cout<<"He wants to connect to protocol "<<protocol<<" on channel "<<channel<<endl;
      cout<<"See if we have some..."<<endl;
      if(my_socketserver.channel_protocol(channel) == protocol) {
	cout<<"Ok, he can connect\n";
	nc->set_type(channel);
	my_socketserver. by_type_connections[channel]. push_back(nc);

      }

    }
  //TODO : Remove the connection from the undefined list
  //TODO : Send a confirmation message to say you are on a RTB ring...
  return 0;
}
