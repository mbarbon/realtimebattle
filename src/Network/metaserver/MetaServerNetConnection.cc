#include "MetaServerNetConnection.h"
#include "Packets.h"

void
MetaServerNetConnection::close_socket()
{
  /*
    CommandPacket CP("Close");
    send_data( CP.make_netstring() );
  */
  NetConnection::close_socket();
}

