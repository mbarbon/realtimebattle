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


#include <arpa/inet.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <strstream>
#include <fstream.h>

#include "ServerPackets.h"
#include "ServerSocket.h"

#include "EventRT.h"
#include "EventHandler.h"

SocketServer::SocketServer()
{
  packet_factories[ 0 ] = new InitPacketFactory;
  packet_factories[ SERVER_CONNECTION ] = new ServerPacketFactory;
  //  packet_factories[ TOURN_AGREE_CHANNEL ] = &my_tournament_agreement_packetfactory; //NOTE : Do I need it here ?

  my_id = 0;
  next_id = 0;
  leader = NULL;
}

void
SocketServer::init( int argc, char* argv[]  )
{
  //TODO : Find a way to do the init of the server here...
  
  int port_nb = server_port;
    
  extern char* optarg;
  extern int optind;
    
  static struct option long_options[] =
  {
    //option, argument?, flag, value
    {"server_port", 1, 0, 0},
    {"server_file", 1, 0, 0},
    
    {0, 0, 0, 0}
  };
    
  for(;;)
    {
      int option_index = 0;
     
      int c = getopt_long( argc, argv, "-",
			   long_options, &option_index );
    
      // Detect the end of the options.
      if (c == -1)
	break;
    
      switch( c ) 
	{
	case 0 : 
	  {
	    switch( option_index ) {
	    case 0 :  
	      sscanf(optarg, "%d", &port_nb);
	      break;
	    case 1 :
	      set_friends_opt_file( (string)optarg ); //TODO : find this from the option file
	      cout<<"server file "<<optarg<<endl;
	      break;
	    }
	  }
	  break;
	}
    }
  
  open_socket( port_nb );
  cout<<"Server Connected to Port : "<<port_nb<<endl;
  find_first_point();      

  optind = 1;   //For the next function which will parse the options
}

void
SocketServer::open_socket( int port_nb = 0 )
{
  struct sockaddr_in src;

  //Set my port number
  if( port_nb == 0 )
    port_number = server_port;
  else
    port_number = port_nb;

  //Set my host name
  char sn[256];
  gethostname(sn, 256);
  host_name = string(sn);

  //Open my socket

  if( (server_socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
    {
      cout<<( "Failed to open socket." );
      quit();
    }

  int opt=1; 
  if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, 
		(char*)&opt, sizeof(opt))) {
    cout<<"setsockopt failed"<<endl;
  }

  src.sin_family = AF_INET;
  src.sin_port = htons( port_number );
  src.sin_addr.s_addr = htonl( INADDR_ANY );

  if( bind( server_socket, (struct sockaddr*) &src, sizeof( src ) ) < 0 )
    {
      cout<<( "Failed to bind socket." );
      quit();
    }

  if( listen( server_socket, max_number_connections ) < 0 )
    {
      cout<<( "Listen to socket failed." );
      quit();
    }
}

void
SocketServer::set_friends_opt_file( string n )      //Where to find my friends
{
  friends_opt_file = n;

  ifstream ifs(friends_opt_file.c_str());
  while(! ifs.eof() )
    {
      friend_on_ring F;
      ifs>>F.name>>F.port>>F.mark;
      if(F.name != "")
	{
	  friends.push_back( F );
	}
    }
}

void
SocketServer::find_first_point( )
  //Maybe there is no use for file_name but for debuging.
  //TODO : Put the friends addresses in the option file ?
{
  /*
  //Find the first point in the network
  for(unsigned int i = 0; i < friends.size(); i ++)
    {
      friend_on_ring* F;
      NetConnection* nc;
      F = &(friends[i]);
      nc = connect_to_an_other_server(F->name, F->port);
      if( nc )
	{
	  cout<<"connected to "<<F->port<<endl;
	  nc->send_data( InitializationPacket( "Join" ).make_netstring() );
	  connection_factory[ nc ] = &my_serverpacketfactory;

	  if(F->mark < 100) {F->mark ++;}
	  return;
	}
      else
	{
	  if(F->mark > 0) {F->mark --;}
	}
    }
  //If I come to this point, I'm not connected to anybody : I'm the leader
  */
  my_id = next_id = 1;
}
/*
bool 
SocketServer::not_connected_to(string h, int p, int c = SERVER_CONNECTION)
{
  //NOTE : most find something to do in case they use the channel argument
  for(list_It_NetConn li = by_type_connections[c].begin();
      li != by_type_connections[c].end(); li ++)
    {
      //      ServerState s = server_states[*li];
      if(h == s.address && p == s.port) //doesn't work all the time : find a better way to check the address
	return false;
    }
  return true;
}
*/

void
SocketServer::close_socket()
{
  close( server_socket );
}

