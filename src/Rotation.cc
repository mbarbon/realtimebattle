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
# include <config.h>
#endif

#include "Rotation.h"
#include <math.h>
#include <float.h>

bool
Rotation::update(const double timestep, const int send_rotation_reached)
{
  angle += timestep * vel;
  bool rot_reached = false;

  if( angle >= right && mode == ROTATE_TO_RIGHT )
    {
      set_rot( right, 0.0, -DBL_MAX, DBL_MAX, NORMAL_ROT);
      if( send_rotation_reached >= 1 ) rot_reached = true;
    }

  if( angle >= right && mode == SWEEP_RIGHT )
    {
      set_rot( right, -vel, left, right, SWEEP_LEFT);
      if( send_rotation_reached >= 2 ) rot_reached = true;
    }
  
  if( angle <= left && mode == ROTATE_TO_LEFT )
    {      
      set_rot( left, 0.0, -DBL_MAX, DBL_MAX, NORMAL_ROT);
      if( send_rotation_reached >= 1 ) rot_reached = true;
    }

  if( angle <= left && mode == SWEEP_LEFT )
    {
      set_rot( left, -vel, left, right, SWEEP_RIGHT);
      if( send_rotation_reached >= 2 ) rot_reached = true;
    }

  return rot_reached;
}
