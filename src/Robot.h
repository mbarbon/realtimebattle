/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2000  Erik Ouchterlony and Ragnar Ouchterlony

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
#include "Process.h"

class Vector2D;


class Robot : public RollingObject
{
public:
  Robot(const string& filename);
  Robot(const int r_id, const long int col, const string& name);
  ~Robot();

  //  void move(const double timestep);  
  void update();
  
  bool update_position(const double timestep);

  void bounce_on_wall(const double bounce_c, const double hardness_c, const Vector2D& normal);
  double get_bounce_coeff( const double angle );
  double get_hardness_coeff( const double angle );

  friend void rolling_object_collision(RollingObject& robj1, 
                                       RollingObject& robj2, 
                                       const Vector2D& normal);

  void change_energy(const double energy_diff);
  void injury_from_collision(const double en, const double angle);

  void set_energy( const double en ) { energy = en; }
  void set_angle( const double a ) { robot_angle.angle = a; }

  void check_name_uniqueness();
  void get_messages();
  void send_message(enum message_to_robot_type ...);
  void set_values_before_game(const Vector2D& pos, double angle);
  void set_values_at_process_start_up();

  Process* get_process() { return process; }


  string get_robot_name() { return robot_name; }

  double get_energy() { return energy; }

  bool is_colour_given() { return colour_given; }
  void set_colour_given( const bool c ) { colour_given = c; }
  bool is_name_given() { return name_given; }
  


  Rotation get_robot_angle() { return robot_angle; }

  bool set_and_get_has_competed() 
    { if( has_competed ) return true; else { has_competed = true; return false; } }


private:
  message_from_robot_type name2msg_from_robot_type(char*);
  //  void move(const double timestep, int iterstep, const double eps);
  bool check_state_for_message(const message_from_robot_type msg_t, 
                               const enum state_t state1,
                               const enum state_t state2 = NO_STATE);

  class Process* process;

  int  send_rotation_reached;

  bool has_competed;

  double energy; 
  double extra_air_resistance;
  double brake_percent;

  Rotation robot_angle;
  
  double last_update_time;

  int robot_name_uniqueness_number;

  string plain_robot_name;      // Name given by robot
  string robot_name;            // plain_robot_name + uniqueness number

  bool colour_given;
  bool name_given;
  
  int position_this_game;
  double time_survived_in_sequence;
};

#endif __ROBOT__
