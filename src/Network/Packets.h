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

#ifndef __PACKETS_H__
#define __PACKETS_H__

#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <list>

#include "NetConnection.h"


class SocketServer;

enum packet_t
{
  PACKET_INIT = 1, //Otherwise we gonna have problems with eof '\0'
  PACKET_META_INIT,
  PACKET_INIT_REPLY,

  PACKET_META_DATA,
  PACKET_META_INFO,

  PACKET_COMMAND,
  PACKET_FACTORY_INFO,
  PACKET_SERVER_COMMAND,

  PACKET_SIGNAL,
  PACKET_FORCE_KILL,

  PACKET_ARENA,
  PACKET_LAUNCH_ROBOT, //Maybe use the submit packet too...

  PACKET_SUBMIT,
  PACKET_TOURNAMENT_COMMIT_CHANGE,
  PACKET_START_TOURNAMENT,

  PACKET_OLD_RTB_MESSAGE,

  PACKET_WARNING,       //Send a warning message... (should be used by everybody...)
};

#define RTB_PROTOCOL_VERSION_STR           string("RTB_NET_Server_devel")
#define RTB_PROTOCOL_TOURNAMENT_AGREEMENT  string("RTB_NET_Agree_0.1")
#define RTB_PROTOCOL_TOURNAMENT_ROBOT      string("RTB_NET_Robot_0.0")
#define RTB_PROTOCOL_TOURNAMENT_VIEW       string("RTB_NET_View_0.0")

class Packet
{
public:
  Packet( ) 
    : data( "" ), my_connection(NULL) {};
  Packet( const string& d, NetConnection* nc) 
    : data( d ), my_connection(nc) {};
 
  virtual ~Packet() {};

  virtual string make_netstring() const {return "";};  //Get what is to send in the socket
  string operator()() const { return make_netstring(); } //Do I need it ?

  virtual int handle_packet() {return 0;};
  virtual packet_t packet_type() = 0;                  //Return the type of the packet (See list below)

protected:
  string data;       //The data in the packet when reveived
  NetConnection* my_connection;
};

//This class have to be virtual (we can build a plug-in loader around it) 
class PacketFactory {
 public:
  PacketFactory() : close_me( false ) {};
  virtual ~PacketFactory() {};
  virtual string Protocol() {return "NoProtocol";};
  virtual Packet* MakePacket(string & s, NetConnection*) {return NULL;};

  virtual bool add_connection( NetConnection*, string = string(""));
  virtual void remove_connection( NetConnection* );

  void broadcast( Packet* );

 protected:
  //TODO : set< NetConnection* >
  list<NetConnection*> my_connections;
  bool close_me;
};

#endif // __PACKETS_H__
