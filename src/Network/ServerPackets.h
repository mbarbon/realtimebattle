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



#ifndef __SERVER_PACKET_H__
#define __SERVER_PACKET_H__

#include "Packets.h"
#include "NetConnection.h"

class ServerPacketFactory: public PacketFactory {
 public:
  string Protocol() {return RTB_PROTOCOL_VERSION_STR ;};
  Packet* MakePacket(string &, NetConnection*); 
};

class InitPacketFactory: public PacketFactory {
 public:
  string Protocol() {return "not a protocol" ;};
  Packet* MakePacket(string &, NetConnection*); 
};

extern ServerPacketFactory my_serverpacketfactory;

class InitializationPacket : /* virtual */ public Packet
{
public:
  InitializationPacket( )  {};

  InitializationPacket( const string& data, NetConnection* nc )
    : Packet(data, nc) {}

  InitializationPacket( const string& t,
			const string& p,
			const int& c)
    : type_init(t), protocol(p), channel(c) {}

  string make_netstring() const;
  int handle_packet( ) ; 

  packet_t packet_type() { return PACKET_INIT; };

protected:
  string type_init, protocol;
  int channel;
};


class CommandPacket : /* virtual */ public Packet
{
public:
  CommandPacket()
    : comm("") {}
  CommandPacket( const string& data, NetConnection* nc ) 
     : Packet(data, nc) {} ;

  CommandPacket( const string& c );

  CommandPacket( const string& c, vector<string>& arg) :
    comm(c), arg(arg) {}

  string make_netstring() const;
  int handle_packet( );

  packet_t packet_type() { return PACKET_COMMAND; };

protected:
  string comm;
  vector<string> arg;
};

class FactoryInfoPacket : /* virtual */ public Packet
{
public:
  FactoryInfoPacket( const string& data, NetConnection* nc ) 
     : Packet(data, nc) {} ;


  FactoryInfoPacket()
    : protocol("No_Protocol"), channel( 0 ) {}
  FactoryInfoPacket( const string& p, int c)
    : protocol( p ), channel( c ) {}
  string make_netstring() const;
  int handle_packet( );
  packet_t packet_type() { return PACKET_FACTORY_INFO; };
protected:
  string protocol;
  int channel;
};


class SubmitListPacket : public Packet
{
public:
  SubmitListPacket( const string& data, NetConnection* nc ) 
    : Packet(data, nc) {} ;

  SubmitListPacket( ) {};  

  SubmitListPacket( string /* type */, const vector<string>& );
  // Hopefully no more than five strings are needed, if so add a new one!
  SubmitListPacket( string /* type */, string& ); // one string
  SubmitListPacket( string /* type */, string&, string& ); // two strings
  SubmitListPacket( string /* type */, string&, string&, string& ); // three strings
  SubmitListPacket( string /* type */, string&, string&, string&, string& ); // four strings
  SubmitListPacket( string /* type */,
                string&, string&, string&, string&, string& ); // five strings

  string make_netstring() const;
  int handle_packet( );
  packet_t packet_type() { return PACKET_SUBMIT; }
  friend class SocketServer;
protected:
  string type;
  vector<string> file_name;
};



#endif
