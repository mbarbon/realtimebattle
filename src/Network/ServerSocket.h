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



#ifndef __SOCKETSERVER_H__
#define __SOCKETSERVER_H__

#include <list>
#include <string>
#include <map>

#include "ServerPackets.h"
#include "NetConnection.h"

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

class ServerState {
 public:
  ServerState() : 
    address("localhost"), port(server_port), 
    nb_conn(0), av_conn_friends(0) 
    {}
  ServerState(string a, int p) : 
    address(a), port(p),
    nb_conn(0), av_conn_friends(0)
    {}
  ServerState(string a, int p, int nc, int acf) : 
    address(a), port(p),
    nb_conn(nc), av_conn_friends(acf)
    {}

  void set_nb_conn(int nc, int acf) {nb_conn = nc; av_conn_friends = acf;}

  friend ostream& operator<<(ostream&, const ServerState&);  

  string address; int port;
  int nb_conn, av_conn_friends; //Nb of connections and nb of connections of my friends
  //av_conn_friends doesn't need to be true all the time : just to have ca. info
};


class SocketServer
{
public:
  SocketServer();
  ~SocketServer();

  void init(int argc, char* argv[]);
  
  void open_socket( int port_number = 0 ); //Open a socket to listen to connections
  void set_friends_opt_file(string n);      //Where to find my friends
  void find_first_point( );                //Find some servers on the net
  void close_socket();                     //Close the listing socket

  void check_socket();                     //Check what is in every socket
  
  NetConnection* connect_to_an_other_server( string, int port_number = 0 );

  bool accept_new_server();                //Accept a new connection
  int port_num() {return port_number;}     //my port

  //give addresses on which I am connected, for other servers to open a ring.
  vector<string> give_other_addresses(NetConnection*, string, int = 0); 
  list<NetConnection*> give_other_servers(NetConnection*, int = 0);
  bool not_connected_to(string, int, int=SERVER_CONNECTION);

  int open_channel(PacketFactory*);  //ask to open a channel using a channel number
  void close_channel(int);
  string channel_protocol(int channel) { return packet_factory(channel)->Protocol(); }
  void uninitialize_connections( list<NetConnection*> & list_conn );


  bool channel_is_used(int);          //Is a channel used or not
  //I don't really know what will be the use of the channels.

  void send_packet_by_name(string, Packet* );   //NOTE : useless ?
  void send_packet_by_type(int, Packet* );  //Send a packet to all elements of a channel (is it usefull ? Maybe for debug)

  friend class InitializationPacket;
  friend class CommandPacket;
  friend class SubmitListPacket;

private:
  NetConnection* accept_connection();       //Accept an incomming connection
  void remove_unconnected_sockets();        //Remove all connections that has been closed during this check


  list<int> used_channels;                  //List of all the used channels

  NetConnection* leader;                //NOTE : useless ?
  int my_id;
  int next_id; 


  int robot_nb;  //Number of robots and chats
  int chat_nb;   //NOTE : useless here

  list_NetConn all_connections;

  map<int, PacketFactory*> packet_factories;

  PacketFactory* packet_factory(int channel) 
    {
      return packet_factories[ channel ];
    }

  map<NetConnection*, PacketFactory*> connection_factory;

  string host_name;         //My host name
  int port_number;          //The port I use.
  int server_socket;        //The server socket


  //How to find your friends on the ring (get into the ring when start
  struct friend_on_ring {
    string name;
    int port;
    int mark;
  };  //Maybe put it in the ServerInfo ... (only mark is different...)

  string friends_opt_file;            //Where to find my friends
  vector<friend_on_ring> friends;  
};

extern SocketServer my_socketserver;

#endif //__SOCKETSERVER_H__
