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
  bool is_server;
};

#endif
