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



#ifndef __TOURN_AGREE_PACKET_H__
#define __TOURN_AGREE_PACKET_H__

#include "Packets.h"
#include "Structs.h"

class TournamentAgreementPacketFactory: public PacketFactory {
 public:
  TournamentAgreementPacketFactory();
  string Protocol() {return RTB_PROTOCOL_TOURNAMENT_AGREEMENT ;};
  void add_connection( NetConnection* );
  void set_start(NetConnection*, bool);
  void reset_start();
  void remove_connection( NetConnection* );
  void start_tournament( bool = false );

  Packet* MakePacket(string &); 
 protected:
  tourn_info_t my_tournament;
  map<NetConnection*, bool> ready_to_start;
};

class TournamentCommitChangePacket : /* virtual */ public Packet {
public:
  TournamentCommitChangePacket(tourn_info_t* tourn, TournamentAgreementPacketFactory* f)
    : tourn_p(tourn), change(""), my_factory( f ) {};
  TournamentCommitChangePacket( const string& c ) 
    : change(c)  {};
  TournamentCommitChangePacket( const string& t, const string& c)
    : type_init( t ), change( c ) {};
  string make_netstring() const;
  int handle_packet( void* );
  packet_t packet_type() { return PACKET_TOURNAMENT_COMMIT_CHANGE; };

  string& type()  {return type_init;}
  string& value() {return change;}
protected:
  tourn_info_t *tourn_p;
  string type_init;
  string change;
  TournamentAgreementPacketFactory* my_factory;
};

class StartTournamentPacket : /* virtual */ public Packet {
public:
  StartTournamentPacket(TournamentAgreementPacketFactory* f)
    : start(false), my_factory( f ) {};
  StartTournamentPacket( bool start ) 
    : start(start)  {};

  string make_netstring() const;
  int handle_packet( void* );
  packet_t packet_type() { return PACKET_START_TOURNAMENT; };

protected:
  bool start;
  TournamentAgreementPacketFactory* my_factory;
};

#endif
