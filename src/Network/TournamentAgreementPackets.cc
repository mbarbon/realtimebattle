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

  if     (type == "TC")    return new TournamentCommitChangePacket( &my_tournament, this );

  //else if(type == "@C")    return new CommandPacket( );
  //else if(type == "SL")    return new SubmitListPacket( );
  //else if(type == "@M")    return new ChatMessagePacket( );
  //else if(type == "@R")    return new RobotMessagePacket( );
  
  return NULL;
}

void 
TournamentAgreementPacketFactory::add_connection( NetConnection* nc )
{
  //TODO : Send all the arenas I have to the players...
  //TODO : send all the robots that are already on my list...
  my_connections.push_back( nc );

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

  is >> type_init;           ;
  if(type_init == "AddRob")
    {
      string dir, name;
      int team, id;
      is >> dir >> name >> team >> id;
      cout<<"Add the following robot : "<<dir<<name<<", "<<team<<"  id : "<<id<<endl;
      
      robot_info_t the_robot(dir, name, team, nc);
      tourn_p->robots.push_back(the_robot);
      ostrstream os;
      os << type_init << " " << dir <<" "<< name << " " << team << " " <<the_robot.id;
      change = os . str();
      my_factory->broadcast( this );
      
    }
  else if(type_init == "RemRob")
    {
      string dir, name;
      int team, id;
      is >> dir >> name >> team >> id;
      cout<<"Remove the following robot : "<<dir<<name<<", "<<team<<"  id : "<<id<<endl;
      
      for(list<robot_info_t>::iterator ri = tourn_p->robots.begin();
	  ri != tourn_p->robots.end(); ri ++ )
	if( ri->id == id && 
	    (ri->filename == name && ri->directory == dir && ri->team == team))  //Robot found
	  {
	    tourn_p->robots.erase( ri );

	    ostrstream os;
	    os << type_init << " " << dir <<" "<< name << " " << team << " " <<id;
	    change = os . str();
	    my_factory->broadcast( this );
	  }
	else
	  {
	    cout<<"Robot not found\n";
	  }
    }
  else if(type_init == "RemArn" || type_init == "AddArn")
    {
      string dir, name;
      int id;
      is >> dir >> name >> id;
      cout<<"Switch the following arena : "<<dir<<name<<", id : "<<id<<endl;
      
      for(list<arena_info_t>::iterator ai = tourn_p->arenas.begin();
	  ai != tourn_p->arenas.end(); ai ++ )
	if( ai->id == id && 
	    (ai->filename == name && ai->directory == dir))  //Arena found
	  {
	    if(type_init == "RemArn")
	      ai -> selected = false;
	    else
	      ai -> selected = true;

	    ostrstream os;
	    os << type_init << " " << dir <<" "<< name << " " <<id;
	    change = os . str();
	    my_factory->broadcast( this );
	  }
	else
	  {
	    cout<<"Arena not found\n";
	  }
    }

  else
    {
      return 1;
    }

  return 0;
}
