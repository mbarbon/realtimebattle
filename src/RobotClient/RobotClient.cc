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
#include "SimpleProcess.h"
#include "ClientSocket.h"
#include "RobotPacketFactory.h"
#include <iostream>

int
main(int argc, char* argv[])
{
  cout<<"Launching robot\n";
  for(int i = 0; i < argc; i ++)
    {
      cout<<i<<"  "<<argv[i]<<endl;
    }

  
  SocketClient my_client;
  NetConnection* my_conn = my_client.connect_to_server( argv[1], 4147 );
  my_client.set_packet_factory( new RobotPacketFactory );

  string id = (string)argv[6] + "_" + (string)argv[7] + "/" + (string)argv[8];

  my_conn->send_data( "@IConnect "+ (string) argv[3] + " " + (string) argv[4] + " " + id ); 

  cout<<"socketclient ok\n";

  SimpleProcess my_process( (string(argv[5]) + string(argv[6])). c_str() );
  my_process.start();

  cout<<"process started\n";

  while( 1 )
    {
      my_client.check_socket();
      //cout<<(my_process.ipipe_streamp)->c_str()<<endl;
    }
  
}
