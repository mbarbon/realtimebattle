#ifndef __NETCONNECTION_H__
#define __NETCONNECTION_H__

#include <vector>
#include <queue>
#include <string>

static const int max_packet_length = 512;

class SocketServer;
class SocketClient;
class MetaServerSocket;



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

class CommandPacket;

class ServerNetConnection;


class NetConnection
{
public:
  NetConnection         ()
    : id(0), the_socket(-1), address(""), connected(false), state(NOT_INITIALIZED) {}
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

  friend class CommandPacket;
  friend class ChatMessagePacket;    //Only use name
  friend class InitializationPacket; //Only use name
protected:
  enum connection_state { NOT_INITIALIZED, INITIALIZED, IN_GAME };

  int id;
  int the_socket;
  string address;

  //string read_buffer;
  queue<string> read_buffers; //All the read but not parsed packets
  //string write_buffer;
  queue<string> write_buffers;  //All the buffers to send next time
  bool connected;
  connection_state state;

  string name;
};

#endif // __NETCONNECTION_H__
