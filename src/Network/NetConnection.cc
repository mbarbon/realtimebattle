/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2001  Erik Ouchterlony and Ragnar Ouchterlony

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <list>
#include <string>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>

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
  ssize_t bytes_read = 0;
  char buffer[max_packet_length]; 
  static string str_buf; //The buffer we are reading currently
  int max_size_at_once = max_nb_packet_at_one_time*max_packet_length; //It is not a strict limit

  char size_c[2]; //Size of the packet in char... (netlong) 

  errno = 0;      //Do I really need to do this ?

  while( 1 )
    {
      if( size_buf == 0 )
	{
	  //Read the size of the packet
	  bytes_read = read( the_socket, size_c, 2);
	  
	  if( bytes_read == 2 ) //Succesfully read the size
	    ;
	  else if( errno == EWOULDBLOCK || errno == EAGAIN ) //Come back latter
	    return 0;
	  else //Anything else should be considered as a problem ?
	    return -1;

	  //Now make the size become an int  
	  memcpy(&size_buf, size_c, 2);
	  size_buf =  ntohs(size_buf);
	}

      //Then try to read the packet
      bytes_read = read( the_socket, buffer, 
			 ((size_buf<max_packet_length)?(size_buf):(max_packet_length))
			 );

      if( bytes_read > 0 ) //More informations on this packet
	{
	  str_buf += string( buffer, bytes_read );
	  size_buf -= bytes_read; //size_c chars to read until the end of the packet
	  max_size_at_once -= bytes_read;
	  if(max_size_at_once < 0)
	    return 0;
	}
      else if( errno == EWOULDBLOCK || errno == EAGAIN )
       	return 0; 
      else if( bytes_read == 0 ) //This was a big problem ?!?
	return -1;

      //Maybe we can put this inst bloc at the begining...
      if( size_buf == 0 ) //We have read the all packet now
	{
	  read_buffers.push_back(str_buf);
	  str_buf = string("");
	}
    }
}

string
NetConnection::get_data()
{
  string to_return = read_buffers.front();
  read_buffers.pop_front();
  return to_return;
}

int
NetConnection::write_data()
{
  while( !write_buffers.empty() )
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
          int nput = 0;

	  //Get the size of the current packet and send it in the socket
	  char buf[2];
	  unsigned short x = write_buffers.front().length();

	  x = htons(x);

	  memcpy( buf, &x, 2 );
	  if( write( the_socket, buf, 2) == -1 )
	    {
              if( errno == EWOULDBLOCK || errno == EAGAIN )
                break;
              close_socket(); // Should I really do this here?
              return -1;
	    }

	  //Now send the all packet itself
	  string write_buffer = write_buffers.front();
	  while( !write_buffer.empty() ) //Can it block???
	    {
	      //The information to send
	      string newstr = write_buffer.substr( 0, max_packet_length );
	      if( (nput = write( the_socket, newstr.data(), newstr.length() )) == -1 )
		{
		  if( errno == EWOULDBLOCK || errno == EAGAIN )
		    break;
		  close_socket(); // Should I really do this here?
		  return -1;
		}

	      //The information we still have here (it isn't in the socket :-( )
	      if( (int)write_buffer.length() < nput )
		write_buffer = write_buffer.substr( nput, string::npos );
	      else
		write_buffer.erase(0, string::npos);
	    }
	  //We don't need it here anymore...
	  write_buffers.pop_front();
        }
    }
  return 0; // For now
}

void
NetConnection::send_data( const string& data )
{
  if( connected )
    {
      write_buffers.push_back( data );
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

