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

/* 
   ServerPackets.cc : description of all the packets needed by servers
                      to communicate in a distributed way
   
   sam jun 29 : Set and use nb of conn of other servers to know if it
                is better to redirect a new server or if I should accept it
   before     : Some kind of connections/communications

   TODO : Make the commands a bit shorter (2-3 char instead of a long string)
*/

#include <netinet/in.h>
#include <string>
#include <iostream.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <strstream>

#include "NetConnection.h"
#include "ServerSocket.h"
#include "ServerPackets.h"

#include "String.h"

Packet*
ServerPacketFactory::MakePacket( string& netstr )
{
  string type = netstr.substr(0,2);

  if     (type == "@C")    return new CommandPacket( );
  else if(type == "SL")    return new SubmitListPacket( );

  //else if(type == "@M")    return new ChatMessagePacket( );
  //else if(type == "@R")    return new RobotMessagePacket( );
  
  return NULL;
}

Packet*
InitPacketFactory::MakePacket( string& netstr )
{

  string type = netstr.substr(0,2);
  if (type == "@I")  return new InitializationPacket( );
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
	  my_socketserver. connection_factory[ nc ] = &my_serverpacketfactory;
	  
	  ServerState new_server_info = ServerState(his_host_name, port_num, 1, 1);
	  my_socketserver. server_states[nc] = new_server_info;
	  my_socketserver.nb_conn_friends += 1;

	  if(my_socketserver.leader == NULL) //I don't have any leader : I'm the leader !!!
	    {
	      ostrstream os;
	      os << "NextId "<<	(my_socketserver.next_id) ++ <<ends; 
	      CommandPacket NextId ( os.str() );
	      my_socketserver.send_packet_by_type(SERVER_CONNECTION, &NextId);	      
	    }	  
	  
	  ostrstream os1;
	  os1<<"Connection_Confirmed "
	     << my_socketserver.by_type_connections[SERVER_CONNECTION].size() << " "
	     << my_socketserver.nb_conn_friends << ends;
	  
	  CommandPacket P ( os1.str() );
	  nc->send_data( P.make_netstring() );
	  
	  //Send all my friends that I have one more connection
	  CommandPacket To_Servers ( "FreeConn" );
	  my_socketserver. send_packet_by_type(SERVER_CONNECTION, &To_Servers );
	  return 1;
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
	my_socketserver. connection_factory[ nc ] = my_socketserver.packet_factory( channel );
	cout<<my_socketserver. connection_factory[ nc ]->Protocol()<<endl;
	return 1;
      }
    }
  else
    {}

  return 0;
}

CommandPacket::CommandPacket( const string& c /* , unsigned n, ... */ )
{
  comm = c;
  /*
    va_arg curseur;
    va_start( curseur, n);
    while(n--)
    arg.push_back(va_arg( curseur, string ));
    va_end (curseur);
  */
}

string 
CommandPacket::make_netstring() const
{
  string datastring;

  if(comm == "FreeConn")
    {
      int av = 0;
      for(map<NetConnection*, ServerState>::iterator it =my_socketserver.server_states.begin();
	  it != my_socketserver.server_states.end(); it ++)
	av += (*it).second.nb_conn;

      ostrstream os;
      os<<"FreeConn "
	<< my_socketserver.by_type_connections[SERVER_CONNECTION].size() << " " 
	<< av << ends;
      datastring = os.str();
    }
  else
    datastring = comm;
  /*
    for( vector<string>::iterator i = arg.begin();
    i != arg.end(); i++ )
    {
    datastring += " " + ( *i );
    }
  */

  string n_str;
  n_str = "@C";

  n_str += datastring;
  
  return n_str;
}

int 
CommandPacket::handle_packet(void* p_void)
{ /* Should be used by the server */
  
  NetConnection* cc = (NetConnection*) p_void;

  cout<<data<<endl;

  istrstream is(data.c_str());
  string command;
  is >> command;

  if( command == "NeedFriends") {
    string protocol;
    is >> protocol ;

    ostrstream os;
    os << "SomeFriends " << protocol << ends;

    vector<string> vs;
    vs = my_socketserver.give_other_addresses(cc, protocol);

    if(vs.size() > 0)
      {
	SubmitListPacket P( os.str(), vs );
	cc->send_data( P.make_netstring() );
      }
  }
  else if( command == "NextId") {
    int next_id;
    is >> next_id;
    if(my_socketserver.my_id == 0)
      {
	my_socketserver.my_id = next_id;
	cout<<"I'm number "<<my_socketserver.my_id<<endl;
	my_socketserver.leader = cc;
      }
    my_socketserver.next_id = next_id;
  }
  else if( command == "FreeConn") { //He has changed his number of connections
    int nb_friends, nb_conn_of_friends;
    is >> nb_friends >> nb_conn_of_friends;
    my_socketserver.nb_conn_friends += nb_friends - my_socketserver.server_states[cc].nb_conn;
    my_socketserver.server_states[cc].set_nb_conn (nb_friends, nb_conn_of_friends);
  }
  else if( command == "Quit" )
    {
      cc->close_socket();
      return 0;
    }
  else if( command == "Connection_Confirmed")
    {
      //Know I know that I'm on the ring !!!
      unsigned int his_nb_conn, nb2;
      is >> his_nb_conn >> nb2;

      my_socketserver.server_states[cc].set_nb_conn(his_nb_conn, nb2);
      my_socketserver.nb_conn_friends += nb2;

      unsigned int my_nb_conn = my_socketserver.by_type_connections[SERVER_CONNECTION].size();

      CommandPacket To_Servers ( "FreeConn" );
      my_socketserver.send_packet_by_type(SERVER_CONNECTION, &To_Servers );

      my_socketserver.by_type_conn(SERVER_CONNECTION) -> push_back(cc);
      if( my_nb_conn < his_nb_conn  ) 
      {
	  //TODO : Also check if I have some request going on...
        cc -> send_data( CommandPacket("NeedFriends 1").make_netstring() );
      }
    }
  else if( command == "Connection_Refused") 
    {
    }
  else
    cout<<"Unknown command "<< command<<endl;

  //Should never come here :-)

  return 1;
}


