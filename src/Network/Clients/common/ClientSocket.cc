#include <stdio.h>
#include <arpa/inet.h>
#include <iostream>
#include <cctype>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Packets.h"
#include "ClientSocket.h"

void
SocketClient::connect_to_server( string hostname, int port )
{
  struct sockaddr_in src;
  struct hostent* hostinfo;
  long address;
  int the_socket;

  if( port == 0 )
    port = default_server_port;

  if( hostname.empty() )
    hostname = "localhost";

  if( isdigit( hostname[0] ) )
    {
      if( (address = inet_addr( hostname.c_str() )) == (long)INADDR_NONE )
        {
          my_cons.write(C_FAIL,  "Invalid hostname" );
          return;
        }
      src.sin_addr.s_addr = address;
      src.sin_family = AF_INET;
    }
  else if( (hostinfo = gethostbyname( hostname.c_str() )) == NULL )
    {
      my_cons.write(C_FAIL,  "Failed looking up host" );
      return;
    }
  else
    {
      src.sin_family = hostinfo->h_addrtype;
      src.sin_addr = *((struct in_addr*) hostinfo->h_addr);
    }

  src.sin_port = htons( port );

  if( (the_socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
    {
      my_cons.write(C_FAIL,  "Socket failed" );
      return;
    }

  if( connect( the_socket, (struct sockaddr*) &src, sizeof(src) ) < 0 )
    {
      my_cons.write(C_FAIL,  "Connect failed" );
      return;
    }

  nc.the_socket = the_socket;
  nc.connected = true;
  nc.address = hostname;
}

void
SocketClient::check_connection()
{
  fd_set readfs;
  fd_set exceptfs;

  FD_ZERO( &readfs );
  FD_ZERO( &exceptfs );

  FD_SET( nc.the_socket, &readfs );
  FD_SET( nc.the_socket, &exceptfs );
  FD_SET( 0 , &readfs ); 
  
  my_cons.prompt_off();

  if( select( nc.the_socket + 1, &readfs, NULL, &exceptfs, NULL ) < 0 )
    {
      my_cons.write(C_FAIL,  "select failed." );
      exit(1);
    }

  if( FD_ISSET( nc.the_socket, &exceptfs ) )
    {
      my_cons.write(C_OK, "Exception for client." );
      nc.close_socket();
    }

  if( FD_ISSET( 0, &readfs ) )
    {
      char buffer[256];
      bzero(buffer, 256);
      fgets(buffer, 255, stdin);
      my_cons.prompt_enter();
      my_cons.update_prompt();
      string buf = string (buffer);
      if( buf == "quit\n")   //The quit event (maybe a click for a chat)
	{ 
	  //Use ClientRequest packet instead ...
	  nc.send_data( CommandPacket("Quit").make_netstring() );
	  exit( 0 );
	}
      else if( buf.substr(0,3) == "say")
	{
	  nc.send_data( ChatMessagePacket("all", 
					  buf.substr(4,buf.length()-5)
					  ).make_netstring() 
			);
	}      
      else if( buf.substr(0, 3) == "add")
	{
	  vector<string> filename;
	  if ( buf.substr(3, 5) == "arena" )
	    {
	      string arenaname;
	      my_cons.write(C_OK, "arena name (0 to finish) : ");
	      cin>>arenaname;
	      while ( arenaname != "0" )
		{
		  filename.push_back(arenaname);
		  my_cons.write(C_OK, "arena name (0 to finish) : ");
		  cin>>arenaname;
		}
	      my_cons.prompt_enter();
	      nc.send_data ( SubmitPacket(Add_Arena, filename).make_netstring() );
	    }
	}
      //read(0, buffer, MAX_BUFFER_SIZE-1);

      
    }

  if( (nc).connected && FD_ISSET( (nc).the_socket, &readfs ) )
    {
      if( (nc).read_data() < 0 )
	{
	  (nc).close_socket();
	}
      else
	{
	  Packet* P ;
	  while( (P = make_packet( nc.read_buffer )) )
	    {
	      nc.read_buffer = 
		P->get_string_from_netstring( nc.read_buffer );
	      switch(P->packet_type()) 
		{
		case PACKET_COMMAND :
		case PACKET_SERVER_COMMAND :
		case PACKET_SERVER_MESSAGE :
		  P->handle_packet( &nc );
		  break;

		case PACKET_META_DATA:
		  //send him all the informations
		  P->handle_packet( &nc ); 
		  break;

		default : break;
		}
	      delete P;
	    }
	}
    }
  my_cons.prompt_on();
}

void 
SocketClient::ask_for_info_to_metaserver()
{
  nc.send_data ( MetaServerAskInfoPacket().make_netstring() );
}



void
SocketClient::initialize( const string& name, const client_t& c )
{
//    string str = (string)( (char)(RTB_NETPROTOCOL_V1 & 0xff) ) + name;

//    int len = str.length();
//    string send_string = (string)( (char)(PACKET_INIT & 0xff) )

  InitializationPacket IP(  RTB_NETPROTOCOL_V1, 
			    c, name );
  nc.name = name;
  nc.send_data( IP.make_netstring() );
}

void
SocketClient::close_connection()
{
  close( nc.the_socket );
  nc.connected = false;
  nc.the_socket = -1;
}

bool
SocketClient::is_connected()
{
  return !nc.is_not_connected();
}

SocketClient* my_socketclient = new SocketClient;

void
exit_cleanly(int Sign)
{
  quit();
}

void
quit()
{
  delete my_socketclient;
  exit( 0 );
}
