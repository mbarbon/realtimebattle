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

#endif
