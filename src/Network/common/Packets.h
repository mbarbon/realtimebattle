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
  PACKET_ROBOT_MESSAGE,

  PACKET_SIGNAL,
  PACKET_FORCE_KILL,

  PACKET_ARENA,
  PACKET_LAUNCH_ROBOT, //Maybe use the submit packet too...
};

class Packet
{
public:
  Packet( ) : data( "" ), size(0) {};
  Packet( string& d ) : data( d ) {};
  virtual ~Packet() {};
  virtual string make_netstring() const {return "";};
  string get_string_from_netstring( string& );
  void remove_from_netstring( string& );

  virtual int handle_packet(void*) {return 0;};
  virtual packet_t packet_type() = 0;

protected:
  string& add_string_to_netstring( const string&, string& ) const;
  string& add_uint8_to_netstring( const unsigned int&, string& ) const;
  string& add_uint16_to_netstring( const unsigned int&, string& ) const;
  unsigned int get_uint8_from_netstring( const string& ) const;
  unsigned int get_uint16_from_netstring( const string& ) const;
  
  string data;
  unsigned int size;
};


#ifdef USE_INITIALIZATION_PACKET
class InitializationPacket : /* virtual */ public Packet
{
public:
  InitializationPacket() 
    : name("") {};
  InitializationPacket( string& d )
    : Packet(d) {};
  InitializationPacket( const network_protocol_t& p, 
                        const client_t& c, 
                        const string& n )
    : protocol(p), client(c), name(n) {}

#ifdef SEND_INITIALIZATION_PACKET
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
  MetaServerInitializationPacket( const network_protocol_t& p )
    : protocol(p) {}
#ifdef SEND_METASERVER_INITIALIZATION_PACKET
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

#ifdef SEND_METASERVER_DATA_PACKET
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

/*
  class InitializationReplyPacket : virtual public Packet
  {
  public:
  InitializationReplyPacket( const string& data );
  private:
  
  };
*/


#ifdef USE_CLIENT_REQUEST_PACKET
class ClientRequestPacket : public Packet
{
public:
  ClientRequestPacket()
    :exp(""), message("") {}
  ClientRequestPacket( const string& req )
    : request( req ) {};
#ifdef SEND_CLIENT_REQUEST_PACKET
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

Packet* 
make_packet( string& );


#endif // __PACKETS_H__
