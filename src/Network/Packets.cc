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



#include <netinet/in.h>
#include <string>
#include <iostream.h>
#include <stdarg.h>

#include "NetConnection.h"
#include "Packets.h"
#include "String.h"


ostream& operator<<(ostream& ost, const Packet& P)
{
  ost<<P.make_netstring();
  return ost;
}

void
PacketFactory::add_connection( NetConnection* nc )
{
  my_connections.push_back( nc );
}

void
PacketFactory::remove_connection( NetConnection* nc )
{
  for(list<NetConnection*>::iterator li = my_connections.begin(); li != my_connections.end(); li ++)
    if((*li) == nc) {
      my_connections. erase( li );
      return;
    }
}

void
PacketFactory::broadcast( Packet* p )
{
  string data = p->make_netstring();
  
  for( list<NetConnection*>::iterator li = my_connections.begin(); li != my_connections.end(); li++ )
    (**li).send_data( data ); 
}

string&
Packet::add_string_to_netstring( const string& str, string& netstr ) const
{
  netstr += str + '\0';
  return netstr;
}
  
string&
Packet::add_uint8_to_netstring( const unsigned int& val, string& netstr ) const
{
  netstr += (char)(val & 0xff);
  return netstr;
}

/*
 * THIS TWO FUNCTIONS DON'T WORK ENOUGHT TO BE USED
 * The problem occurs when there is an 0x00 value in val
 */
string&
Packet::add_uint16_to_netstring( const unsigned int& val, string& netstr ) const
{ 
  unsigned short x = htons(val);

  char buf[3];
  x += 0x0101;  //This may be much better (for end of string)
  memcpy( buf, &x, 2 );
  buf[2] = '\0';
  netstr += buf;

  return netstr;
}

unsigned int 
Packet::get_uint8_from_netstring( const string& ns) const
{
  char buf[3];
  strcpy(buf, ns.substr(0,2).c_str());
  unsigned short x;
  memcpy(&x, buf, 2);
  x -= 0x0101;
  return ntohs(x);
}

/* 
 * Extract a string from a net string.
 * Useless since the NetConnection::read_data cut the long
 * socket string into a vector of string
 */

string
Packet::get_string_from_netstring( string& netstring ) 
{
  if(netstring.length() > 2)
    data = netstring.substr( 2, netstring.length()-2 );
  else
    data = "";

  return netstring;
}



