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
