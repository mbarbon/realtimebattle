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

#ifndef __EXPLOSION__
#define __EXPLOSION__

#include "GeometricalObjects.h"
#include "Gadgets/ExplosionGadget.h"

class Vector2D;

class Explosion : public Circle
{
public:
  Explosion(const Vector2D& c, const ExplosionGadget& gad, const double st_time);
  ~Explosion() {}

  double get_percentage_inside( const Vector2D& c, const double r );
  
  void update_size( const double time );

protected:
  
  ExplosionGadget my_gadget;

  double start_time;
  double max_size;
  double reach_maximum_time;
  double start_shrink_time;
  double end_time;
};

#endif __EXPLOSION__
