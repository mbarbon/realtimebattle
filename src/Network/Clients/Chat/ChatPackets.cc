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


#include "ChatPackets.h"

#include <netinet/in.h>
#include <string>
#include <iostream.h>
#include <stdarg.h>
#include <unistd.h>

#include "NetConnection.h"
#include "Packets.h"
#include "String.h"

Packet*
make_packet( string& netstr )
{
  string type = netstr.substr(0,2);

  /* How to use the get_8uint_from_netstring???
    switch(type)
    {
    case PACKET_INIT : return new InitializationPacket(); break;
    }
  */

  if(type == "@C")
    {
      //This wouldn't be very usefull (CommandPacket would be a virtual class)
      return new CommandPacket( );
    }
  else if(type == "@m")
    {
      return new ServerMessagePacket( );
    }
  else if(type[0] == 'S')  
    //Maybe we should change the name of this class
    //so it would different if it's provide from chat or form server
    {
      return new SubmitPacket( type[1] );
    }
  else if(type == "MD")
    {
      return new MetaServerDataPacket( );
    }

  else if(type == "LR")
    {
      return new LaunchRobotPacket( );
    }
  //else I don't know this kind of packet;

  return 0;
}

string 
InitializationPacket::make_netstring() const
{
  string datastring;

  if (protocol ==   RTB_NETPROTOCOL_V1)
    {
      datastring = "RTBNetV1";
    }
  else
    {
      datastring = "RTBNetUnknown";
    }
  datastring += " ";

  if(client == ROBOT_CLIENT_CONNECTION)
    datastring += "Robot";
  else if(client == CHAT_CLIENT_CONNECTION)
    datastring += "Chat";
  else if(client == ROOT_CLIENT_CONNECTION)
    datastring += "Root";
  else
    datastring += "NoType";

  datastring += " " + name + " ";

  string n_str;
  n_str = "@I";
  n_str += datastring;
  
  return n_str;
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
  // add_uint8_to_netstring( PACKET_COMMAND, n_str );
  n_str = "@C";
  //add_uint16_to_netstring( datastring.length(), n_str );
  n_str += datastring;
  
  return n_str;
}

int 
CommandPacket::handle_packet(void* p_void)
{
  NetConnection* nc = (NetConnection*) p_void;
  /* Parse the string... */
  if( data == "Close" && nc )
    nc->close_socket();
  return 0;
}

string 
ChatMessagePacket::make_netstring() const
{

  string datastring;
  
  datastring += "[" + dest + "]";
  datastring += message;

  /*
    add_uint8_to_netstring( protocol, t_str );
    add_string_to_netstring( name, t_str );
  */

  string n_str;
  // add_uint8_to_netstring( PACKET_INIT, n_str );
  n_str = "@M";
  //add_uint16_to_netstring( datastring.length(), n_str );
  n_str += datastring;
  
  return n_str; 
}

int 
ServerMessagePacket::handle_packet( void* p_void ) 
{
  //Test if there are errors...
  int begin = data.find('[');
  int end = data.find(']');
  message = data.substr(end-begin+1, data.length()-end-1);
  cout<<"From "<<data.substr(begin+1, end-begin-1)
      <<" : "<<message<<endl;
  return 0;
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
  n_str = "S";
  if( type == Add_Arena)
    n_str += "A";
  else if (type == Add_Robot)
    n_str += "R";
  n_str += datastring;
  
  return n_str; 
}

int 
SubmitPacket::handle_packet( void* )
{

  if( type == Add_Arena )
    cout<<"Add the following arenas : ";
  else if( type == Del_Arena )
    cout<<"Delete the following arenas : ";
  else if( type == Add_Robot )
    cout<<"Add the following robots : ";
  else if( type == Del_Robot )
    cout<<"Delete the following robots : ";
  else
    return 1;
  
  cout<<data<<endl;
  return 0;
}


string 
MetaServerInitializationPacket::make_netstring() const
{
  string datastring;

  if (protocol ==   RTB_NETPROTOCOL_V1)
    {
      datastring = "RTBNetV1";
    }
  else
    {
      datastring = "RTBNetUnknown";
    }

  string n_str;
  n_str = "MI";
  n_str += datastring;
  
  return n_str;
}

int MetaServerDataPacket::handle_packet ( void* )
{
  cout<<"["<<data<<"]"<<endl;
  return 0;
}


string
MetaServerAskInfoPacket::make_netstring() const
{
  string datastring;
  string n_str;

  n_str = "MA";
  n_str += datastring;
  
  return n_str;   
}

int
LaunchRobotPacket::handle_packet( void* )
{
  cout<<"Launch the robot : "<<data<<endl;
  int pid;
  
  if( (pid = fork()) < 0 )
    {
      cerr<<"Couldn'd fork childprocess for robot "<<data<<endl;
      exit( 0 );
    }

  if( pid == 0 ) //Child process, to be the new robot client
    {
      
      if( execl("rtbrobot", "rtbrobot", "/bin/usr/local/w3c/...", "localhost", "1234", NULL) == -1 )
	{
	  cerr<<"Couldn't open rtbrobot\n";
	  exit( 0 );
	}
    }
  return 0;
}
