#include "Packets.h"

#include <netinet/in.h>
#include <string>
#include <iostream.h>
#include <stdarg.h>

#include "NetConnection.h"
#include "ServerNetConnection.h"
#include "ServerSocket.h"
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
  //else I don't know this kind of packet;

  return 0;
}




int 
InitializationPacket::handle_packet(void* p_void)
{
  NetConnection* nc = (NetConnection*) p_void;
  //cout<<"In InitializationPacket : "<<data<<endl;
  if( data != "" )  //Have to find a way not to do that
    {
      vector<string> wordlist;
      //cout<<data<<endl;
      wordlist = split_string( data, wordlist ); //This function is buggy

      if(wordlist.size() == 3)
	{
	  if(wordlist[0] == "RTBNetV1")
	    {
	      nc->name = wordlist[2];
	      //cout<<"--> " <<wordlist[1]<<endl;

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
		  return (client_t)CHAT_CLIENT_CONNECTION;
		}
	    }
	}
    }
  //something wrong...
  return (client_t)UNINITIALIZED;
}

string 
InitializationPacket::make_netstring() const
{ /* Should be used by the server */ 
  return "";
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
  add_uint16_to_netstring( datastring.length() + 3, n_str );
  n_str += datastring;
  
  return n_str;
}

int 
CommandPacket::handle_packet(void* p_void)
{ /* Should be used by the server */
  Chat_Connection* cc = (Chat_Connection*) p_void;
  if( data == "Quit" && cc->is_root )
    quit();
  else
    cc->close_socket();
  return 1;
}

string 
ChatMessagePacket::make_netstring() const
{ /* Should be used by the server */ 
  return "";
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

int 
ServerMessagePacket::handle_packet( void* p_void ) 
{
  return 1;
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

  /*
    add_uint8_to_netstring( protocol, t_str );
    add_string_to_netstring( name, t_str );
  */

  string n_str;
  // add_uint8_to_netstring( PACKET_INIT, n_str );
  n_str = "S";  //maybe use a small "s"
  if( type == Add_Arena)
    n_str += "A";
  else if (type == Add_Robot)
    n_str += "R";
  else if (type == Del_Robot)
    n_str += "r";
  else if (type == Del_Arena)
    n_str += "A";

  //add_uint16_to_netstring( datastring.length() + 3, n_str );
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

  /*
    add_uint8_to_netstring( protocol, t_str );
    add_string_to_netstring( name, t_str );
  */

  string n_str;
  // add_uint8_to_netstring( PACKET_INIT, n_str );
  n_str = "MI";
  //add_uint16_to_netstring( datastring.length(), n_str );
  n_str += datastring;
  
  return n_str;
}

int 
MetaServerInitializationPacket::handle_packet( void* ) 
{
  return 0;
}

string
MetaServerDataPacket::make_netstring() const
{
  string datastring;

  datastring = name  + " " + version 
    + " " + Port_num + " " + Nb_Conn + " " + language;
  
  /*
    add_uint8_to_netstring( protocol, t_str );
    add_string_to_netstring( name, t_str );
  */

  string n_str;
  // add_uint8_to_netstring( PACKET_INIT, n_str );
  n_str = "MD";
  //add_uint16_to_netstring( datastring.length(), n_str );
  n_str += datastring;
  
  return n_str;
} 


int MetaServerDataPacket::handle_packet ( void* )
{
  return 0;
}

string
MetaServerAskInfoPacket::make_netstring() const
{
  string datastring;  //To do it quickly
  string n_str;
  // add_uint8_to_netstring( PACKET_INIT, n_str );
  n_str = "MA";
  //add_uint16_to_netstring( datastring.length(), n_str );
  n_str += datastring;
  
  return n_str;   
}

int 
MetaServerAskInfoPacket::handle_packet ( void* )
{
  return 0;
}
