#include <string>
#include <iostream.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <strstream>

#include "ArenaController.h"

#include "NetConnection.h"
#include "ServerSocket.h"
#include "TournamentAgreementPackets.h"

#include "Robot.h"
#include "Arena.h"

#include "String.h"

Packet*
TournamentAgreementPacketFactory::MakePacket( string& netstr )
{
  string type = netstr.substr(0,2);

  //  cout<<"TournamentAgreementPacketFactory::MakePacket\n";

  if     (type == "TC")    return new TournamentCommitChangePacket( &my_tournament );

  //else if(type == "@C")    return new CommandPacket( );
  //else if(type == "SL")    return new SubmitListPacket( );
  //else if(type == "@M")    return new ChatMessagePacket( );
  //else if(type == "@R")    return new RobotMessagePacket( );
  
  return NULL;
}

string 
TournamentCommitChangePacket::make_netstring() const
{
  //TODO : split it in 2 : type_of_change and value_of_change
  ostrstream os;
  os << change << ends;
  
  string n_str;
  n_str = "TC";
  
  n_str += string( os.str() );
  return n_str;
}

int 
TournamentCommitChangePacket::handle_packet(void* p_void)
{
  NetConnection* nc = (NetConnection*) p_void;
  istrstream is(data.c_str());
  string type_init;

  cout<<data<<endl;

  is >> type_init;           ;
  if(type_init == "AddRobot")
    {
      is >> change;
      cout<<"Add the following robot : "<<change<<endl;
      //tourn_p->current_match->...
    }
  else
    {
      return 1;
    }

  return 0;
}
