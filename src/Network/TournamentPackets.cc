#include <string>
#include <iostream.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <strstream>

#include "NetConnection.h"
#include "ServerSocket.h"
#include "TournamentPackets.h"


#include "Robot.h"
#include "Arena.h"

#include "String.h"

Packet*
TournamentRobotPacketFactory::MakePacket( string& netstr, NetConnection* nc )
{
  string type = netstr.substr(0,2);
  string data = netstr.substr(2, netstr.length() - 2);

  if     (type == "OM")    return new OldRTBMessagePacket( data, nc );
  else if(type == "@R")    return new RobotCommandPacket( data, nc );

  //else if(type == "@C")    return new CommandPacket( );
  //else if(type == "SL")    return new SubmitListPacket( );

  //else if(type == "@M")    return new ChatMessagePacket( );
  //else if(type == "@R")    return new RobotMessagePacket( );
  
  return NULL;
}

bool
TournamentRobotPacketFactory::add_connection( NetConnection* nc, string more_arg)
{
  Robot* r;
  r = my_tournament->connect_to_robot(nc, more_arg) ;
  if( r != NULL )
    {
      PacketFactory::add_connection( nc, more_arg );
      robot_connection[nc] = r;

      return true;
    }
  return false;
}

Packet*
TournamentViewPacketFactory::MakePacket( string& netstr, NetConnection* nc )
{
  string type = netstr.substr(0,2);

  //if     (type == "CO")    return new InitializationPacket( );
  //else if(type == "@C")    return new CommandPacket( );
  //else if(type == "SL")    return new SubmitListPacket( );

  //else if(type == "@M")    return new ChatMessagePacket( );
  //else if(type == "@R")    return new RobotMessagePacket( );
  
  return NULL;
}


string OldRTBMessagePacket::make_netstring() const
{
  return string("OM");
}

int OldRTBMessagePacket::handle_packet( )
{
}

string RobotCommandPacket::make_netstring() const
{
  return string("@R");
}

int RobotCommandPacket::handle_packet( )
{
}
