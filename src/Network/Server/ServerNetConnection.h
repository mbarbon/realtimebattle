#ifndef __SERVER_NET_CONNECTION__
#define __SERVER_NET_CONNECTION__

#include "NetConnection.h"

class CommandPacket;

class ServerNetConnection : public NetConnection {
 public:
  void close_socket();
 virtual client_t type_conn() { return UNINITIALIZED; } 
};

class Root_Connection : public ServerNetConnection {
 public:
  Root_Connection(int);
  Root_Connection(ServerNetConnection&);
  ~Root_Connection();
  client_t type_conn() { return ROOT_CLIENT_CONNECTION; }
 protected:
  static bool Allready_a_root;
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
  Chat_Connection(ServerNetConnection&, bool Is_Root = false);
  ~Chat_Connection();
  friend class CommandPacket;
  client_t type_conn() { return CHAT_CLIENT_CONNECTION; }
 protected:
  bool is_root;
};

#endif
