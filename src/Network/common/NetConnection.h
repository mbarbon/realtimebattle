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


#ifndef __NETCONNECTION_H__
#define __NETCONNECTION_H__

#include <vector>
#include <queue>
#include <string>

static const int max_packet_length = 512;

static const int server_port = 4147;
static const int meta_server_port = 4146;
static const string server_host = "localhost";
static const string meta_server_host = "localhost";
static const int max_number_connections = 10;

enum network_protocol_t
{
  RTB_UNKNOWN,
  RTB_NETPROTOCOL_V1
};

typedef enum client_t
{
  ROBOT_CLIENT_CONNECTION = 0,
  CHAT_CLIENT_CONNECTION,
  ROOT_CLIENT_CONNECTION,
  UNINITIALIZED
};

class NetConnection {
public:
  NetConnection         ()
    : id(0), the_socket(-1),/* address(""),*/ connected(false), state(NOT_INITIALIZED) {}
  virtual ~NetConnection() { close_socket(); }

  virtual void close_socket     ();

  int read_data         ();
  int write_data        ();

  // ?? get_data        (??)
  void send_data        ( const string& data );

  void make_nonblocking ();

  bool is_not_connected () { return !connected; }
  bool is_initialized   () { return state == INITIALIZED; }
  bool is_in_game       () { return state == IN_GAME; }

  friend class SocketServer;
  friend class MetaServerSocket;
  friend class SocketClient;
  friend class SocketChat;
  friend class SocketRobot;

  friend class CommandPacket;
  friend class ChatMessagePacket;    //Only use name
  friend class InitializationPacket; //Only use name
  friend class LaunchRobotPacket;    //Use address and the_socket

protected:
  enum connection_state { NOT_INITIALIZED, INITIALIZED, IN_GAME };

  int id;
  int the_socket;
  //  string address;

  deque<string> read_buffers; //All the read but not parsed packets
  deque<string> write_buffers;  //All the buffers to send next time
  bool connected;
  connection_state state;

  string name;
};

#endif // __NETCONNECTION_H__
