#include "ServerNetConnection.h"

extern Root_Connection* root_client;

void
ServerNetConnection::close_socket()
{
  NetConnection::close_socket();
}

