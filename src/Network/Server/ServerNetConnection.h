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


#ifndef __SERVER_NET_CONNECTION__
#define __SERVER_NET_CONNECTION__

#include "NetConnection.h"

class CommandPacket;

class ServerNetConnection : public NetConnection {
 public:
  void close_socket();
  virtual client_t type_conn() { return UNINITIALIZED; } 
};

class Robot_Connection : public ServerNetConnection {
 public:
  Robot_Connection(int);
  Robot_Connection(ServerNetConnection&);
  ~Robot_Connection();
  client_t type_conn() { return ROBOT_CLIENT_CONNECTION; }
};

class Chat_Connection : public ServerNetConnection {
 public:
  Chat_Connection(int);
  Chat_Connection(ServerNetConnection&);
  ~Chat_Connection();
  client_t type_conn() { return CHAT_CLIENT_CONNECTION; }
};

extern Chat_Connection* root_client;

#endif
