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

#ifndef __ROLLING_OBJECT__
#define __ROLLING_OBJECT__

class Vector2D;

#include "MovingObject.h"


class RollingObject : public MovingObject
{
public:
  RollingObject();
  RollingObject(const Vector2D& c, const double r, const Vector2D& vel);
  ~RollingObject() {}

  void move(const double timestep);
  void update_velocity(const double timestep);

  void bounce_on_wall(const double bounce_c, const double hardness_c, const Vector2D& normal);

  friend void rolling_object_collision(RollingObject& robj1, 
                                       RollingObject& robj2, 
                                       const Vector2D& normal);

  void set_position(const double x, const double y );

protected:
  void move(const double timestep, int iterstep, const double eps);


protected:
  Vector2D acceleration;
};

#endif __ROLLING_OBJECT__
