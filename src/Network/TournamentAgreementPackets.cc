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
#include "TournamentPackets.h"

#include "Tournament.h"
#include "EventHandler.h"
#include "ServerSocket.h"

#include "Robot.h"
#include "Arena.h"

#include "String.h"

//TODO : remove the robots added by a closed connection ...

TournamentAgreementPacketFactory::TournamentAgreementPacketFactory()
  : PacketFactory()
{
  accept_new_connection = true;

  //Use local arenas and distant robots !
  /*
    TODO : Uncomment this code ...

    list<string> robotdirs;
    list<string> arenadirs;
    
    read_dirs_from_system( robotdirs, arenadirs );
    
    for(list<arena_info_t>::iterator li = my_tournament.arenas.begin() ; 
    li != my_tournament.arenas.end() ; li++ )
    {
    DIR* dir;
    if( NULL != (dir = opendir((*li).c_str())))
    {
    struct dirent * entry;
    while (NULL != ( entry = readdir( dir ) ) )
    {
    string full_file_name = (*li) + entry->d_name;
    if( check_if_filename_is_arena( full_file_name ) )
    {
    dir_list->push_back( arena_info_t( entry->d_name, *li ) );
    }
    closedir( dir );
    }
    }
    }
  */

  my_tournament.arenas.push_back(arena_info_t("Original.arena", "/home/roussebe/RealTimeBattle/Arenas/"));
  my_tournament.arenas.push_back(arena_info_t("Arena1", "/arenas/"));
  my_tournament.arenas.push_back(arena_info_t("Arena3", "/arenas/"));
}

Packet*
TournamentAgreementPacketFactory::MakePacket( string& netstr, NetConnection * nc )
{
  string type = netstr.substr(0,2);
  string data = netstr.substr(2, netstr.length() - 2 );

  if     (type == "TC")    return new TournamentCommitChangePacket( data, nc, &my_tournament, this );
  else if(type == "ST")    return new StartTournamentPacket( data, nc, this );

  //else if(type == "SL")    return new SubmitListPacket( );
  //else if(type == "@M")    return new ChatMessagePacket( );
  //else if(type == "@R")    return new RobotMessagePacket( );
  
  return NULL;
}

bool 
TournamentAgreementPacketFactory::add_connection( NetConnection* nc, string more_arg )
{
  //Send all the arenas I have to the players...
  for( list<arena_info_t>::iterator ai = my_tournament.arenas.begin();
       ai != my_tournament.arenas.end(); ai ++ )
    {
      ostrstream os;
      os<<"InitArn "<<ai->directory<<" "<<ai->filename<<" "<<ai->id<<" "<<ai->selected<<ends;
      TournamentCommitChangePacket P( os.str() );
      nc->send_data( P.make_netstring() );
    }

  //Send all the robots that are already on my list...
  for( list<robot_info_t>::iterator ri = my_tournament.robots.begin();
       ri != my_tournament.robots.end(); ri ++ )
    {
      ostrstream os;
      os<<"AddRob " << ri->directory <<" "<< ri->filename << " " << ri->team << " " <<ri->id;
      TournamentCommitChangePacket P( os.str() );
      nc->send_data( P.make_netstring() );
    }

  my_connections.push_back( nc );
  ready_to_start[ nc ] = false;
  return true;
}

void
TournamentAgreementPacketFactory::set_start(NetConnection* nc, bool start)
{
  ready_to_start[ nc ] = start;
  if( start )  //If I accepted, maybe I'm the last one to accept : try to run the tournament
    start_tournament( true );
}

void
TournamentAgreementPacketFactory::reset_start( )
{
  //All the ready_to_start to false ; used if some chatter said 'start' but something changed in the tournament_info
  for(map<NetConnection*, bool>::iterator mi = ready_to_start.begin();
      mi != ready_to_start.end(); mi ++)
    mi->second = false;
}

void
TournamentAgreementPacketFactory::remove_connection( NetConnection* nc )
{
  //Remove the connection from my_connection;
  PacketFactory::remove_connection( nc );

  //Remove connection from the ready_to_start map
  bool all_true = true;
  for(map<NetConnection*, bool>::iterator mi = ready_to_start.begin();
      mi != ready_to_start.end(); mi ++) {
    if(mi->first == nc) {
      ready_to_start.erase( mi );
      break;
    }
    else {
      all_true = all_true && mi->second;
    }
  }

  //TODO : remove the robot added by this connection...

  //Run the tournament if everybody agree
  if( all_true )
    start_tournament( );
}

