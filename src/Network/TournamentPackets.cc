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
TournamentRobotPacketFactory::MakePacket( string& netstr )
{
  string type = netstr.substr(0,2);

  //if     (type == "CO")    return new InitializationPacket( );
  //else if(type == "@C")    return new CommandPacket( );
  //else if(type == "SL")    return new SubmitListPacket( );

  //else if(type == "@M")    return new ChatMessagePacket( );
  //else if(type == "@R")    return new RobotMessagePacket( );
  
  return NULL;
}

Packet*
TournamentViewPacketFactory::MakePacket( string& netstr )
{
  string type = netstr.substr(0,2);

  //if     (type == "CO")    return new InitializationPacket( );
  //else if(type == "@C")    return new CommandPacket( );
  //else if(type == "SL")    return new SubmitListPacket( );

  //else if(type == "@M")    return new ChatMessagePacket( );
  //else if(type == "@R")    return new RobotMessagePacket( );
  
  return NULL;
}