/*
 * TODO : remove all this constructor !!!
 *    Only need to have SubmitList(string, vector<string>&)
 *    And maybe a function AddToList(string) to add one element to the list !!!
 */
SubmitListPacket::SubmitListPacket( string T , const vector<string>& FN)
{
  type = T;
  file_name = FN;
}

SubmitListPacket::SubmitListPacket( string T, string& s1 )
{
  type = T;
  file_name.push_back(s1);
}

SubmitListPacket::SubmitListPacket( string T, string& s1, string& s2 )
{
  type = T;
  file_name.push_back(s1);
  file_name.push_back(s2);
}

SubmitListPacket::SubmitListPacket( string T, string& s1, string& s2, string& s3 )
{
  type = T;
  file_name.push_back(s1);
  file_name.push_back(s2);
  file_name.push_back(s3);
}

SubmitListPacket::SubmitListPacket( string T, string& s1,
                            string& s2, string& s3, string& s4 )
{
  type = T;
  file_name.push_back(s1);
  file_name.push_back(s2);
  file_name.push_back(s3);
  file_name.push_back(s4);
}

SubmitListPacket::SubmitListPacket( string T, string& s1,
                            string& s2, string& s3, string& s4, string& s5 )
{
  type = T;
  file_name.push_back(s1);
  file_name.push_back(s2);
  file_name.push_back(s3);
  file_name.push_back(s4);
  file_name.push_back(s5);
}

string 
SubmitListPacket::make_netstring() const
{
  string datastring = type + " ";

  for(unsigned int i = 0; i < file_name.size(); i++)
    {  //Maybe put the size of each element so they will be no char restriction
      datastring += "<" + file_name[i]  + ">";
    }

  string n_str;

  n_str = "SL";  //maybe use a small "s"
  
  n_str += datastring;
  cout<<n_str<<endl;
  return n_str; 
}

int 
SubmitListPacket::handle_packet( void* p_void)
{
  //  NetConnection* cc = (NetConnection*) p_void;

  //TODO : see if this is very usefull
  if( data == "" ) //Prevent from empty list
    return 1;

  int first_list_elem = data.find('<');
  
  type = data.substr(0, first_list_elem);

  data = data.substr(first_list_elem, data.length()-first_list_elem);

  while( 1 )
    {
      //Extract the robot name
      int begin = data.find('<');
      int end = data.find('>');
      if (begin == -1) return 1;

      file_name.push_back(data.substr(begin+1, end-begin-1));
      
      //Delete this string from the data string
      data = data.substr(end-begin+1, data.length()-end-1);
      if( data == "" )
	break;
    }
      
  istrstream is(type.c_str());
  string command;
  is >> command;

  if( command  == "SomeFriends")
    {
      string protocol;
      is >> protocol;

      map<int, PacketFactory*>::iterator it;
      
      for(it = my_socketserver.packet_factories.begin(); it != my_socketserver.packet_factories.end(); it++)
	{
	  if(protocol == it->second->Protocol())
	    { break; }
	}
     
      if( it != my_socketserver.packet_factories.end() ) //if not : I don't need it anymore...
	{
	  int local_channel = it->first;

	  string hostname; 
	  int port_num, dist_channel;

	  if(local_channel == SERVER_CONNECTION)
	    {
	      for(unsigned int i = 0; i < file_name.size(); i ++)
		{
		  istrstream is(file_name[i].c_str());
		  is>>hostname>>port_num>>dist_channel;
		  if( my_socketserver.not_connected_to(hostname, port_num))
		    {
		      NetConnection* nc = 
			my_socketserver.connect_to_an_other_server(hostname, port_num);
		      if( nc )
			{
			  //Either force him to accept my connection           (70%)
			  //Or tell him to redirect me on one of his friend    (30%)
			  //if(my_socketserver.by_type_connections[SERVER_CONNECTION].size() == 1)
			    nc->send_data( InitializationPacket( "Join" ).make_netstring() );
		      
			  nc->send_data( CommandPacket( "FreeConn" ).make_netstring() );
			}
		    }
		}
	    }
	  else  //if local_channel != SERVER_CONNECTION
	    {
	      //NOTE : right now I only get 1, but maybe this will change
	      istrstream is(file_name[0].c_str());
	      is>>hostname>>port_num>>dist_channel;
	      if(dist_channel != 0)
		{
		  NetConnection* nc = my_socketserver.connect_to_an_other_server(hostname, port_num);
		  if( nc ) {
		    nc->send_data( InitializationPacket( "Connect", protocol, dist_channel ).make_netstring());
		    my_socketserver.connection_factory[ nc ] = my_socketserver.packet_factory( local_channel );
		    my_socketserver.by_type_connections[ local_channel ].push_back( nc );
		  }
		}
	    }
	}
    }
  else 
    return 1;

  return 0;
}

