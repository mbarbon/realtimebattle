#include "ServerNetConnection.h"
#include <iostream.h>
#include <string>


Robot_Connection::Robot_Connection(ServerNetConnection& C) : ServerNetConnection(C)
{
  state = INITIALIZED;
}

Robot_Connection::~Robot_Connection()
{
  close_socket();
}

