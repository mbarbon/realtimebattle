#include <netinet/in.h>
#include <string>
#include <iostream.h>
#include <stdarg.h>

#include "NetConnection.h"
#include "Packets.h"
#include "String.h"

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



