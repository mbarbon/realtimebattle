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


#ifndef __METASERVER_H__
#define __METASERVER_H__

#include <list>
#include <string>

#include "MetaServerNetConnection.h"
#include "MetaServerPackets.h"
#include "Console.h"

void exit_cleanly(int);
void quit();

class MetaServerSocket {
 public:
  MetaServerSocket() {};
  ~MetaServerSocket();

  void open_socket();
  void close_socket();

  void check_socket();

  void go_through_read_buffers();

  friend class MetaServerAskInfoPacket;

  void handle_console_command( string );
  Console my_cons;

 protected:
  void accept_connection();
  void remove_unconnected_sockets();

  list<MetaServerNetConnection*> connected_servers;
  int next_id;
  int server_socket;
  


};

extern MetaServerSocket* my_metaserversocket;

#endif
