/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony

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


/*
  The application to communicate from server to robot !
  NOTE : this may also be used to communicate from GUI client if clients use pipes as input !
*/

#include "RobotClient.h"
#include "Process.h"
#include "ClientSocket.h"
#include "RobotPacketFactory.h"
#include <iostream>
#include <unistd.h>
int
main(int argc, char* argv[])
{
  /* argv[0] : robotclient

     argv[1] : hostname
     argv[2] : port_number
     argv[3] : Protocol version
     argv[4] : Channel number

     argv[5] : robot path
     argv[6] : robot exec

     argv[7] : uniqueness id number
     argv[8] : random id number
  */

  cout<<"Launching robot\n";
  /*
    for(int i = 0; i < argc; i ++)
    {
      cout<<i<<"  "<<argv[i]<<endl;
    }
  */
  
  SocketClient my_client;
  NetConnection* my_conn = my_client.connect_to_server( argv[1], 4147 );
  my_client.set_packet_factory( new RobotPacketFactory );

  string id = (string)argv[6] + "_" + (string)argv[7] + "/" + (string)argv[8];

  my_conn->send_data( "@IConnect "+ (string) argv[3] + " " + (string) argv[4] + " " + id ); 

  cout<<"socketclient ok\n";

  string robot_name = string(argv[5]) + string(argv[6]);
  cout<<"Robot file : "<<robot_name<<endl;

  Process P( robot_name. c_str() );
  P.start( NORMAL_MODE );

  cout<<"process started\n";

  //(*P.get_outstreamp())<<"Initialize 1"<<endl;

  //P.set_signal_to_send(true, 10);
  //P.send_signal();


  while( 1 )
    {
      my_client.check_socket();

      P.reset_messages();
      while( P.more_messages()  )
	{
	  string message = P.get_next_message();
	  my_conn->send_data("OM" + message );
	}
    }
  
}
