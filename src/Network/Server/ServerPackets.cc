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


#include <netinet/in.h>
#include <string>
#include <iostream.h>
#include <stdarg.h>

#include "NetConnection.h"
#include "ServerNetConnection.h"
#include "ServerSocket.h"
#include "ServerPackets.h"

#include "String.h"

#define SHUNT  //For everything that is temporary and that shouldn't be on the last verstion :-)

Packet*
make_packet( string& netstr )
{
  string type = netstr.substr(0,2);

  /*
    switch(type)
    {
    case PACKET_INIT : return new InitializationPacket(); break;
    }
  */

  if(type == "@I")
    {
      //This is an initialisation packet
      return new InitializationPacket( );
    }
  else if(type == "@C")
    {
      //This wouldn't be very usefull (CommandPacket would be a virtual class)
      return new CommandPacket( );
    }
  else if(type == "@M")
    {
      return new ChatMessagePacket( );
    }
  else if(type[0] == 'S')
    {
      return new SubmitPacket( type[1] );
    }
  else if( type == "@R" )
    {
      return new RobotMessagePacket( );
    }
  //else I don't know this kind of packet;

  return 0;
}

int 
InitializationPacket::handle_packet(void* p_void)
{
  NetConnection* nc = (NetConnection*) p_void;

  if( data != "" )  //Have to find a way not to do that
    {
      vector<string> wordlist;

      wordlist = split_string( data, wordlist ); //This function is buggy???

      if(wordlist.size() == 3)
	{
	  if(wordlist[0] == "RTBNetV1")
	    {
	      nc->name = wordlist[2];

	      // This is the good protocol
	      if( wordlist[1] == "Root" )
		{
		  return (client_t)ROOT_CLIENT_CONNECTION;
		}
	      else if( wordlist[1] == "Robot" )
		{
		  return (client_t)ROBOT_CLIENT_CONNECTION;  
		}
	      else if( wordlist[1] == "Chat" )
		{
		  //TODO : Check if the name isn't in the list...
		  return (client_t)CHAT_CLIENT_CONNECTION;
		}
	    }
	}
    }
  //something wrong...
  return (client_t)UNINITIALIZED;
}

CommandPacket::CommandPacket( const string& c /* , unsigned n, ... */ )
{
  comm = c;
  /*
    va_arg curseur;
    va_start( curseur, n);
    while(n--)
    arg.push_back(va_arg( curseur, string ));
    va_end (curseur);
  */
}


string 
CommandPacket::make_netstring() const
{
  string datastring;

  datastring = comm;
  /*
    for( vector<string>::iterator i = arg.begin();
    i != arg.end(); i++ )
    {
    datastring += " " + ( *i );
    }
  */

  string n_str;
  n_str = "@C";

  n_str += datastring;
  
  return n_str;
}

//The robot list we will send when the tournament will start (delete it as soon as possible
SHUNT class RobotElement {
SHUNT public:
SHUNT   string robot_name;
SHUNT   Chat_Connection* cc;
SHUNT };
SHUNT list<RobotElement*> RobotList; 





int 
CommandPacket::handle_packet(void* p_void)
{ /* Should be used by the server */
  Chat_Connection* cc = (Chat_Connection*) p_void;

  if(cc == root_client)
    {
      if( data == "Quit" )
	{
	  quit( );
	  return 0;
	}
      else if (data == "StartGame")
	{
	  SHUNT cout<<"Start the tournament\n";
	  
	  SHUNT for(list<RobotElement*>::iterator i = RobotList.begin(); i != RobotList.end(); i++)
	    SHUNT {
	    SHUNT ((**i).cc)->send_data( LaunchRobotPacket((**i).robot_name).make_netstring() );
	    SHUNT }

	  return 0;
	}
    }
  else 
    {
      if( data == "Quit" )
	{
	  cc->close_socket();
	  return 0;
	}
      else if( data == "StartGame" /* || data == "kick"... */) //Should we really do that ???
	{
	  (*cc).send_data(
			  ServerMessagePacket("Server",
					      "Only root can do that !!!"
					      ).make_netstring() 
			  );
	  return 0;
	}
    }

  //Should never come here :-)
  (*cc).send_data( ServerMessagePacket("Server",
				       "Unknown command"
				       ).make_netstring()
		   );
  return 1;
}

