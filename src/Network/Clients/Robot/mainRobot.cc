#include "ClientSocket.h"
#include <signal.h>


int
main( int argc, char* argv[] )
{
  string host_name;
  int PortNum;

  while(1)
    {
      cout<<"ServerName : ";
      cin>>host_name;
      cout<<"Port Number : ";
      cin>>PortNum;
      my_socketclient->connect_to_server( host_name, PortNum );
      string name;
      cout<<"Name : ";
      cin>>name;
      my_socketclient->initialize( name, ROBOT_CLIENT_CONNECTION );
      while( my_socketclient->is_connected() )
	my_socketclient->check_connection();

      my_socketclient->close_connection();
    }
  return 0;
}
