#ifndef __SOCKETSERVER_H__
#define __SOCKETSERVER_H__

#include <list>
#include <string>

#include "Packets.h"
#include "NetConnection.h"

static const int server_port = 4147;
static const int metaserver_port = 4146;
static const int max_number_connections = 10;

void exit_cleanly(int);
void quit();

class SocketServer
{
public:
  SocketServer() 
    : next_id(0), root_client(0), MetaServer(0), 
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

  int robot_nb;  //Number of robots and chats
  int chat_nb;
  list<ServerNetConnection*> connected_clients;
  string name;
  string language;
  string version;
  int port_number;
  int next_id;

  ServerNetConnection* root_client;
  NetConnection* MetaServer;
  int server_socket;
};

#endif __SOCKETSERVER_H__