int 
ChatMessagePacket::handle_packet( void* p_void)
{
  NetConnection* nc = (NetConnection*) p_void;
  //Test if there are errors...
  int begin = data.find('[');
  int end = data.find(']');
  exp = nc->name;
  dest = data.substr(begin+1, end-begin-1);
  message = data.substr(end-begin+1, data.length()-end-1);
  return 0;
}


string 
ServerMessagePacket::make_netstring() const
{

  string datastring;
  
  datastring += "[" + exp + "]";
  datastring += message;

  string n_str;

  n_str = "@m";

  n_str += datastring;
  
  return n_str; 
}

SubmitPacket::SubmitPacket( int T , const vector<string>& FN)
{
  type = T;
  file_name = FN;
}

SubmitPacket::SubmitPacket( int T, string& s1 )
{
  type = T;
  file_name.push_back(s1);
}

SubmitPacket::SubmitPacket( int T, string& s1, string& s2 )
{
  type = T;
  file_name.push_back(s1);
  file_name.push_back(s2);
}

SubmitPacket::SubmitPacket( int T, string& s1, string& s2, string& s3 )
{
  type = T;
  file_name.push_back(s1);
  file_name.push_back(s2);
  file_name.push_back(s3);
}

SubmitPacket::SubmitPacket( int T, string& s1,
                            string& s2, string& s3, string& s4 )
{
  type = T;
  file_name.push_back(s1);
  file_name.push_back(s2);
  file_name.push_back(s3);
  file_name.push_back(s4);
}

SubmitPacket::SubmitPacket( int T, string& s1,
                            string& s2, string& s3, string& s4, string& s5 )
{
  type = T;
  file_name.push_back(s1);
  file_name.push_back(s2);
  file_name.push_back(s3);
  file_name.push_back(s4);
  file_name.push_back(s5);
}

string 
SubmitPacket::make_netstring() const
{
  string datastring;

  for(unsigned int i = 0; i < file_name.size(); i++)
    {
      datastring += "<" + file_name[i]  + ">";
    }

  string n_str;

  n_str = "S";  //maybe use a small "s"
  if( type == Add_Arena)
    n_str += "A";
  else if (type == Add_Robot)
    n_str += "R";
  else if (type == Del_Robot)
    n_str += "r";
  else if (type == Del_Arena)
    n_str += "A";

  n_str += datastring;
  
  return n_str; 
}

int 
SubmitPacket::handle_packet( void* p_void)
{
  Chat_Connection* cc = (Chat_Connection*) p_void;

  if( type == Add_Arena )
    cout<<"Add the following arenas : ";
  else if( type == Del_Arena )
    cout<<"Delete the following arenas : ";
  else if( type == Add_Robot )
    {
      int count = 0;
      cout<<"Add the following robots : ";
      while( 1 )
	{
	  //Extract the robot name
	  int begin = data.find('<');
	  int end = data.find('>');
	  string robot_name = data.substr(begin+1, end-begin-1);

	  //Add the robot to the list
	  SHUNT RobotElement* R;
	  SHUNT R = new RobotElement;
	  SHUNT R->robot_name = robot_name; R->cc = cc;
	  SHUNT RobotList.push_back(R);

	  count ++;

	  //Get the following robots
	  data = data.substr(end-begin+1, data.length()-end-1);
	  if( data == "" )
	    break;
	}
      cout<<count<<" more robots on the list\n";
    }
  else if( type == Del_Robot )
    cout<<"Delete the following robots : ";
  else
    return 1;

  return 0;
}


string 
MetaServerInitializationPacket::make_netstring() const
{
  string n_str;

  n_str = "MI";

  if (protocol ==   RTB_NETPROTOCOL_V1)
    {
      n_str += "RTBNetV1";
    }
  else
    {
      n_str += "RTBNetUnknown";
    }

  return n_str;
}

string
MetaServerDataPacket::make_netstring() const
{
  string n_str;
  n_str = "MD" 
    + name  + " " + version 
    + " " + Port_num + " " + Nb_Conn + " " + language;
  
  return n_str;
} 

string
MetaServerAskInfoPacket::make_netstring() const
{
  string n_str;

  n_str = "MA";
  
  return n_str;   
}

string
LaunchRobotPacket::make_netstring() const
{
  string n_str;
  n_str = string( "LR" ) + robot_name;
  return n_str;
}

string
RobotMessagePacket::make_netstring() const
{
  return ( string("@R") + robot_message );
}

int 
RobotMessagePacket::handle_packet( void* ) 
{
  robot_message = data;
  cout<<"The robot send "<<data<<endl;
  return 0;
}
