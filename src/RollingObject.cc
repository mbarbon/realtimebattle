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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>
#include <math.h>
#include <typeinfo>

#include "RollingObject.h"
#include "Robot.h"
#include "Options.h"
#include "ArenaBase.h"
#include "ArenaController.h"

RollingObject::RollingObject()
{
  acceleration = Vector2D(0.0, 0.0);
}

RollingObject::RollingObject(const Vector2D& c, const double r, const Vector2D& vel)
      : MovingObject(c, r, vel)
{
  acceleration = Vector2D(0.0, 0.0);
}



void
RollingObject::set_position( const double x, const double y )
{
  center = Vector2D(x, y);
}

void
RollingObject::bounce_on_wall(const double bounce_c, 
                              const double hardness_c, 
                              const Vector2D& normal)
{
  double e = get_bounce_coeff() * bounce_c;
  velocity -= (1.0 + e) * dot(normal, velocity) * normal;
}

void
rolling_object_collision(RollingObject& robj1, RollingObject& robj2, const Vector2D& normal)
{
  double h, e;
  
  double angle = vec2angle(normal);
  double nangle = angle + M_PI;

  h = robj1.get_hardness_coeff( angle ) * robj2.get_hardness_coeff( nangle );
  e = robj1.get_bounce_coeff( angle ) * robj2.get_bounce_coeff( nangle );

  Vector2D start_vel1 = robj1.velocity;
  Vector2D start_vel2 = robj2.velocity;
  double mass1 = robj1.get_mass();
  double mass2 = robj2.get_mass();

  double impulse = mass1 * mass2 / ( mass1 + mass2 ) * (1.0 + e) * 
    dot(robj2.velocity - robj1.velocity, normal);

  robj1.velocity += (impulse/mass1) * normal;
  robj2.velocity -= (impulse/mass2) * normal;


  if( typeid( robj1 ) == typeid( Robot ) )
    ((Robot&)robj1).injury_from_collision( 0.5 * h * robj1.get_mass() * 
                                           lengthsqr(start_vel1 - robj1.velocity), angle );

  if( typeid( robj2 ) == typeid( Robot ) )
    ((Robot&)robj2).injury_from_collision( 0.5 * h * robj2.get_mass() * 
                                           lengthsqr(start_vel2 - robj2.velocity), angle );

  //
  // Only for robots !!
  //
//    double an = vec2angle(-normal);
//    double en_diff = 0.5 * mass * lengthsqr(start_vel1 - robj1.velocity);
//    double injury = en_diff * 0.5 * (h1 + h2) * (1.0-e) * (1.0-p1);
//    robj1.change_energy(-injury);
//    robj1.send_message(COLLISION, ROBOT, an-robj1.robot_angle.pos);

//    an = vec2angle(normal);
//    en_diff = 0.5 * mass * lengthsqr(start_vel2 - robj2.velocity);
//    injury = en_diff * 0.5 * (h1 + h2) * (1.0-e) * (1.0-p2);
//    robj2.change_energy(-injury);
//    robj2.send_message(COLLISION, ROBOT, an-robj2.robot_angle.pos);
}




void
RollingObject::update_velocity(const double timestep)
{
  double gt = the_opts.get_d(OPTION_GRAV_CONST);
  double fric = the_opts.get_d(OPTION_ROLL_FRICTION);
  double air_res = the_opts.get_d(OPTION_AIR_RESISTANCE);

  velocity = velocity * max(0.0, 1.0 - (gt*fric + air_res)*timestep ) +
    timestep*acceleration;
}


// Assuming constant velocity between bounces.
void
RollingObject::move(const double timestep)
{
  /*
  move(timestep, 1, timestep / 50.0);

  if( is_alive() )
    realtime_arena.print_to_logfile('R', id, center[0], center[1],
                                    robot_angle.pos, cannon_angle.pos, 
                                    radar_angle.pos, energy);
  */
}

void
RollingObject::move(const double timestep, int iterstep, const double eps)
{
  /*
  object_type closest_shape;
  Shape* colliding_object;
  double time_to_collision = 
    the_arena.get_shortest_distance(center, velocity, radius, 
                                    closest_shape, colliding_object, this);
  if( time_to_collision > timestep )
    {
      center += timestep*velocity;
    }
  else
    {
      if( iterstep > 10 ) time_to_collision = max( time_to_collision , eps );
      double time_remaining = timestep - time_to_collision; 
      center += time_to_collision*velocity;
      //      Vector2D new_center = center - min(eps, time_to_collision)*velocity;
      

      switch( closest_shape )
        {
        case WALL:
          {
            Vector2D normal = colliding_object->get_normal(center);
            bounce_on_wall(colliding_object->get_bounce_coeff(), 
                           colliding_object->get_hardness_coeff(), normal);
            center += normal*eps;
          }
          break;
        case ROBOT:
          {
            Vector2D normal = ((Robot*)colliding_object)->get_normal(center);
            bounce_on_robot(*this, *(Robot*)colliding_object, normal);
            time_remaining = 0.0;
          }
          break;
        case SHOT:
          {
            Shot* shotp =(Shot*)colliding_object;
            double en =  -shotp->get_energy();
            change_energy( en );
            send_message(COLLISION, SHOT, vec2angle(shotp->get_center()-center)-robot_angle.pos);
            shotp->die();
            the_arena.get_object_lists()[SHOT].remove( shotp );
          }
          break;
        case COOKIE:
          {
            Cookie* cookiep =(Cookie*)colliding_object;
            double en =  cookiep->get_energy();
            change_energy( en );
            send_message(COLLISION, COOKIE, vec2angle(cookiep->get_center()-center)-robot_angle.pos);
            cookiep->die();
            the_arena.get_object_lists()[COOKIE].remove( cookiep );
          }
          break;
        case MINE:
          {
            Mine* minep =(Mine*)colliding_object;
            double en =  -minep->get_energy();
            change_energy( en );
            send_message(COLLISION, MINE, vec2angle(minep->get_center()-center)-robot_angle.pos);
            minep->die();
            the_arena.get_object_lists()[MINE].remove( minep );
          }
          break;
        default:
          Error(true, "Collided with unknown object", "RollingObject::move");
          break;
        }
      
      //      center = new_center;
      //if( iterstep % 10 == 0 ) 
      //  cout << "Iterstep: " << iterstep << "   time_remaining: " << time_remaining
      //       << "  Collided on: " << closest_shape << "   time_to_col: " << time_to_collision << endl;
      if( iterstep > 65 ) Error(true, "Too many bounces, must be a bug!", "RollingObject::move");
      if( alive && time_remaining > 0.0 ) move( time_remaining, iterstep + 1, eps );
    }
  */
}

