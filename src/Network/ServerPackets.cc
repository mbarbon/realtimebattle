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

  is >> type_init >> protocol;

  if(type_init == "Connect") //He is already connected and he wants to open a other channel
    {
      is >> channel;
      cout<<"He wants to connect to protocol "<<protocol<<" on channel "<<channel<<endl;

      PacketFactory* factory =  my_socketserver.packet_factory( channel );
      if(factory && factory->Protocol() == protocol)
	{
	  nc->set_type(channel);

	  factory->add_connection( nc );
	  my_socketserver. connection_factory[ nc ] = factory;
	  cout<<factory->Protocol()<<endl;

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
      /*
      int av = 0;
      for(map<NetConnection*, ServerState>::iterator it =my_socketserver.server_states.begin();
	  it != my_socketserver.server_states.end(); it ++)
	av += (*it).second.nb_conn;

      ostrstream os;
      os<<"FreeConn "
	<< my_socketserver.by_type_connections[SERVER_CONNECTION].size() << " " 
	<< av << ends;
      datastring = os.str();
      */
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
    /*
    vector<string> vs;
    vs = my_socketserver.give_other_addresses(cc, protocol);

    if(vs.size() > 0)
      {
	SubmitListPacket P( os.str(), vs );
	cc->send_data( P.make_netstring() );
      }
    */
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
    /*
    int nb_friends, nb_conn_of_friends;
    is >> nb_friends >> nb_conn_of_friends;
    my_socketserver.nb_conn_friends += nb_friends - my_socketserver.server_states[cc].nb_conn;
    my_socketserver.server_states[cc].set_nb_conn (nb_friends, nb_conn_of_friends);
    */
  }
  else if( command == "Quit" )
    {
      cc->close_socket();
      return 0;
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

  return 0;
}