void
SocketServer::check_socket()
{
  struct timeval tv;
  tv.tv_sec  = 0;
  tv.tv_usec = 0; //500000;

  fd_set readfs;
  fd_set exceptfs;


  FD_ZERO( &readfs );
  FD_ZERO( &exceptfs );

  FD_SET( 0 , &readfs ); 
 
  FD_SET( server_socket, &readfs );
  FD_SET( server_socket, &exceptfs );

  int max_desc = server_socket;

  list_It_NetConn li;
  for( li = all_connections.begin(); li != all_connections.end(); li++ )
    if( (**li).connected )
      {
        FD_SET( (**li).the_socket, &readfs );
        FD_SET( (**li).the_socket, &exceptfs );
        max_desc = max( max_desc, (**li).the_socket );
      }

  if( select( max_desc + 1, &readfs, NULL, &exceptfs, &tv ) < 0 )
    {
      cout<<( "select failed." ) << endl;
      quit();
    }

  
  if( FD_ISSET( 0, &readfs ) )
    {
      char buffer[256];
      bzero(buffer, 256);
      fgets(buffer, 255, stdin);
      
      string buf = string (buffer);
      
      if( buf == "quit\n")   //The quit event (maybe a click for a chat)
	{ 
	  cout<<"Ciao\n";
	  exit( 0 );
	}
    }

  if( FD_ISSET( server_socket, &readfs ) )
    {
      cout << "Got new connection."<<endl;
      accept_connection();
    }

  for( li = all_connections.begin(); li != all_connections.end(); li++ )
    if( (**li).connected && FD_ISSET((**li).the_socket, &exceptfs) )
      {
        cout<<( "Exception for client.\n" );
        (**li).close_socket();
      }

  for( li = all_connections.begin(); li != all_connections.end(); li++ )
    {
      if( (**li).connected && FD_ISSET( (**li).the_socket, &readfs ) )
	{
	  //NOTE : Maybe have the connection as a variable in the class.
	  int read = (**li).read_data();

	  Packet *P;
	  while( ! ((**li).read_buffers).empty() )
	    {
	      //Extract the string for the queue and make a packet with it
	      string data = (**li).read_buffers.front();

	      P = connection_factory[*li]->MakePacket( data );

	      (**li).read_buffers.pop_front();

	      if( !P ) continue; //Jump to the next Packet

	      P->get_string_from_netstring( data );

	      P->handle_packet( *li );

	      //Delete this old packet...
	      delete P;
	    }

	  if( read < 0 )
	    {
	      (**li).close_socket();
	    }
	}
    }
  remove_unconnected_sockets();
}

bool
SocketServer::channel_is_used(int c)
{
  for(list<int>::iterator it = used_channels.begin();
      it != used_channels.end(); it ++)
    if((*it) == c)
      return true;
  return false;
}

int
SocketServer::open_channel(PacketFactory* PF) {
  int channel = 2;
  while( 1 )
    {
      if(packet_factory( channel ) == NULL)
	{
	  packet_factories[ channel ] = PF;
	  cout<<PF->Protocol()<<" opened on channel "<<channel<<endl;
	  return channel;
	}
      channel ++;
    }
  return 0;
}

