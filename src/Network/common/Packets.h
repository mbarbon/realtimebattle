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

#ifndef __PACKETS_H__
#define __PACKETS_H__

#include <string>
#include <stdio.h>
#include <stdarg.h>

#include "NetConnection.h"


class SocketServer;

enum packet_t
{
  PACKET_INIT = 1, //Otherwise we gonna have problems with eof '\0'
  PACKET_META_INIT,
  PACKET_INIT_REPLY,

  PACKET_META_DATA,
  PACKET_META_INFO,

  PACKET_COMMAND,
  PACKET_SERVER_COMMAND,

  PACKET_CLIENT_REQUEST,
  PACKET_SUBMIT,

  PACKET_SERVER_MESSAGE,
  PACKET_CHAT_MESSAGE,

  PACKET_SIGNAL,
  PACKET_FORCE_KILL,

  PACKET_ARENA,
  PACKET_LAUNCH_ROBOT, //Maybe use the submit packet too...

  PACKET_ROBOT_MESSAGE, //Send and receive a information between robot and server
  PACKET_ROBOT_SIGNAL,  //Maybe the robot_client will have to do it without instruction from server
};

class Packet
{
public:
  Packet( ) : data( "" ), size(0) {};
  Packet( string& d ) : data( d ) {};
  virtual ~Packet() {};
  virtual string make_netstring() const {return "";};  //Get what is to send in the socket
  string get_string_from_netstring( string& );         //Is it still in use???
  void remove_from_netstring( string& );

  virtual int handle_packet(void*) {return 0;};        //What is to do when we receive the packet
  virtual packet_t packet_type() = 0;                  //Return the type of the packet (See list below)

protected:
  //All this functions are not used anymore because there is problems with the '\0' char
  string& add_string_to_netstring( const string&, string& ) const;      
  string& add_uint8_to_netstring( const unsigned int&, string& ) const;
  string& add_uint16_to_netstring( const unsigned int&, string& ) const;
  unsigned int get_uint8_from_netstring( const string& ) const;
  unsigned int get_uint16_from_netstring( const string& ) const;
  
  string data;       //The data in the packet when reveived
  unsigned int size; //The size of the packet... is it very useful???
};


#ifdef USE_INITIALIZATION_PACKET
class InitializationPacket : /* virtual */ public Packet
{
public:
  InitializationPacket() 
    : name("") {};
  InitializationPacket( string& d )
    : Packet(d) {};

#ifdef SEND_INITIALIZATION_PACKET
  InitializationPacket( const network_protocol_t& p, 
                        const client_t& c, 
                        const string& n )
    : protocol(p), client(c), name(n) {}
  string make_netstring() const;
#endif SEND_INITIALIZATION_PACKET
#ifdef RECV_INITIALIZATION_PACKET
  int handle_packet( void* ) ; 
#endif RECV_INITIALIZATION_PACKET
  packet_t packet_type() { return PACKET_INIT; };

protected:
  network_protocol_t protocol;
  client_t client;
  string name;
};
#endif USE_INITIALIZATION_PACKET

#ifdef USE_METASERVER_INITIALIZATION_PACKET
class MetaServerInitializationPacket : public Packet
{
public:
  MetaServerInitializationPacket() 
    : protocol( (network_protocol_t) RTB_UNKNOWN ) {}
  MetaServerInitializationPacket( string& d )
    : Packet(d) {};
#ifdef SEND_METASERVER_INITIALIZATION_PACKET
  MetaServerInitializationPacket( const network_protocol_t& p )
    : protocol(p) {}
  string make_netstring() const;
#endif
#ifdef RECV_METASERVER_INITIALIZATION_PACKET
  int handle_packet( void* ) ; 
#endif
  packet_t packet_type() { return PACKET_META_INIT; };
protected:
  network_protocol_t protocol;
};
#endif

#ifdef USE_METASERVER_DATA_PACKET
class MetaServerDataPacket : public Packet
{
public:
  MetaServerDataPacket() 
    : name("") {};
  MetaServerDataPacket( string& d )
    : Packet(d) {};
#ifdef SEND_METASERVER_DATA_PACKET
  MetaServerDataPacket( const string& n, const string & ver, 
                        int Port, int NbConn, 
                        const string& lang )
    : name(n), language(lang), version(ver) 
  {
    char buf [16];
    sprintf(buf, "%d", Port);
    Port_num = buf;
    sprintf(buf, "%d", NbConn);
    Nb_Conn = buf;
  }
  MetaServerDataPacket( const string& n, const string & ver, 
                        const string& add, int Port, int NbConn, 
                        const string& lang ) 
    : name(n), language(lang), version(ver), address(add)
  { 
    char buf [16];
    sprintf(buf, "%d", Port);
    Port_num = buf;
    sprintf(buf, "%d", NbConn);
    Nb_Conn = buf;
  }

  string make_netstring() const;
#endif
#ifdef RECV_METASERVER_DATA_PACKET
  int handle_packet ( void* );
#endif
  packet_t packet_type() { return PACKET_META_DATA; };
protected:
  string name;
  string language;
  string version;
  string address;
  string Port_num;
  string Nb_Conn;
};
#endif  USE_METASERVER_DATA_PACKET

#ifdef USE_METASERVER_ASKINFO_PACKET
class MetaServerAskInfoPacket : public Packet
{
public:
#ifdef SEND_METASERVER_ASKINFO_PACKET
  string make_netstring() const;
#endif
#ifdef RECV_METASERVER_ASKINFO_PACKET
  int handle_packet ( void* );
#endif
  packet_t packet_type() { return PACKET_META_INFO; };
};
#endif

