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

#ifndef __GEOMETRICAL_OBJECTS__
#define __GEOMETRICAL_OBJECTS__

#include "Shape.h"
#include "Vector2D.h"

// ---------------------  Line : Shape ---------------------

class Line : public Shape
{
public:
  Line();
  Line(const Vector2D& sp, const Vector2D& d, const double len, const double th);
  Line(const Vector2D& sp, const Vector2D& d, const double len, const double th, 
       const double b_c, const double hardn);
  virtual ~Line() {}

  double get_distance(const Vector2D& pos, const Vector2D& vel, const double size);
  bool within_distance(const Vector2D& pos, const double size);
  Vector2D get_normal(const Vector2D& pos);

  Vector2D get_start_point() { return start_point; }
  Vector2D get_direction() { return direction; }
  double get_length() { return length; }
  double get_thickness() { return thickness; }

protected:
  Vector2D start_point;
  Vector2D direction;
  double length;
  double thickness;
};


// ---------------------  Circle : Shape ---------------------

class Circle : public Shape 
{
public:
  Circle();
  Circle(const Vector2D& c, const double r); 
  Circle(const Vector2D& c, const double r, const long int col); 
  Circle(const Vector2D& c, const double r, const double b_c, const double hardn);
  virtual ~Circle() {}

  double get_distance(const Vector2D& pos, const Vector2D& dir, const double size);
  bool within_distance(const Vector2D& pos, const double size);
  Vector2D get_normal(const Vector2D& pos);

  double get_radius() { return radius; }
  Vector2D get_center() { return center; }
  
protected:
  Vector2D center;
  double radius;
};

// ---------------------  InnerCircle : Shape ---------------------

class InnerCircle : public Shape 
{
public:
  InnerCircle();
  InnerCircle(const Vector2D& c, const double r); 
  InnerCircle(const Vector2D& c, const double r, const double b_c, const double hardn);
  ~InnerCircle() {}

  double get_distance(const Vector2D& pos, const Vector2D& dir, const double size);
  bool within_distance(const Vector2D& pos, const double size);
  Vector2D get_normal(const Vector2D& pos);

  double get_radius() { return radius; }
  Vector2D get_center() { return center; }
   
protected:
  Vector2D center;
  double radius;
};

#endif __GEOMETRICAL_OBJECTS__