void
TournamentAgreementPacketFactory::start_tournament(bool check_everybody_ready = false)
{
  bool all_true = true;
  if(check_everybody_ready)  //Check if all the connection are ready (maybe it's allready done...)
    {
      for(map<NetConnection*, bool>::iterator mi = ready_to_start.begin();
	  mi != ready_to_start.end(); mi ++) {
	all_true = all_true && mi->second;
      }
    }

  all_true = all_true && (my_tournament.robots.size() != 0);  //Need at least one robot (need a better test : it depends on the arena type) 

  //Need at least one arena to start the tournament;
  bool one_arena_at_least = false;
  for(list<arena_info_t>::iterator ai = my_tournament.arenas.begin();
      ai != my_tournament.arenas.end() ; ai ++ )
    {
      if(ai -> selected)
	{
	  one_arena_at_least = true;
	  break;
	}
    }

  all_true = all_true && one_arena_at_least;

  if( all_true )  //Everything is ok to start
    {
      accept_new_connection = false;
      Tournament *T = new Tournament ( my_tournament );

      PacketFactory * View; PacketFactory * Robot;
      //Create a Robot PacketFactory
      int robot_channel = my_socketserver.open_channel( Robot = new TournamentRobotPacketFactory( T ) );

      //Create a View PacketFactory
      int view_channel  = my_socketserver.open_channel( View = new TournamentViewPacketFactory( T ) );

      T->set_packet_factories((TournamentRobotPacketFactory *) Robot,
			      (TournamentViewPacketFactory *)  View);

      //Tell them to start and where to connect !
      Packet* P    = new StartTournamentPacket( true );
      Packet* FI_1 = new FactoryInfoPacket( Robot->Protocol(), robot_channel );
      Packet* FI_2 = new FactoryInfoPacket( View->Protocol(), view_channel );

      broadcast( P );
      broadcast( FI_1 );
      broadcast( FI_2 );

      delete P;
      delete FI_1;
      delete FI_2;      

      T->start();
      
      //Release the connections !
      my_socketserver.uninitialize_connections( my_connections );

      
      //Close me !
      my_connections.clear();

      the_eventhandler.set_tournament( T );
    }
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
TournamentCommitChangePacket::handle_packet()
{
  istrstream is(data.c_str());
  string type_init;

  bool modified = false;

  is >> type_init;
  if(type_init.substr(3, 6) == "Rob")
    {
      //cout<<type_init.substr(3, 6)<<endl;
    }
  if(type_init == "AddRob")
    {
      string dir, name;
      int team, id;
      is >> dir >> name >> team >> id;
      cout<<"Add robot : "<<dir<<name<<", "<<team<<"  id : "<<id<<endl;
      
      robot_info_t the_robot(name, dir, team, my_connection);
      tourn_p->robots.push_back(the_robot);
      ostrstream os;
      os << type_init << " " << dir <<" "<< name << " " << team << " " <<the_robot.id <<ends;
      change = os . str();
      my_factory->broadcast( this );
      modified = true;
    }
  else if(type_init == "RemRob")
    {
      string dir, name;
      int team, id;
      is >> dir >> name >> team >> id;
      cout<<"Remove robot : "<<dir<<name<<", "<<team<<"  id : "<<id<<endl;
      
      for(list<robot_info_t>::iterator ri = tourn_p->robots.begin();
	  ri != tourn_p->robots.end(); ri ++ )
	if( ri->id == id && 
	    (ri->filename == name && ri->directory == dir && ri->team == team))  //Robot found
	  {
	    tourn_p->robots.erase( ri );

	    ostrstream os;
	    os << type_init << " " << dir <<" "<< name << " " << team << " " <<id<<ends;
	    change = os . str();
	    my_factory->broadcast( this );
	    modified = true;
	  }
    }
  else if(type_init == "RemArn" || type_init == "AddArn")
    {
      string dir, name;
      int id;
      is >> dir >> name >> id ;
      
      for(list<arena_info_t>::iterator ai = tourn_p->arenas.begin();
	  ai != tourn_p->arenas.end(); ai ++ )
	if( ai->id == id && 
	    (ai->filename == name && ai->directory == dir))  //Arena found
	  {
	    cout<<"Switch arena : "<<dir<<name<<", id : "<<id<<endl; //Not such a good idea !

	    if(type_init == "RemArn")
	      ai -> selected = false;
	    else
	      ai -> selected = true;

	    ostrstream os;
	    os << type_init << " " << id << " " << ai->selected << ends ;
	    change = os . str();
	    my_factory->broadcast( this );
	    modified = true;
	  }
    }
  if( modified )
    {
      //Reset all the connection that have accepted the setting because they may not accept the new conditions
      my_factory->reset_start( );
      return 0;
    }
  else
    return 1;
}

string 
StartTournamentPacket::make_netstring() const
{
  ostrstream os;
  os << start << ends;
  
  string n_str;
  n_str = "ST";
  
  n_str += string( os.str() );
  return n_str;
}

int 
StartTournamentPacket::handle_packet()
{
  istrstream is(data.c_str());

  is>>start;

  my_factory->set_start(my_connection, start);
  return 0;
}
