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
static const int max_nb_packet_at_one_time = 32; //Not to be overfitted

static const int max_number_connections = 10;   //NOTE : Remove this


#define client_t int  
#define UNINITIALIZED_CONN   0


//TODO : Make the netconnection work as a stream...

class NetConnection {
public:
  NetConnection         ()
    : id(0), the_socket(-1), size_buf( 0 ),
    connected(false), state(NOT_INITIALIZED), type(UNINITIALIZED_CONN)
    {}
  virtual ~NetConnection() { close_socket(); }

  virtual void close_socket     ();
  
  int read_data         ();
  int write_data        ();

  // ?? get_data        (??)
  void send_data        ( const string& data );

  void make_nonblocking ();

  int get_type() {return type;}
  void set_type(int t) {type = t;};

  bool is_not_connected () { return !connected; }
  bool is_initialized   () { return state == INITIALIZED; } //Don't think we need it here
  bool is_in_game       () { return state == IN_GAME; }


  //NOTE : Do we really need all this... 
  friend class SocketServer;
  friend class MetaServerSocket;
  friend class SocketClient;
  friend class SocketChat;
  friend class SocketRobot;

  friend class CommandPacket;
  friend class ChatMessagePacket;    //Only use name
  friend class InitializationPacket; //Only use name
  friend class SubmitListPacket;    //Use address and the_socket

protected:
  enum connection_state { NOT_INITIALIZED, INITIALIZED, IN_GAME }; //Is it usefull here ?

  int id;
  int the_socket;

  unsigned short int size_buf;

  //TODO ? map<int, deque<string>> read_buffers;   //All the read but not parsed packets
  deque<string>  read_buffers;   //All the read but not parsed packets
  deque<string>  write_buffers;  //All the buffers to send next time
  bool connected;
  connection_state state;  //Do we need it ?

  int type;
};

#endif // __NETCONNECTION_H__
