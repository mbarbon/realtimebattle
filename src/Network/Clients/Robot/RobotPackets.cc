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



