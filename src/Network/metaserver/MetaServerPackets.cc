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

#include <netinet/in.h>
#include <string>
#include <iostream.h>
#include <stdarg.h>

#include "MetaServerSocket.h"
#include "MetaServerNetConnection.h"
#include "MetaServerPackets.h"
#include "String.h"

Packet*
make_packet( string& netstr )
{
  string type = netstr.substr(0,2);

  /* How to use the get_8uint_from_netstring???
    switch(type)
    {
    case PACKET_INIT : return new InitializationPacket(); break;
    }
  */

  if(type == "MI")
    {
      return new MetaServerInitializationPacket( );
    }
  else if(type == "MD")
    {
      return new MetaServerDataPacket( );
    }
  else if(type == "MA")
    {
      return new MetaServerAskInfoPacket( );
    }
  //else I don't know this kind of packet;

  return 0;
}

/*
 * THIS ARE THE PACKETS THAT METASERVER SEND
 */

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

string
MetaServerDataPacket::make_netstring() const
{
  string datastring;
  
  datastring = name + " " + version + " " + address + " " + Port_num 
    + " " + Nb_Conn + "  " + language;

  string n_str;
  n_str = "MD"; 
  n_str += datastring;
  
  return n_str; 
}

/*
 * THIS ARE THE PACKETS THAT METASERVER RECEIVE
 */

int 
MetaServerInitializationPacket::handle_packet( void* ) 
{
  return 0;
  /* Make sure this is a RTB_V2 Net Protocol */
  /* Maybe just ask the protocol the server use... */
  //cout<<data<<endl;
}

int 
MetaServerDataPacket::handle_packet ( void* p_void)
{
  MetaServerNetConnection* msnc = (MetaServerNetConnection*) p_void;
  if( data != "" )  //Have to find a way not to do that
    {
      vector<string> wordlist;
      wordlist = split_string( data, wordlist ); //This function is buggy
      
      if( wordlist.size() == 5 )
	{
	    msnc->name = wordlist[0];
	    msnc->version = wordlist[1];
	    sscanf(wordlist[2].c_str(), "%d", &(msnc->port_number));
	    sscanf(wordlist[3].c_str(), "%d", &(msnc->nb_conn));
	    msnc->language = wordlist[4];
	    msnc->summary  = "";
	    msnc->is_server = true;
	}
    }

  return 0;
}

int 
MetaServerAskInfoPacket::handle_packet( void* nc )
{
  MetaServerNetConnection* msnc = (MetaServerNetConnection*) nc;
  
  list<MetaServerNetConnection*>::iterator i;
  
  for( i  = (my_metaserversocket->connected_servers).begin();
       i != (my_metaserversocket->connected_servers).end();
       i ++ )
    {
      if((*i)->is_server)
	{
	  msnc->send_data(  MetaServerDataPacket( (*i)->name, (*i)->version, 
						  (*i)->address, (*i)->port_number, 
						  (*i)->nb_conn, (*i)->language ).make_netstring() );
	}
    }

  if( ! (msnc->is_server) )
    {
      msnc->send_data( CommandPacket( "Close" ).make_netstring() );
      msnc->close_socket();
    }
  return 0;
}
