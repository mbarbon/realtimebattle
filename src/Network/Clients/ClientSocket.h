#ifndef __SOCKETCLIENT_H__
#define __SOCKETCLIENT_H__

#include <string>

#include "NetConnection.h"
#include "Console.h"

static const int default_server_port = 4147;
static const int meta_server_port = 4146;
static const string meta_server_host = "localhost";


class SocketClient
{
public:
  void connect_to_server( string hostname, int port = 0 );
  
  void check_connection();
  //  void write_to_server( const string& str );
  void initialize( const string&, const client_t& ); // RTBv2 initialization
  void close_connection();
  void ask_for_info_to_metaserver();

  bool is_connected();
  Console my_cons;

private:
  NetConnection nc;
};

extern SocketClient *my_socketclient;

void exit_cleanly(int Sign);
void quit();



#endif __SOCKETCLIENT_H__
