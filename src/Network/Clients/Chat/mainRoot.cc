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

/*
 * TODO : try to merge this src with the mainClient
 * TODO : so that it would be only one exec (much more simple for user)
 */


#include <signal.h>

#include "ClientSocket.h"
#include "Console.h"

#define RTB_NAME_VERSION "RTB v2"
#define WEBSITE_URL      "http://"

int
main( int argc, char* argv[] )
{
  signal(SIGALRM, exit_cleanly);
  signal(SIGTERM, exit_cleanly);
  signal(SIGINT,  exit_cleanly);
  
  my_socketclient = new SocketClient;

  my_socketclient->my_cons.write(C_VERSION, "This is the client for %s", RTB_NAME_VERSION);
  my_socketclient->my_cons.write(C_COMMENT, "You can learn a lot about RealTimeBattle at %s",
				     WEBSITE_URL);
  
  int no_port;
  cout<<"please enter port number : ";
  cin>>no_port;

  my_socketclient->connect_to_server( "", no_port );
  my_socketclient->initialize( "root", ROOT_CLIENT_CONNECTION );

  my_socketclient->my_cons.prompt_on();
  my_socketclient->my_cons.update_prompt();

  while( my_socketclient->is_connected() )
    my_socketclient->check_connection();
  
  my_socketclient->close_connection();
  quit();
  return 0;
}
