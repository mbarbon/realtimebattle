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

#ifndef __ROBOT_SOCKET_H__
#define __ROBOT_SOCKET_H__

#include "ClientSocket.h"
#include "Process.h"

class SocketRobot : public SocketClient 
{
 public:
  SocketRobot() 
    : SocketClient(), P("/usr/bin/ls") 
    {};
  SocketRobot(const string robotnm) :
    P(robotnm) {};
  void check_connection();
  void initialize( const string& auth_key );

  friend class RobotMessagePacket;

 protected:
  Process P;
};

extern SocketRobot *my_socketclient;

#endif
