#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <list>
#include <string>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <stack>
#include <typeinfo>
#include <set>
#include <algorithm>


#include "NetConnection.h"
#include "String.h"

void
NetConnection::close_socket()
{
  if( connected )
    {
      close( the_socket );
      connected = false;
      the_socket = -1;
    }
}

int
NetConnection::read_data()
{
  int bytes_read = 0;
  char buffer[max_packet_length];

  bytes_read = read( the_socket, buffer, max_packet_length );

  if( bytes_read > 0 )
    {
      string str_buf( buffer, bytes_read );
      read_buffer += str_buf;
      return bytes_read;
    }
  else if( bytes_read == 0 )
    {
      cout << "EOF on socket!" << endl;
      return -1;
    }
  else if( errno == EWOULDBLOCK || errno == EAGAIN )
    return 0;

  return -1;
}

int
NetConnection::write_data()
{
  while( !write_buffer.empty() )
    {
      fd_set writefs;
      fd_set exceptfs;

      FD_ZERO( &writefs );
      FD_ZERO( &exceptfs );

      FD_SET( the_socket, &writefs );
      FD_SET( the_socket, &exceptfs );

      struct timeval tv;
      tv.tv_sec = 0;
      tv.tv_usec = 0;

      if( select( the_socket+1, NULL, &writefs, &exceptfs, &tv ) <= 0 )
        break;

      if( FD_ISSET( the_socket, &exceptfs ) )
        {
          close( the_socket );
          connected = false;
          the_socket = -1;
          return -1;
        }

      if( FD_ISSET( the_socket, &writefs ) )
        {
          string newstr = write_buffer.substr( 0, max_packet_length );
          int nput = 0;

          if( (nput = write( the_socket, newstr.data(), newstr.length() )) == -1 )
            {
              if( errno == EWOULDBLOCK || errno == EAGAIN )
                break;
              close_socket(); // Should I really do this here?
              return -1;
            }

          if( (int)write_buffer.length() < nput )
            write_buffer = write_buffer.substr( nput, string::npos );
          else
            write_buffer.erase(0, string::npos);
        }
    }
  return 0; // For now
}

void
NetConnection::send_data( const string& data )
{
  if( connected )
    {
      write_buffer += data;
      write_data();
    }
}

void
NetConnection::make_nonblocking()
{
  int f_set;

  if( (f_set = fcntl( the_socket, F_GETFL )) == -1 )
    cerr << "fcntl F_GETFL failed" << endl;

  f_set |= O_NONBLOCK;

  if( fcntl( the_socket, F_SETFL, f_set ) == -1 )
    cerr << "fcntl F_SETFL failed" << endl;
}

