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



#ifndef __SOCKETSERVER_H__
#define __SOCKETSERVER_H__

#include <list>
#include <string>

#include "ServerPackets.h"
#include "NetConnection.h"
#include "ServerNetConnection.h"

void exit_cleanly(int);
void quit();

class SocketServer
{
public:
  SocketServer() 
    : next_id(0), MetaServer(0), 
    name("A_Server"), language("fr"), version("RTB_v2.0.0"), 
    robot_nb(0), chat_nb(0) 
    {}
  ~SocketServer();
  void open_socket( int port_number = 0 );
  void close_socket();

  void check_socket();
  void connect_to_metaserver( string, int port_number = 0 );

  void go_through_read_buffers();

private:
  void accept_connection();
  void remove_unconnected_sockets();

  void send_packet_by_name(string, ServerMessagePacket* );
  void send_packet_by_type(client_t, Packet* );

  int next_id;

  NetConnection* MetaServer;

  string name;
  string language;
  string version;

  int robot_nb;  //Number of robots and chats
  int chat_nb;
  list<ServerNetConnection*> connected_clients;

  int port_number;
  int server_socket;
};

#endif __SOCKETSERVER_H__
