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


#include "RobotPacketFactory.h"
#include <iostream>

RobotPacketFactory::RobotPacketFactory()
{
  cout<<"Creating the robot packet factory\n";
}

Packet*
RobotPacketFactory::MakePacket(string & s, NetConnection*)
{
  cout<<"Making a packet from "<<s<<endl;
  if( s == "@CQuit" ) { exit( 0 ); }
  return NULL;
}
