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

#ifndef __ROTATION__
#define __ROTATION__

enum rotation_mode_t 
{ 
  NORMAL_ROT, 
  SWEEP_LEFT, 
  SWEEP_RIGHT, 
  ROTATE_TO_RIGHT, 
  ROTATE_TO_LEFT 
};

struct Rotation
{
  Rotation() : angle(0), vel(0), left(0), right(0), mode(NORMAL_ROT) {}
  Rotation(const double a, const double v, 
           const double l, const double r, const rotation_mode_t m) :
    angle(a), vel(v), left(l), right(r), mode(m) {}

  void set_rot(const double a, const double v, const double l, 
               const double r, const rotation_mode_t m) 
    { 
      angle=a; 
      vel=v; 
      left=l; 
      right=r; 
      mode=m; 
    }

  bool update(const double timestep, const int send_rotation_reached);

  double angle;
  double vel;
  double left;
  double right;
  rotation_mode_t mode;
};

#endif __ROTATION__
