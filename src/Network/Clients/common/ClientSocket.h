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


#ifndef __SOCKETCLIENT_H__
#define __SOCKETCLIENT_H__

#include <string>

#include "NetConnection.h"
#include "Console.h"

class SocketClient
{
public:
  void connect_to_server( string hostname, int port = 0 );
  
  void check_connection();
  //  void write_to_server( const string& str );
  void initialize( const string&, const client_t& ); // RTBv2 initialization
  void close_connection();
  void ask_for_info_to_metaserver();

  bool is_connected();
  Console my_cons;

private:
  NetConnection nc;
};

extern SocketClient *my_socketclient;

void exit_cleanly(int Sign);
void quit();



#endif // __SOCKETCLIENT_H__
