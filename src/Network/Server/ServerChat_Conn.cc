#include "ServerNetConnection.h"

Chat_Connection::Chat_Connection(ServerNetConnection& C, bool Is_Root = false)
  : ServerNetConnection(C), is_root(Is_Root)
{
  state = INITIALIZED;
}

Chat_Connection::~Chat_Connection()
{
  close_socket();
}

Chat_Connection::Chat_Connection(int sockfd)
{
  state = NOT_INITIALIZED;
}


