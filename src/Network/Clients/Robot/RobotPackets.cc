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

#include "NetConnection.h"
#include "RobotPackets.h"
#include "String.h"

Packet*
make_packet( string& netstr )
{
  string type = netstr.substr(0,2);

  if(type == "@C")
    {
      //This wouldn't be very usefull (CommandPacket would be a virtual class)
      return new CommandPacket( );
    }
  //else I don't know this kind of packet;

  return 0;
}

string 
InitializationPacket::make_netstring() const
{
  string datastring;

  if (protocol ==   RTB_NETPROTOCOL_V1)
    {
      datastring = "RTBNetV1";
    }
  else
    {
      datastring = "RTBNetUnknown";
    }
  datastring += " ";

  datastring += "Robot";
  
  datastring += " " + name + " ";

  string n_str;
  n_str = "@I";
  n_str += datastring;
  
  return n_str;
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
{
  NetConnection* nc = (NetConnection*) p_void;
  /* Parse the string... */
  if( data == "Close" && nc )
    nc->close_socket();
  return 0;
}



