#include "ClientSocket.h"
#include <signal.h>

#define RTB_NAME_VERSION "RTB v2"
#define WEBSITE_URL      "http://"

int
main( int argc, char* argv[] )
{
  string host_name;
  int PortNum;

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
	}

      my_socketclient->my_cons.prompt_on();
      my_socketclient->my_cons.update_prompt();

      while( my_socketclient->is_connected() )
	my_socketclient->check_connection();

      my_socketclient->close_connection();
    }
  return 0;
}
