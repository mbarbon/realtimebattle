/*
NetPack for RealTimeBattle
Copyright (C) 2002  Benoit Rousseau

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
#include <map>

#include "NetConnection.h"
#include "Packets.h"
#include "SocketHandler.h"

#define NB_FRIENDS_IN_FILE  10      //NOTE : Maybe get this from the real option file
#define MAX_NB_FRIENDS      9999    //Just a big number; find a better way to limit this...          

const unsigned int nb_friends_in_file = NB_FRIENDS_IN_FILE;
const int max_nb_friends = MAX_NB_FRIENDS;

static const int server_port = 4147;            //NOTE : Put it in socketserver

//TODO : Try not to use predefine channels for connections (except for server)...
#define SERVER_CONNECTION    1
#define TOURN_AGREE_CHANNEL  2
#define ROBOT_CONNECTION     3



void exit_cleanly(int);
void quit();

typedef list<NetConnection*> list_NetConn;
typedef list<NetConnection*>::iterator list_It_NetConn; //Note : use sets instead of lists ?!?


class SocketServer : public SocketHandler
{
public:
  SocketServer();
  ~SocketServer();

  void open_socket( int port_number = 0 ); //Open a socket to listen to connections
  void close_socket();                     //Close the listing socket

  bool accept_new_server();                //Accept a new connection
  int port_num() {return port_number;}     //my port

  void set_packet_factory( PacketFactory* pf)
    { server_packet_factory = pf ; }

protected:
  //Functions needed by SocketHandler
  virtual void handle_stdin( char * );
  virtual int  check_fd( );
  virtual int  set_fd( );

  NetConnection* accept_connection();       //Accept an incomming connection
  void remove_unconnected_sockets();        //Remove all connections that has been closed during this check


  list<int> used_channels;                  //List of all the used channels

  int my_id;
  int next_id; 

  list_NetConn all_connections;


  string host_name;         //My host name
  int port_number;          //The port I use.

  PacketFactory* server_packet_factory;

private:
  int server_socket;        //The server socket
};

#endif //__SOCKETSERVER_H__
