/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2001  Erik Ouchterlony and Ragnar Ouchterlony

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
  PACKET_SERVER_COMMAND,

  PACKET_SIGNAL,
  PACKET_FORCE_KILL,

  PACKET_ARENA,
  PACKET_LAUNCH_ROBOT, //Maybe use the submit packet too...

  PACKET_SUBMIT,
  PACKET_TOURNAMENT_COMMIT_CHANGE,
  

  PACKET_WARNING,       //Send a warning message... (should be used by everybody...)
};

#define RTB_PROTOCOL_VERSION_STR string("RTB_NET_Server_devel")
#define RTB_PROTOCOL_TOURNAMENT_AGREEMENT string("RTB_NET_Agree_0.0")


class Packet
{
public:
  Packet( ) : data( "" ), size(0) {};
  Packet( string& d ) : data( d ) {};          //NOTE : USELESS
  virtual ~Packet() {};
  virtual string make_netstring() const {return "";};  //Get what is to send in the socket
  //string operator()() const { return make_netstring(); } //Do I need it ?
  string get_string_from_netstring( string& );         //Is it still in use???
  void remove_from_netstring( string& );

  virtual int handle_packet(void*) {return 0;};        //What is to do when we receive the packet
  virtual packet_t packet_type() = 0;                  //Return the type of the packet (See list below)

  friend ostream & operator<<(ostream&, const Packet&);

protected:
  //All this functions are not used anymore because there is problems with the '\0' char
  string& add_string_to_netstring( const string&, string& ) const;      
  string& add_uint8_to_netstring( const unsigned int&, string& ) const;
  string& add_uint16_to_netstring( const unsigned int&, string& ) const;
  unsigned int get_uint8_from_netstring( const string& ) const;
  unsigned int get_uint16_from_netstring( const string& ) const;
  
  string data;       //The data in the packet when reveived
  unsigned int size; //The size of the packet... is it very useful???
};

ostream & operator<<(ostream&, const Packet&);


//This class have to be virtual (we can build a plug-in loader around it) 
class PacketFactory {
 public:
  virtual string Protocol() {return "NoProtocol";};
  virtual Packet* MakePacket(string & s) {return NULL;};
  virtual void add_connection( NetConnection* );

 protected:
  //TODO : set< NetConnection* >
  list<NetConnection*> my_connections;
};

#endif // __PACKETS_H__
