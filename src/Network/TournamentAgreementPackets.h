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

class TournamentAgreementPacketFactory: public PacketFactory {
 public:
  /* TODO : construct this class with a GuiInterface pointer as argument... */
  string Protocol() {return RTB_PROTOCOL_TOURNAMENT_AGREEMENT ;};
  Packet* MakePacket(string &); 
};

extern TournamentAgreementPacketFactory my_tournament_agreement_packetfactory;

class TournamentCommitChangePacket : /* virtual */ public Packet {
public:
  TournamentCommitChangePacket()
    : change("") {};
  TournamentCommitChangePacket( const string& c ) 
    : change(c)  {};
  string make_netstring() const;
  int handle_packet( void* );
  packet_t packet_type() { return PACKET_TOURNAMENT_COMMIT_CHANGE; };
protected:
  string change;
};

#endif
