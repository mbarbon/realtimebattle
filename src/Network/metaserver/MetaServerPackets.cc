#include <netinet/in.h>
#include <string>
#include <iostream.h>
#include <stdarg.h>

#include "MetaServerSocket.h"
#include "MetaServerNetConnection.h"
#include "Packets.h"
#include "String.h"

extern MetaServerSocket* my_metaserversocket;

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

  if(type == "MI")
    {
      return new MetaServerInitializationPacket( );
    }
  else if(type == "MD")
    {
      return new MetaServerDataPacket( );
    }
  else if(type == "MA")
    {
      return new MetaServerAskInfoPacket( );
    }
  //else I don't know this kind of packet;

  return 0;
}


/* 
 * THE FOLLOWING PACKETS ARE NOT USED AT ALL BY THE METASERVER
 */

int InitializationPacket::handle_packet(void* p_void) { return 0; }
string InitializationPacket::make_netstring() const { return ""; }

string ChatMessagePacket::make_netstring() const { return ""; }
int ChatMessagePacket::handle_packet( void* p_void){ return 0; }

string ServerMessagePacket::make_netstring() const { return ""; }
int ServerMessagePacket::handle_packet( void* p_void ) { return 0; }

SubmitPacket::SubmitPacket( int T , const vector<string>& FN) {}
SubmitPacket::SubmitPacket( int T , unsigned n, ... ) {}
string SubmitPacket::make_netstring() const { return ""; }
int SubmitPacket::handle_packet( void* ){ return 0; }


/*
 * THE FOLLOWING FUNCTIONS ARE NOT USED AT ALL BY THE METASERVER
 */
string MetaServerInitializationPacket::make_netstring() const { return ""; }
int CommandPacket::handle_packet(void* p_void){ return 0; }
string MetaServerAskInfoPacket::make_netstring() const { return ""; }


/*
 * THIS ARE THE PACKETS THAT METASERVER SEND
 */

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

string
MetaServerDataPacket::make_netstring() const
{
  string datastring;
  
  datastring = name + " " + version + " " + address + " " + Port_num 
    + " " + Nb_Conn + "  " + language;

  /*
    add_uint8_to_netstring( protocol, t_str );
    add_string_to_netstring( name, t_str );
  */

  string n_str;
  // add_uint8_to_netstring( PACKET_INIT, n_str );
  n_str = "MD"; 

  add_uint16_to_netstring( datastring.length(), n_str );
  n_str += datastring;
  
  return n_str; 
}

/*
 * THIS ARE THE PACKETS THAT METASERVER RECEIVE
 */

int 
MetaServerInitializationPacket::handle_packet( void* ) 
{
  /* Make sure this is a RTB_V2 Net Protocol */
  //cout<<data<<endl;
}

int 
MetaServerDataPacket::handle_packet ( void* p_void)
{
  MetaServerNetConnection* msnc = (MetaServerNetConnection*) p_void;
  if( data != "" )  //Have to find a way not to do that
    {
      vector<string> wordlist;
      wordlist = split_string( data, wordlist ); //This function is buggy
      
      if( wordlist.size() == 5 )
	{
	    msnc->name = wordlist[0];
	    msnc->version = wordlist[1];
	    sscanf(wordlist[2].c_str(), "%d", &(msnc->port_number));
	    sscanf(wordlist[3].c_str(), "%d", &(msnc->nb_conn));
	    msnc->language = wordlist[4];
	    msnc->summary  = "";
	    msnc->is_server = true;
	    //cout<<"There is a new server in the server list\n"; //In fact not in all cases
	}
    }
  return 0;
}

int 
MetaServerAskInfoPacket::handle_packet( void* nc )
{
  MetaServerNetConnection* msnc = (MetaServerNetConnection*) nc;
  
  list<MetaServerNetConnection*>::iterator i;
  
  for( i  = (my_metaserversocket->connected_servers).begin();
       i != (my_metaserversocket->connected_servers).end();
       i ++ )
    {
      if((*i)->is_server)
	{
	  /*string temp = MetaServerDataPacket( (*i)->name, (*i)->version, 
	    (*i)->address, (*i)->port_number, 
	    (*i)->nb_conn, (*i)->language ).make_netstring();
	    cout<<temp.substr(4, temp.length() - 4)<<endl;
	  */
	  msnc->send_data(  MetaServerDataPacket( (*i)->name, (*i)->version, 
						  (*i)->address, (*i)->port_number, 
						  (*i)->nb_conn, (*i)->language ).make_netstring() );
	}
    }

  if( ! (msnc->is_server) )
    {
      msnc->send_data( CommandPacket( "Close" ).make_netstring() );
      msnc->close_socket();
    }
  return 0;
}
