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


#ifndef __ROBOT_PACKETS_H__
#define __ROBOT_PACKETS_H__

#define USE_INITIALIZATION_PACKET 
#define SEND_INITIALIZATION_PACKET 

#define USE_COMMAND_PACKET
#  define RECV_COMMAND_PACKET
#  define SEND_COMMAND_PACKET

#define USE_ROBOTMESSAGE_PACKET
#  define RECV_ROBOTMESSAGE_PACKET
#  define SEND_ROBOTMESSAGE_PACKET

#include "Packets.h"

#endif
