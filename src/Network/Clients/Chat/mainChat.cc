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


#include <vector>
#include <string>
#include <iostream>

#include <signal.h>

#include "ClientSocket.h"

#define RTB_NAME_VERSION "RTB v2"
#define WEBSITE_URL      "http://"

int
main( int argc, char* argv[] )
{
  string host_name = server_host;
  int PortNum;
  bool launch_server;

  /* Parse the arguments */
  vector<string> args(argv, argv + argc);
  for(vector<string>::iterator iter = args.begin();
      iter != args.end();
      iter++)
    {
      cout<<*iter<<endl;
      if( *iter == string("--help") ) {
	cout<<"This is the help : \n";
	cout<<"\t--launch_server\t\tlaunch the realtimebattle server\n";
	cout<<"\t--server_name [name]\t\twhere to connect (always localhost when you launch it\n";
	cout<<"\t--server_port [port]\t\twhere to connect ("<<server_port<<" by default)\n";
	exit( 0 );
      }
      else if( *iter == string("--metaserver") ) {
	//Connect immediatly to the metaserver and quit ???

      }
      else if( *iter == string("--launch_server") ) {
	launch_server = true;
	cout<<"I will be root \n";
      }
      else if( *iter == string("--server_name") ) {
	host_name = *(iter + 1);
      }
      else if( *iter == string("--server_port") ) {
      }
    }








  my_socketclient->my_cons.write(C_VERSION, "This is the client for %s", RTB_NAME_VERSION);
  my_socketclient->my_cons.write(C_COMMENT, "You can learn a lot about RealTimeBattle at %s",
				     WEBSITE_URL);
  

  while(1)
    {
      my_socketclient->my_cons.prompt_off();
      my_socketclient->my_cons.write(C_OK, "ServerName : ");
      cin>>host_name;
      if( host_name == "metaserver") 
	{
	  my_socketclient->connect_to_server( meta_server_host , meta_server_port );
	  my_socketclient->ask_for_info_to_metaserver() ;
	}
      else
	{
	  cout<<"Port Number : ";
	  cin>>PortNum;
	  my_socketclient->connect_to_server( host_name, PortNum );
	  string name;
	  cout<<"Name : ";
	  cin>>name;
	  my_socketclient->initialize( name, CHAT_CLIENT_CONNECTION );

	  my_socketclient->my_cons.prompt_on();
	  my_socketclient->my_cons.update_prompt();
	}

      while( my_socketclient->is_connected() )
	my_socketclient->check_connection();

      my_socketclient->close_connection();
    }
  return 0;
}
