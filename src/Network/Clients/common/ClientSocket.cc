#include <stdio.h>
#include <arpa/inet.h>
#include <iostream>
#include <cctype>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ClientSocket.h"

void
SocketClient::connect_to_server( string hostname, int port )
{
  struct sockaddr_in src;
  struct hostent* hostinfo;
  long address;
  int the_socket;

  if( port == 0 )
    port = server_port;

  if( hostname.empty() )
    hostname = "localhost";

  if( isdigit( hostname[0] ) )
    {
      if( (address = inet_addr( hostname.c_str() )) == (long)INADDR_NONE )
        {
          my_cons.write(C_FAIL,  "Invalid hostname" );
          return;
        }
      src.sin_addr.s_addr = address;
      src.sin_family = AF_INET;
    }
  else if( (hostinfo = gethostbyname( hostname.c_str() )) == NULL )
    {
      my_cons.write(C_FAIL,  "Failed looking up host" );
      return;
    }
  else
    {
      src.sin_family = hostinfo->h_addrtype;
      src.sin_addr = *((struct in_addr*) hostinfo->h_addr);
    }

  src.sin_port = htons( port );

  if( (the_socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
    {
      my_cons.write(C_FAIL,  "Socket failed" );
      return;
    }

  if( connect( the_socket, (struct sockaddr*) &src, sizeof(src) ) < 0 )
    {
      my_cons.write(C_FAIL,  "Connect failed" );
      return;
    }


  nc.the_socket = the_socket;
  nc.make_nonblocking();

  nc.connected = true;
  nc.address = hostname;
}