#ifdef USE_COMMAND_PACKET
class CommandPacket : /* virtual */ public Packet
{
public:
  CommandPacket()
    : comm("") {}
  CommandPacket( const string& c /* , unsigned n, ... */);
#ifdef SEND_COMMAND_PACKET
  string make_netstring() const;
#endif
#ifdef RECV_COMMAND_PACKET
  int handle_packet( void* );
#endif
  packet_t packet_type() { return PACKET_COMMAND; };
protected:
  string comm;
  vector<string> arg;
};
#endif USE_COMMAND_PACKET

#ifdef USE_CHAT_MESSAGE_PACKET
class ChatMessagePacket : public Packet
{
public:
  ChatMessagePacket()
    : exp(""), dest("all"), message("") {}
  ChatMessagePacket( const string& to, const string& mes )
    : exp(""), dest( to ), message(mes) {}
  ChatMessagePacket( const string& from, const string& mes, const string& to )
    : exp(from), dest(to), message(mes) {}
#ifdef SEND_CHAT_MESSAGE_PACKET
  string make_netstring() const;
#endif
#ifdef RECV_CHAT_MESSAGE_PACKET
  int handle_packet( void* );
#endif
  packet_t packet_type() { return PACKET_CHAT_MESSAGE; } 
  friend class SocketServer;
protected:
  string exp; //Would it be usefull (server knows who send him the packet...)
  string dest;
  string message;
};
#endif

#ifdef USE_SERVER_MESSAGE_PACKET
class ServerMessagePacket : public Packet
{
public:
  ServerMessagePacket()
    :exp(""), message("") {}
  ServerMessagePacket( const string& from, const string& mes )
    : exp(from), message(mes) {}
#ifdef SEND_SERVER_MESSAGE_PACKET
  string make_netstring() const;
#endif
#ifdef RECV_SERVER_MESSAGE_PACKET
  int handle_packet( void* );
#endif
  packet_t packet_type() { return PACKET_SERVER_MESSAGE; }
  friend class SocketServer;
protected:
  string exp;
  string message;
};
#endif


#ifdef USE_SUBMIT_PACKET

#define Add_Robot 1
#define Del_Robot 2
#define Add_Arena 3
#define Del_Arena 4

class SubmitPacket : public Packet
{
public:
  SubmitPacket( char T = 0 )  
  {
    if( T == 'R' )
      type = Add_Robot;
    else if( T == 'r')
      type = Del_Robot;
    else if( T == 'A')
      type = Add_Arena;
    else if( T == 'a' )
      type = Del_Arena;
    else
      type = 0;
  };
  SubmitPacket( int /* type */, const vector<string>& );
  // Hopefully no more than five strings are needed, if so add a new one!
  SubmitPacket( int /* type */, string& ); // one string
  SubmitPacket( int /* type */, string&, string& ); // two strings
  SubmitPacket( int /* type */, string&, string&, string& ); // three strings
  SubmitPacket( int /* type */, string&, string&, string&, string& ); // four strings
  SubmitPacket( int /* type */,
                string&, string&, string&, string&, string& ); // five strings
#ifdef SEND_SUBMIT_PACKET
  string make_netstring() const;
#endif
#ifdef RECV_SUBMIT_PACKET
  int handle_packet( void* );
#endif
  packet_t packet_type() { return PACKET_SUBMIT; }
  friend class SocketServer;
protected:
  int type;
  vector<string> file_name;
};
#endif USE_SUBMIT_PACKET

#ifdef USE_CLIENT_REQUEST_PACKET
class ClientRequestPacket : public Packet
{
public:
  ClientRequestPacket()
    :exp(""), message("") {}
#ifdef SEND_CLIENT_REQUEST_PACKET
  ClientRequestPacket( const string& req )
    : request( req ) {};
  string make_netstring() const;
#endif
#ifdef RECV_CLIENT_REQUEST_PACKET
  int handle_packet( void* );
#endif
  packet_t packet_type() { return PACKET_CLIENT_REQUEST; }
protected:
  string request;
};
#endif


#ifdef USE_LAUNCHROBOT_PACKET
class LaunchRobotPacket : /* virtual */ public Packet
{
public:
  LaunchRobotPacket() 
    : robot_name("") {};
#ifdef SEND_LAUNCHROBOT_PACKET
  LaunchRobotPacket( string& name )
    : robot_name(name) {};
  string make_netstring() const;
#endif SEND_LAUNCHROBOT_PACKET
#ifdef RECV_LAUNCHROBOT_PACKET
  int handle_packet( void* ) ; 
#endif RECV_LAUNCHROBOT_PACKET
  packet_t packet_type() { return PACKET_LAUNCH_ROBOT; };

protected:
  string robot_name;
};
#endif USE_LAUNCHROBOT_PACKET


#ifdef USE_ROBOTMESSAGE_PACKET
class RobotMessagePacket : /* virtual */ public Packet
{
public:
  RobotMessagePacket() 
    : robot_message("") {};
#ifdef SEND_ROBOTMESSAGE_PACKET
  RobotMessagePacket( string& mess )
    : robot_message(mess) {};
  string make_netstring() const;
#endif SEND_ROBOTMESSAGE_PACKET
#ifdef RECV_ROBOTMESSAGE_PACKET
  int handle_packet( void* ) ; 
#endif RECV_ROBOTMESSAGE_PACKET
  packet_t packet_type() { return PACKET_LAUNCH_ROBOT; };

protected:
  string robot_message;
};
#endif USE_ROBOTMESSAGE_PACKET



Packet* 
make_packet( string& );




#endif // __PACKETS_H__
