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

string
Packet::get_string_from_netstring( string& netstring ) 
{
  if(netstring.length() < 4)  //Bad packet (len or type not read)
    {
      return string("");
    }

  size = get_uint8_from_netstring( netstring.substr(2,2) );

  data = netstring.substr(4, size);

  //Remove data from netstring
  if (netstring.length() != size+1)
    {
      netstring = netstring.substr(size+4, netstring.length() - (size+4) );
    }
  else
    {
      netstring = "";
    }

  return netstring;
}



