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


#ifndef __CLIENT_SOCKET_H__
#define __CLIENT_SOCKET_H__

#include "NetConnection.h"
#include "Packets.h"
#include <string>

class SocketClient {
 public:
  SocketClient() : my_connection( NULL ), his_packet_factory( NULL ) {}
  virtual ~SocketClient();
  void set_packet_factory( PacketFactory* );
  NetConnection* connect_to_server( string hostname, int port_nb = 0 );
  int check_socket();
  virtual void handle_input_stream(char*) {};
 protected:
  NetConnection* my_connection;
  PacketFactory* his_packet_factory;
};

#endif //__CLIENT_SOCKET_H__