NetConnection*
SocketServer::connect_to_an_other_server( string hostname, int port_nb = 0 )
{
  //if( port_nb == 0 )
  //  port_nb = meta_server_port;

  struct sockaddr_in src;
  struct hostent* hostinfo;
  long address;
  int the_socket;

  if( hostname.empty() )
    hostname = "localhost";

  if( isdigit( hostname[0] ) )
    {
      if( (address = inet_addr( hostname.c_str() )) == (long)INADDR_NONE )
        {
          cout<< "Invalid hostname\n" ;
	  return NULL;
        }
      src.sin_addr.s_addr = address;
      src.sin_family = AF_INET;
    }
  else if( (hostinfo = gethostbyname( hostname.c_str() )) == NULL )
    {
      cout<<"Failed looking up host\n" ;
      return NULL;
    }
  else
    {
      src.sin_family = hostinfo->h_addrtype;
      src.sin_addr = *((struct in_addr*) hostinfo->h_addr);
    }
  
  src.sin_port = htons( port_nb );

  if( (the_socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
    {
      cout<<"Socket failed\n";
      return NULL;
    }

  if( connect( the_socket, (struct sockaddr*) &src, sizeof(src) ) < 0 )
    {
      cout<<"Connect failed\n";
      return NULL;
    }
  
  NetConnection* Negociator = new NetConnection;

  all_connections.push_back(Negociator);

  Negociator->the_socket = the_socket;
  Negociator->connected = true;
  Negociator->make_nonblocking();

  /*
  ServerState s(hostname, port_nb, max_nb_friends, max_nb_friends);
  server_states[Negociator] = s;   
  */

  return Negociator;
}

NetConnection*
SocketServer::accept_connection()
{
  int new_socket;
  struct sockaddr_in fromend;

  unsigned int fromlen = sizeof(fromend);

  cout<<"Accepting a new connection\n";

  if( (new_socket = accept( server_socket,
                            (struct sockaddr*) &fromend, &fromlen )) < 0 )
    {
      cout<<( "Accepting connection failed" );
      close( server_socket );
      quit();
    }

  struct NetConnection* nc = new NetConnection;
  nc->id = next_id;
  next_id++;

  nc->the_socket = new_socket;
  nc->make_nonblocking();
  nc->connected = true;

  all_connections.push_back( nc );
  //cout<<"Now we are "<<all_connections.size() + 1<<endl;
  //  by_type_connections[0].push_back( nc );
  connection_factory[ nc ] = packet_factory( 0 );
  packet_factory( 0 )-> add_connection( nc );

  return nc;
}

vector<string>
SocketServer::give_other_addresses(NetConnection* nc, string protocol, int nb_addresses = 0)  //USELESS : must be done by the ServerPacketFactory ?
{
  vector<string> v;
  map<int, PacketFactory*>::iterator it;

  for(it = packet_factories.begin(); it != packet_factories.end(); it++)
    {
      cout<<it->second->Protocol()<<endl;
      if(protocol == it->second->Protocol())
	{ break; }
    }


  if( it != packet_factories.end() )
    {
      int channel = it->first;

      /*Try this :
       * if( channel == SERVER_CONNECTION )
       *   his_channel = SERVER_CONNECTION;
       * else
       *   his_channel = 0;
       *
       * for(it = by_type_conn(channel)-> begin..end )
       *   os << s.address << " "<< s.port <<" "<<his_channel;
       *   ...
       * if( channel_is_used(channel)
       *   send my info
       */
 	{ 
	  ostrstream os;
	  os<<host_name<<" "
	    <<port_number<<" "
	    <<channel
	    <<ends;
	  cout<<os<<endl;
	  v.push_back(os.str());
	}
    }
  else
    {
      cout<<"No such protocol : "<<protocol<<endl;
    }
  return v;
}

struct ptr_is_not_connected  //Use in the next function
{
  bool operator()(NetConnection* const p) {return p->is_not_connected();}
};

void
SocketServer::remove_unconnected_sockets()
{
  list_It_NetConn li;
  while( ( li = find_if( all_connections.begin(), all_connections.end(),
                         ptr_is_not_connected() ) )
         != all_connections.end() )
    {
      connection_factory[ *li ] -> remove_connection( *li );
      delete (*li);
      all_connections.erase( li );
    }
}

void
SocketServer::send_packet_by_name(string name, Packet* P) //Change ServerMessagePacket to Packet when 'ready'
{
  /* This is to send a message to specific named chatters */

  //cout<<"Send ["<<P->message<<"] to "<<name<<endl;
  
  string data = (*P).make_netstring();
  
  list_It_NetConn li;
  for( li = all_connections.begin(); li != all_connections.end(); li++ )
    {
      if( (**li).get_type() == SERVER_CONNECTION )
	{
	  cout<<".";
	  (**li).send_data( data ); 
	}
    }
  cout<<endl;
}

void
SocketServer::send_packet_by_type(int type, Packet* P)
{
  /* TODO : broadcast to a packet factory
  string data = (*P).make_netstring();

  list_It_NetConn li;
  for( li = by_type_conn(type)->begin(); li != by_type_conn(type)->end(); li++ )
    (**li).send_data( data ); 
  */
}

SocketServer::~SocketServer()
{
  list_It_NetConn li;

  //Say you want to quit to everybody
  CommandPacket P("Quit");
  string data = P.make_netstring();
  
  for( li = all_connections.begin(); li != all_connections.end(); li++ )
    (**li).send_data( data ); 

  sleep(1);

  //Close all the connections
  for(li = all_connections.begin(); li != all_connections.end();
      li ++)
    {
      (**li).close_socket();
      delete (*li);
    }
  all_connections.clear();

  //Delete all the packet factories
  for(map<int, PacketFactory*>::iterator mi = packet_factories.begin();
      mi != packet_factories.end(); mi ++)
    delete mi->second;

  //Save my friends (NOTE : Don't know if we need this anymore....)
  ofstream ofs(friends_opt_file.c_str());
  for(unsigned int i = 0; i < friends.size(); i ++)
    {//TODO : choose those to save...
      ofs<<friends[i].name<<"  "<<friends[i].port<<"  "<<friends[i].mark<<endl;
    }
}

void
exit_cleanly(int Sign)
{
  quit();
}

void
quit()
{
  exit( 0 );
}

