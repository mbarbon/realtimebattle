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


#include "RobotSocket.h"
#include <signal.h>
#include <stdio.h>


#define WEBSITE_URL      "http://"

SocketRobot* my_socketclient;

int
main( int argc, char* argv[] )
{
  /*
    argv[0] : rtbrobot
    argv[1] : robot path
    argv[2] : server address
    argv[3] : server port
    // argv[4] : authentification key
  */

 
  if( argc != 4 )
    {
      cerr<<"rtbrobot : This program is used by RealTimeBattle to establish a comunication\n"
	  <<"           between a robot and the RealTimeBattle server.\n"
	  <<"           You should use this program by yourself.\n\n";
      cout<<"You can learn more about RealTimeBattle at "<<WEBSITE_URL<<endl;
      exit( 0 );
    }

  string host_name = argv[2];

  int PortNum;
  sscanf(argv[3], "%d", &PortNum);

  my_socketclient = new SocketRobot(argv[1]);
  
  my_socketclient->connect_to_server( host_name, PortNum );
  my_socketclient->initialize( "Robot" /*TODO : Put the authentification key instead */ );
 
  while( my_socketclient->is_connected() )
    my_socketclient->check_connection();
  
  my_socketclient->close_connection();

  exit( 0 );
}
