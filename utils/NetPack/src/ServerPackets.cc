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

Packet*
ServerPacketFactory::MakePacket( string& netstr, NetConnection* nc )
{
  string type = netstr.substr(0,2);
  string data = netstr.substr( 2, netstr.length()-2 );

  //else if(type == "@M")    return new ChatMessagePacket( );
  //else if(type == "@R")    return new RobotMessagePacket( );
  
  return NULL;
}

Packet*
InitPacketFactory::MakePacket( string& netstr, NetConnection* nc )
{
  string type = netstr.substr(0,2);
  string data = netstr.substr( 2, netstr.length()-2 );

  if (type == "@I")  return new InitializationPacket( data, nc );
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
InitializationPacket::handle_packet( )
{
  istrstream is(data.c_str());
  string type_init;

  is >> type_init >> protocol;

  if(type_init == "Connect") //He is already connected and he wants to open a other channel
    {
      is >> channel;

      PacketFactory* factory =  my_socketserver.packet_factory( channel );
      if(factory && factory->Protocol() == protocol)
	{
	  string more_arg;
	  is >> more_arg;

	  if(factory->add_connection( my_connection, more_arg ))
	    {
	      my_connection->set_type(channel);

	      my_socketserver.packet_factory( 0 )->remove_connection( my_connection );

	      my_socketserver. connection_factory[ my_connection ] = factory;

	      cout<<"New connection to "<<factory->Protocol()<<" on channel "<<channel<<endl;

	      return 1;
	    }
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
CommandPacket::handle_packet( )
{ /* Should be used by the server */

  cout<<data<<endl;

  istrstream is(data.c_str());
  string command;
  is >> command;

/*  if( command == "Quit" )
    {
      my_connection->close_socket();
      return 0;
    }
  else   */

  cout<<"Unknown command "<< command<<endl;

  //Should never come here :-)

  return 1;
}




