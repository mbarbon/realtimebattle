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


#include <stdio.h>
#include <arpa/inet.h>
#include <iostream>
#include <cctype>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ChatPackets.h"
#include "ChatSocket.h"

void
SocketChat::check_connection()
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
	  sleep( 1 );  //Maybe a select???
	  cout<<"Ciao\n";
	  exit( 0 );
	}
      else if( buf.substr(0,3) == "say")
	{
	  nc.send_data( ChatMessagePacket("all", 
					  buf.substr(4,buf.length()-5)
					  ).make_netstring() 
			);
	}      
      else if( buf == "start\n")
	{
	  nc.send_data( CommandPacket("StartGame").make_netstring() );
	}
      else if( buf.substr(0, 3) == "add")
	{
	  vector<string> filename;
	  if ( buf.substr(4, 5) == "robot" )
	    {
	      string robotname;
	      my_cons.write(C_OK, "robot name (0 to finish) : ");
	      cin>>robotname;
	      while ( robotname != "0" )
		{
		  filename.push_back(robotname);
		  my_cons.write(C_OK, "robot name (0 to finish) : ");
		  cin>>robotname;
		}
	      my_cons.prompt_enter();
	      nc.send_data ( SubmitPacket(Add_Robot, filename).make_netstring() );
	    }
	}
      //read(0, buffer, MAX_BUFFER_SIZE-1);

      
    }

  if( (nc).connected && FD_ISSET( (nc).the_socket, &readfs ) )
    {
      int read = nc.read_data();

      /* Analyse the packets even if we had a problem during reading */
      Packet *P;
      while( ! (nc.read_buffers).empty() )
	{
	  //Extract the string for the queue and make a packet with it
	  string data = nc.read_buffers.front();

	  P = make_packet( data );
	  nc.read_buffers.pop_front();
	      
	  if( !P ) continue; //Jump to the next Packet
	  
	  P->get_string_from_netstring( data );

	  P->handle_packet( &nc );

	  delete P;
	}

      if( read < 0 )
	{ //We had a problem when we last read the socket...
	  cout<<"Problem while reading\n";
	  (nc).close_socket();
	}
    }
  my_cons.prompt_on();
}

void 
SocketChat::ask_for_info_to_metaserver()
{
  nc.send_data ( MetaServerAskInfoPacket().make_netstring() );
}


SocketChat* my_socketclient = new SocketChat;

void
quit()
{
  delete my_socketclient;
  exit( 0 );
}
