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


#include <stdio.h>
#include <arpa/inet.h>
#include <iostream>
#include <cctype>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "RobotPackets.h"
#include "RobotSocket.h"

void 
SocketRobot::initialize( const string& auth_key )
{
  SocketClient::initialize( auth_key, ROBOT_CLIENT_CONNECTION ); //Say to the server who I am
  P.start( NORMAL_MODE );   //Start the robot (maybe we should wait for a reply from server first...
}

void
SocketRobot::check_connection()
{
  struct timeval tv;
  tv.tv_sec  = 0;
  tv.tv_usec = 500000;

  fd_set readfs;
  fd_set exceptfs;

  FD_ZERO( &readfs );
  FD_ZERO( &exceptfs );
  
  FD_SET( nc.the_socket, &readfs );
  FD_SET( nc.the_socket, &exceptfs );
  
  while( ! P.get_instreamp()->eof() )
    {
      /*
       * TODO : I can't remember the name of the defined var for max_buf_length... sorry
       */
      
      char temp[512] ;
      P.get_instreamp()->getline( temp, 512, '\n' );
      
      string tmp_str(temp);

      nc.send_data( RobotMessagePacket( tmp_str ).make_netstring() );
    }

  if( select( nc.the_socket + 1, &readfs, NULL, &exceptfs, &tv ) < 0 )
    {
      cerr<<"Select failed in SocketRobot::check_connection\n";
      exit(1);
    }

  if( FD_ISSET( nc.the_socket, &exceptfs ) )
    {
      my_cons.write(C_OK, "Exception for client." );
      nc.close_socket();
    }
  
  if( (nc).connected && FD_ISSET( (nc).the_socket, &readfs ) )
    {
      int read = nc.read_data();

      Packet *P;
      while( ! (nc.read_buffers).empty() )
	{
	  //Extract the string for the queue and make a packet with it
	  string data = nc.read_buffers.front();
	  
	  P = make_packet( data );
	  nc.read_buffers.pop_front();
	  
	  if( !P ) continue; //Jump to the next Packet
	  
	  P->get_string_from_netstring( data );
	  
	  switch(P->packet_type()) 
	    {
	    case PACKET_COMMAND :
	    case PACKET_SERVER_COMMAND :
	    case PACKET_SERVER_MESSAGE :
	      P->handle_packet( &nc );
	      break;
	    default : break;
	    }
	  delete P;
	}
    
      if( read < 0 )
	{
	  (nc).close_socket();
	}
    }
}

void
quit()
{
  delete my_socketclient;
  exit( 0 );
}
