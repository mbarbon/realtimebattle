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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Robot.h"
#include "Shot.h"
#include "ArenaController.h"
#include "Arena.h"
#include "Various.h"
#include "OptionHandler.h"

#include "Gadgets/ShotGadget.h"

Shot::Shot(const Vector2D& c, //const double r, 
           const Vector2D& vel, 
           ShotGadget& sg, const int shot_id = -1)           
  : MovingObject(c, the_opts.get_d(OPTION_SHOT_RADIUS), vel), my_shotgadget(sg)
{
  if( shot_id == -1 )
    id = 0;//the_arena.increase_shot_count();
  else
    id = shot_id;
}

void
Shot::move(const double timestep)
{
  object_type closest_shape;
  Shape* colliding_object;
  double time_to_collision = the_arena.
    get_shortest_distance(center, velocity, radius, closest_shape, colliding_object);

  if( time_to_collision > timestep )
    {
      center += timestep*velocity;
    }
  else
    {
      the_arena.collision( this, colliding_object, time_to_collision );
    }
}

void
Shot::move_no_check(const double timestep)
{
  center += timestep*velocity;
}

//  void
//  shot_collision(Shot* shot1p, Shot* shot2p)
//  {
//    shot_collision(shot1p, shot2p->velocity, shot2p->energy);
//    shot2p->die();
//    the_arena.get_object_lists()[SHOT].remove( shot2p );
//  }

//  void
//  shot_collision(Shot* shot1p, const Vector2D& shot2_vel, const double shot2_en)
//  {
//    Vector2D vel = ( shot1p->energy * shot1p->velocity + 
//                     shot2_en * shot2_vel ) / ( shot1p->energy + shot2_en );

//    double en = dot( shot1p->energy * unit(shot1p->velocity) + shot2_en * unit(shot2_vel),  
//                     unit(vel));

//    if( en < the_opts.get_d(OPTION_SHOT_MIN_ENERGY) || 
//        length(vel) < the_opts.get_d(OPTION_SHOT_SPEED) * 0.5 )
//      {
//        shot1p->die();
//      }
//    else
//      {
//        shot1p->velocity = vel;
//        shot1p->energy = en;
//        realtime_arena.print_to_logfile('D', (int)'S', shot1p->id);
//        shot1p->id = the_arena.increase_shot_count();
//        realtime_arena.print_to_logfile('S', shot1p->id, shot1p->center[0], shot1p->center[1], 
//                                        shot1p->velocity[0], shot1p->velocity[1]);
      

//      }
//  }
