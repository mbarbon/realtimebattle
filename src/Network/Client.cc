/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2000  Erik Ouchterlony and Ragnar Ouchterlony

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


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif


#include <unistd.h>
#include <fstream.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include "Socklib.h"


int 
main ( int argc, char* argv[] )
{
  const int port = 19074;
  const int dbug = 1;

  char buffer[80];


  int socket_fd = connectto("sibelius", port, dbug);

  ifstream in_socket (socket_fd);
  ofstream out_socket(socket_fd);

  out_socket << argv[1] << endl; 


  do
    {
      out_socket << argv[1] << " still active" << endl; 

//        in_socket >> ws;
//        in_socket.clear();
//        in_socket.get(buffer, 80, '\n');
//        cout << buffer << endl;

      sleep(1);

    }  while( true );

}

