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

#ifndef __ROBOT__
#define __ROBOT__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif


#include <fstream.h>
#include <list>

#include "Messagetypes.h"

#include "Rotation.h"
#include "RollingObject.h"
#include "Structs.h"
#include "NetConnection.h"

class Vector2D;

class RobotBodyGadget;

class Robot : public RollingObject
{
public:
  Robot(const string&, int);
  Robot(const int r_id, const long int col, const string& name);
  ~Robot();

  void set_connection(NetConnection* nc); 

  //  void move(const double timestep);  
  void update();
  
  bool update_position(const double timestep);

  void bounce_on_wall(const double bounce_c, const double hardness_c, const Vector2D& normal);
  double get_bounce_coeff( const double angle );
  double get_hardness_coeff( const double angle );

  friend void rolling_object_collision(RollingObject& robj1, 
                                       RollingObject& robj2, 
                                       const Vector2D& normal);

  void change_health(const double health_diff);
  void injury_from_collision(const double en, const double angle);

  void set_health( const double h ) { health = h; }
  void set_angle( const double a ) { robot_angle.angle = a; }

  void check_name_uniqueness();
  void get_messages();
  void send_message(enum message_to_robot_type ...);
  void set_values_before_game(const Vector2D& pos, double angle);
  void set_values_at_process_start_up();

  //TODO : change the name to start_robot or something like that
  void start_process();

  string get_robot_name() { return robot_name; }

  double get_health() { return health; }

  bool is_colour_given() { return colour_given; }
  void set_colour_given( const bool c ) { colour_given = c; }
  bool is_name_given() { return name_given; }
  
  friend class CommandPacket;
  friend class SocketServer;

  Rotation get_robot_angle() { return robot_angle; }



private:
  message_from_robot_type name2msg_from_robot_type(char*);
  //  void move(const double timestep, int iterstep, const double eps);
  bool check_state_for_message(const message_from_robot_type msg_t, 
                               const enum state_t state1,
                               const enum state_t state2 = NO_STATE);

  class RobotBodyGadget* body;

  int  send_rotation_reached;

  static int nb_robots;

  NetConnection* owner;           //NOTE : not such a good idea ?
  bool connected_to_robot_client; //False if it is still connected to chat_client

  //Doesn't correspond to the description of a robot anymore...
  double health;
  double fuel;
  double temperature;

  double brake_percentage;

  Rotation robot_angle;
  
  double last_update_time;

  int robot_name_uniqueness_number;

  string robot_filename;
  string plain_robot_name;      // Name given by robot
  string robot_name;            // plain_robot_name + uniqueness number
                                // Would be used as the robot_id_key before the robot is really connected

  bool colour_given;
  bool name_given;
  
};

#endif // __ROBOT__
