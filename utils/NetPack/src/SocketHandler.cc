/*
NetPack for RealTimeBattle
Copyright (C) 2002  Benoit Rousseau

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

#include "SocketHandler.h"

#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

SocketHandler::SocketHandler() {};

SocketHandler::~SocketHandler() {};

void
SocketHandler::add_read( int i )
{
  FD_SET( i , &readfs );
  max_desc = (max_desc > i) ? max_desc : i ;
}

void
SocketHandler::add_except( int i )
{
  FD_SET( i, &exceptfs );
  max_desc = (max_desc > i) ? max_desc : i ;
}

void
SocketHandler::add_fd( int i )
{
  add_except( i );
  add_read  ( i );
}

bool
SocketHandler::is_fd_read( int i )
{
  return FD_ISSET( i , &readfs );
}

bool
SocketHandler::is_fd_except( int i )
{
  return FD_ISSET( i, &exceptfs );
}


void
SocketHandler::check_socket() {
	struct timeval tv;
	tv.tv_sec  = 0;
	tv.tv_usec = 0; //500000;

	FD_ZERO( &readfs );
  FD_ZERO( &exceptfs );

  add_read( 0 );

  set_fd();

  if( select( max_desc + 1, &readfs, NULL, &exceptfs, &tv ) < 0 )
  {
    cout<< "select failed." << endl;
    exit( 0 ); //throw an exception instead
  }

  if( is_fd_read( 0 ) )
  {
    char buffer[256];
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);
    handle_stdin( buffer );
  }

  check_fd();
}

