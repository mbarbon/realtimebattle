#include <netinet/in.h>
#include <string>
#include <iostream.h>
#include <stdarg.h>

#include "MetaServerSocket.h"
#include "MetaServerNetConnection.h"
#include "MetaServerPackets.h"
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
  n_str = "@C";
  n_str += datastring;
  
  return n_str;
}

string
MetaServerDataPacket::make_netstring() const
{
  string datastring;
  
  datastring = name + " " + version + " " + address + " " + Port_num 
    + " " + Nb_Conn + "  " + language;

  string n_str;
  n_str = "MD"; 
  n_str += datastring;
  
  return n_str; 
}

/*
 * THIS ARE THE PACKETS THAT METASERVER RECEIVE
 */

int 
MetaServerInitializationPacket::handle_packet( void* ) 
{
  return 0;
  /* Make sure this is a RTB_V2 Net Protocol */
  /* Maybe just ask the protocol the server use... */
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
	  /*
	    string temp = MetaServerDataPacket( (*i)->name, (*i)->version, 
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
