#include "ServerNetConnection.h"
#include "String.h"
#include "Packets.h"

extern Root_Connection* root_client;

void
ServerNetConnection::close_socket()
{
  NetConnection::close_socket();
}

