/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-1999  Erik Ouchterlony and Ragnar Ouchterlony

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

#ifndef __EXTRAS__
#define __EXTRAS__

#include "Arena.h"
#include "Shape.h"

// ---------------------  Extras : ArenaObject  ---------------------

class Extras : public virtual ArenaObject
{
public:
  //Extras(const Vector2D& c, const double r); 
  //~Extras() {}
  
};

// ---------------------  Cookie : Extras  ---------------------

class Cookie : public virtual Extras, public virtual Circle
{
public:
  Cookie(const Vector2D& c, const double r, const double e); 
  ~Cookie() {}
  
  object_type get_object_type() { return COOKIE; }
  bool is_alive() { return alive; }
  void die();
  double get_energy() { return energy; }

private:
  double energy;
  bool alive;
};

// ---------------------  Mine : Extras  ---------------------

class Mine : public virtual Extras, public virtual Circle
{
public:
  Mine(const Vector2D& c, const double r, const double e); 
  ~Mine() {}

  object_type get_object_type() { return MINE; }
  bool is_alive() { return alive; }
  void die();
  double get_energy() { return energy; }

private:
  double energy;
  bool alive;
};

#endif __EXTRAS__

