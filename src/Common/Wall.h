/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony

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

#ifndef __WALL__
#define __WALL__

#include "GeometricalObjects.h"
#include "WallGadget.h"

//TODO : for each class : add the wallgadget corresponding to the description !!!

class Wall 
{
 public:
  Wall(const WallGadget* wg) : wg(wg) 
    {}
 protected:
  const WallGadget* wg;
};

class WallCircle : public Circle, public Wall
{
 public:
  WallCircle(const Vector2D& c, const double r, 
             const WallGadget* wg ) 
    : Circle(c, r), Wall( wg ) 
    { }

  ~WallCircle() {}
};

class WallInnerCircle : public InnerCircle, public Wall
{
public:
  WallInnerCircle(const Vector2D& c, const double r, 
		  const WallGadget* wg ) : InnerCircle(c, r) , Wall( wg )
    { }
  ~WallInnerCircle() {}
};

class WallLine : public Line, public Wall
{
public:
  WallLine(const Vector2D& sp, const Vector2D& d, const double len,
           const WallGadget* wg ) : Line(sp, d, len) , Wall( wg )
    { }
  ~WallLine() {}
};


class WallArc : public Arc, public Wall
{
public:
  WallArc(const Vector2D& c, const double& r1, const double& r2,
          const double a1, const double a2, 
          const WallGadget* wg ) : Arc(c, r1, r2, a1, a2), Wall( wg ) 
    {  }
  ~WallArc() {}
};


#endif // __WALL__
