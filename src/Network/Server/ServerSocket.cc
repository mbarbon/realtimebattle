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


#include "ServerPackets.h"
#include "ServerSocket.h"
#include "ServerNetConnection.h"

Chat_Connection* root_client = NULL; //Don't really know if it is good to let it in SocketServer...
SocketServer* my_socketserver;

void
SocketServer::open_socket( int port_nb = 0 )
{
  struct sockaddr_in src;

  if( port_nb == 0 )
    port_number = server_port;
  else
    port_number = port_nb;

  if( (server_socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
    {
      cerr << "Failed to open socket." << endl;
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
      cerr << "Failed to bind socket." << endl;
      quit();
    }

  if( listen( server_socket, max_number_connections ) < 0 )
    {
      cerr << "Listen to socket failed." << endl;
      quit();
    }
}

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
  tv.tv_usec = 500000;

  fd_set readfs;
  fd_set exceptfs;

  FD_ZERO( &readfs );
  FD_ZERO( &exceptfs );

  FD_SET( server_socket, &readfs );
  FD_SET( server_socket, &exceptfs );

  int max_desc = server_socket;

  list<ServerNetConnection*>::iterator li;
  for( li = connected_clients.begin(); li != connected_clients.end(); li++ )
    if( (**li).connected )
      {
        FD_SET( (**li).the_socket, &readfs );
        FD_SET( (**li).the_socket, &exceptfs );
        max_desc = max( max_desc, (**li).the_socket );
      }

  if( select( max_desc + 1, &readfs, NULL, &exceptfs, &tv ) < 0 )
    {
      cerr << "select failed." << endl;
      quit();
    }

  if( FD_ISSET( server_socket, &readfs ) )
    {
      cout << "Got new connection." << endl;
      accept_connection();
    }

  for( li = connected_clients.begin(); li != connected_clients.end(); li++ )
    if( (**li).connected && FD_ISSET((**li).the_socket, &exceptfs) )
      {
        cerr << "Exception for client." << endl;
        (**li).close_socket();
      }

  for( li = connected_clients.begin(); li != connected_clients.end(); li++ )
    {
      if( (**li).connected && FD_ISSET( (**li).the_socket, &readfs ) )
	{
	  int read = (**li).read_data();

	  Packet *P;
	  while( ! ((**li).read_buffers).empty() )
	    {
	      //Extract the string for the queue and make a packet with it
	      string data = (**li).read_buffers.front();
	      
	      P = make_packet( data );
	      (**li).read_buffers.pop_front();
	      
	      if( !P ) continue; //Jump to the next Packet
	      
	      P->get_string_from_netstring( data );
	      
	      if(P->packet_type() == PACKET_INIT)  //Maybe I should put this code in InitializationPacket::handle()
		{
		  //I must initialize this packet...
		  ServerNetConnection* temp = NULL;
		  client_t client = (client_t)P->handle_packet( *li );
		  if( client == (client_t)ROOT_CLIENT_CONNECTION)
		    {
		      if( !root_client )
			{
			  temp = new Chat_Connection((**li));
			  root_client = (Chat_Connection*) temp;
			  chat_nb ++;
			}
		      else //Close it as he tried to become a root
			(**li).close_socket();
		    }
		  else if( client == (client_t)CHAT_CLIENT_CONNECTION )
		    {
		      temp = new Chat_Connection( **li );

		      ServerMessagePacket TempMess("Server",
						   temp->name + string(" has join the game") 
						   );
		      send_packet_by_name( "all", &TempMess); 

		      chat_nb++;
		    }
		  else if( client == (client_t)ROBOT_CLIENT_CONNECTION )
		    {
		      temp = new Robot_Connection( **li );
		      robot_nb++;
		    }
		  else
		    cout<<"Connection not initialized\n";
		  
		  if(temp)
		    {
		      (**li).connected = false;

		      connected_clients.push_back( temp );
		      if( MetaServer )
			{
			  MetaServer->send_data( MetaServerDataPacket( name, version, port_number, 
									   chat_nb, 
									   language ).make_netstring() );
			}
		    }
		}
	      else if(P->packet_type() == PACKET_CHAT_MESSAGE)
		{
		  ChatMessagePacket* CM = (ChatMessagePacket*) P;
		  ServerMessagePacket * Temp;
		  P->handle_packet( *li );
		  send_packet_by_name( CM->dest, Temp = new ServerMessagePacket((*li)->name, CM->message) );
		  delete Temp;
		}
	      else //In any other case, handle it...
		P->handle_packet( *li );
						  
	      //Delete this old packet...
	      delete P;
	    }

	  if( read < 0 )
	    {
	      cout<<"I have to close it...\n";
	      (**li).close_socket();
	    }
	}
    }
  //go_through_read_buffers();
  remove_unconnected_sockets();
}

void
SocketServer::connect_to_metaserver( string hostname, int port_nb = 0 )
{
  if( port_nb == 0 )
    port_nb = meta_server_port;

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
          cerr << "Invalid hostname" << endl;
          exit(1);
        }
      src.sin_addr.s_addr = address;
      src.sin_family = AF_INET;
    }
  else if( (hostinfo = gethostbyname( hostname.c_str() )) == NULL )
    {
      cerr << "Failed looking up host" << endl;
      exit(1);
    }
  else
    {
      src.sin_family = hostinfo->h_addrtype;
      src.sin_addr = *((struct in_addr*) hostinfo->h_addr);
    }

  src.sin_port = htons( port_nb );

  if( (the_socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
    {
      cerr << "Socket failed" << endl;
      exit(1);
    }

  if( connect( the_socket, (struct sockaddr*) &src, sizeof(src) ) < 0 )
    {
      cerr << "Connect failed" << endl;
      exit(1);
    }

  MetaServer = new ServerNetConnection;

  MetaServer->the_socket = the_socket;
  MetaServer->connected = true;

  /*
   *  TO DELETE THE FOLLOWING AS SOON AS POSSIBLE!!!
   */
  
  cout<<"Informations for the metaserver\n";
  cout<<"Name : "; cin>>name;
  cout<<"Language : "; cin>>language;

  MetaServer->send_data( MetaServerInitializationPacket( RTB_NETPROTOCOL_V1 ).make_netstring() );
  MetaServer->send_data( MetaServerDataPacket( name, version, port_number, chat_nb, 
					       language ).make_netstring() );

}

void
SocketServer::accept_connection()
{
  int new_socket;
  struct sockaddr_in fromend;

  unsigned int fromlen = sizeof(fromend);

  if( (new_socket = accept( server_socket,
                            (struct sockaddr*) &fromend, &fromlen )) < 0 )
    {
      cerr << "Accepting connection failed" << endl;
      close( server_socket );
      quit();
    }

  struct ServerNetConnection* nc = new ServerNetConnection;
  nc->id = next_id;
  next_id++;

  nc->the_socket = new_socket;

  nc->make_nonblocking();

  /*struct hostent* from = gethostbyaddr( (char*)&fromend.sin_addr,
                                        sizeof(fromend.sin_addr),
                                        AF_INET );
  */
  nc->connected = true;

  connected_clients.push_back( nc );
  //cout<<"Now we are "<<connected_clients.size()<<endl;
}

void
SocketServer::go_through_read_buffers()
{
  list<ServerNetConnection*>::iterator li;
  for( li = connected_clients.begin(); li != connected_clients.end(); li++ )
    ;//cout << (**li).read_buffer << endl; // Very temporary
}


struct ptr_is_not_connected  //Use in the next function
{
  bool operator()(ServerNetConnection* const p) {return p->is_not_connected();}
};

void
SocketServer::remove_unconnected_sockets()
{
  bool has_deleted_chater = false;
  list<ServerNetConnection*>::iterator li;
  while( ( li = find_if( connected_clients.begin(), connected_clients.end(),
                         ptr_is_not_connected() ) )
         != connected_clients.end() )
    {
      if((*li)->type_conn() ==  ROBOT_CLIENT_CONNECTION )
	robot_nb--;
      else if((*li)->type_conn() ==  CHAT_CLIENT_CONNECTION )
	{
	  ServerMessagePacket Temp("Server",
				   (*li)->name + string(" has left the game") 
				   );
	  send_packet_by_name( "all", &Temp); 
		
	  if( (*li) == root_client )  //We can't play without a root
	    quit();
	  chat_nb--;
	  has_deleted_chater = true;
	}
      delete (*li);
      connected_clients.erase( li );
    }
  if( has_deleted_chater && MetaServer )
    {
      MetaServer->send_data( MetaServerDataPacket( name, version, port_number, chat_nb, 
						   language ).make_netstring() );
    }

}

void
SocketServer::send_packet_by_name(string name, ServerMessagePacket* P) //Change ServerMessagePacket to Packet when 'ready'
{
  /* This is to send a message to specific named chatters */

  //cout<<"Send ["<<P->message<<"] to "<<name<<endl;

  list<ServerNetConnection*>::iterator li;
  for( li = connected_clients.begin(); li != connected_clients.end(); li++ )
    {
      if( (**li).type_conn() == CHAT_CLIENT_CONNECTION )
	(**li).send_data( (*P).make_netstring() ); 
    }
}

SocketServer::~SocketServer()
{
  list<ServerNetConnection*>::iterator li;
  for(li = connected_clients.begin(); li != connected_clients.end();
      li ++)
    {
      (**li).close_socket();
      delete (*li);
    }
  connected_clients.clear();
  if(MetaServer)
    {
      MetaServer->close_socket();
      delete MetaServer;
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
  delete my_socketserver;
  exit( 0 );
}

int
main( int argc, char* argv[] )
{
  my_socketserver = new SocketServer;

  int port_nb;
  char answer;

  cout<<"Please enter a port number : "; //This would be a parameter when the prog would be called
  cin>>port_nb;


  my_socketserver->open_socket( port_nb );

  cout<<"Would you like to be listed in the meta-server : (y : yes) ";
  cin>>answer;
  
  if( answer == 'y' )
    my_socketserver->connect_to_metaserver( "", meta_server_port ); 

  signal(SIGALRM, exit_cleanly);
  signal(SIGTERM, exit_cleanly);
  signal(SIGINT,  exit_cleanly);
  
  cout<<"Welcome on RealTimeBattle 2.0.0 server(in development)\n";
  cout<<"Enjoy the game\n";

  while(1)
    {
      my_socketserver->check_socket();
    }

  my_socketserver->close_socket();
  quit();
  return 0;
}
