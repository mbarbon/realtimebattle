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

#ifndef __META_SERVER_NET_CONNECTION__
#define __META_SERVER_NET_CONNECTION__

#include <string>
#include "NetConnection.h"

class MetaServerDataPacket;
class MetaServerAskInfoPacket;

class MetaServerNetConnection : public NetConnection {
 public:
  MetaServerNetConnection() :
    name( "" ), is_server( false ) {};
  void close_socket();

  friend class MetaServerSocket;

  friend class MetaServerDataPacket;
  friend class MetaServerAskInfoPacket;
  
 protected:
  string name;
  string language;
  string summary;
  int port_number;
  int nb_conn;
  string version;
  string address;
  bool is_server;
};

#endif
