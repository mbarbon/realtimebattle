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

#ifndef __SHAPE__
#define __SHAPE__

#include <gdk/gdk.h>

#include "Arena.h"
#include "Vector2D.h"

//---------------------  Shape ---------------------

class Shape
{
public:
  Shape();
  Shape(long col);

  virtual ~Shape() {}

  virtual double get_distance(const Vector2D& pos, const Vector2D& vel, 
                              const double size) = 0;
  virtual bool within_distance(const Vector2D& pos, const double size) = 0;
  virtual Vector2D get_normal(const Vector2D& pos) = 0;

#ifndef NO_GRAPHICS
  virtual void draw_shape(bool erase) = 0;
#endif

  GdkColor get_colour() { return colour; }

protected:
  GdkColor colour;
};


// ---------------------  Line : Shape ---------------------

class Line : public virtual Shape
{
public:
  Line();
  Line(const Vector2D& sp, const Vector2D& d, const double len, const double th);
  Line(const Vector2D& sp, const Vector2D& d, const double len, const double th, 
       const double b_c, const double hardn);
  ~Line() {}

  double get_distance(const Vector2D& pos, const Vector2D& vel, const double size);
  bool within_distance(const Vector2D& pos, const double size);
  Vector2D get_normal(const Vector2D& pos);

#ifndef NO_GRAPHICS
  void draw_shape(bool erase);
#endif

  Vector2D get_start_point() { return start_point; }
  Vector2D get_direction() { return direction; }
  double get_length() { return length; }
  double get_thickness() { return thickness; }

protected:
  Vector2D start_point;
  Vector2D direction;
  double length;
  double thickness;

  Vector2D last_drawn_start_point;
  Vector2D last_drawn_direction;
  double last_drawn_length;
  double last_drawn_thickness;
};

// ---------------------  Circle : Shape ---------------------

class Circle : public virtual Shape 
{
public:
  Circle();
  Circle(const Vector2D& c, const double r); 
  Circle(const Vector2D& c, const double r, const double b_c, const double hardn);
  ~Circle() {}

  double get_distance(const Vector2D& pos, const Vector2D& dir, const double size);
  bool within_distance(const Vector2D& pos, const double size);
  Vector2D get_normal(const Vector2D& pos);

#ifndef NO_GRAPHICS
  void draw_shape(bool erase);
#endif
  
  double get_radius() { return radius; }
  Vector2D get_center() { return center; }
  //void add_to_center(const Vector2D& diff) { center += diff; }
  
protected:
  Vector2D center;
  double radius;

  Vector2D last_drawn_center;
  double last_drawn_radius;
  //  double radiussqr;
};

// ---------------------  InnerCircle : Shape ---------------------

class InnerCircle : public virtual Shape 
{
public:
  InnerCircle();
  InnerCircle(const Vector2D& c, const double r); 
  InnerCircle(const Vector2D& c, const double r, const double b_c, const double hardn);
  ~InnerCircle() {}

  double get_distance(const Vector2D& pos, const Vector2D& dir, const double size);
  bool within_distance(const Vector2D& pos, const double size);
  Vector2D get_normal(const Vector2D& pos);

#ifndef NO_GRAPHICS
  void draw_shape(bool erase);
#endif

  double get_radius() { return radius; }
  Vector2D get_center() { return center; }
  //void add_to_center(const Vector2D& diff) { center += diff; }
  
protected:
  Vector2D center;
  double radius;

  Vector2D last_drawn_center;
  double last_drawn_radius;
  //  double radiussqr;
};

// ---------------------  Wall : ArenaObject ---------------------

class Wall : public virtual ArenaObject 
{
public:
  object_type get_object_type() { return WALL; }

  friend void bounce_on_wall(class Robot& robot, const class Wall& wall, const Vector2D& normal);

protected:
  double bounce_coeff;
  double hardness_coeff;
};

class WallCircle : public virtual Wall, public virtual Circle
{
public:
  WallCircle(const Vector2D& c, const double r, 
             const double b_c, const double hardn) : Circle(c, r) 
    { bounce_coeff = b_c; hardness_coeff = hardn; }

  ~WallCircle() {}
};

class WallInnerCircle : public virtual Wall, public virtual InnerCircle
{
public:
  WallInnerCircle(const Vector2D& c, const double r, 
             const double b_c, const double hardn) : InnerCircle(c, r) 
    { bounce_coeff = b_c; hardness_coeff = hardn; }
  ~WallInnerCircle() {}
};

class WallLine : public virtual Wall, public virtual Line
{
public:
  WallLine(const Vector2D& sp, const Vector2D& d, const double len, const double th, 
           const double b_c, const double hardn) : Line(sp, d, len, th) 
    { bounce_coeff = b_c; hardness_coeff = hardn; }
  ~WallLine() {}
};

#endif __SHAPE__
