/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony

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
#include "Packets.h"
#include "String.h"


bool
PacketFactory::add_connection( NetConnection* nc, string more_arg )
{
  my_connections.push_back( nc );
  return true;
}

void
PacketFactory::remove_connection( NetConnection* nc )
{
  for(list<NetConnection*>::iterator li = my_connections.begin(); li != my_connections.end(); li ++)
    if((*li) == nc) {
      my_connections. erase( li );
      return;
    }
}

void
PacketFactory::broadcast( Packet* p )
{
  string data = p->make_netstring();
  
  for( list<NetConnection*>::iterator li = my_connections.begin(); li != my_connections.end(); li++ )
    (**li).send_data( data ); 
}



