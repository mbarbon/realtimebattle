#include <math.h>
#include "Shape.h"
#include "Various.h"
//#include "Arena.h"

Shape::Shape()
{
  colour = make_gdk_colour( the_opts.get_l(OPTION_FOREGROUND_COLOUR) ); 
}

Shape::Shape(long col)
{
  colour = make_gdk_colour( col );
}

Line::Line()
{
  start_point = Vector2D(0.0, 0.0);
  direction = Vector2D(0.0, 0.0);
  length = 0.0;
  thickness = 0.0;
  last_drawn_start_point = Vector2D(-infinity,-infinity);
  last_drawn_direction = Vector2D(-infinity,-infinity);
  last_drawn_length = 0.0;
  last_drawn_thickness = 0.0;
}

Line::Line(const Vector2D& sp, const Vector2D& d, const double len, const double th)
{
  last_drawn_start_point = start_point = sp;
  last_drawn_direction = direction = d;
  last_drawn_length = length = len;
  last_drawn_thickness = thickness = th;
}

// Line::Line(const Vector2D& sp, const Vector2D& d, const double len, 
//            const double th, const double b_c, const double hardn)
// {
//   last_drawn_start_point = start_point = sp;
//   last_drawn_direction = direction = d;
//   last_drawn_length = length = len;
//   last_drawn_thickness = thickness = th;
//   bounce_coeff = b_c;
//   hardness_coeff = hardn;
// }

inline double
Line::get_distance(const Vector2D& pos, const Vector2D& vel, const double size)
{
  double det, s, t, d, t_middle;

  Vector2D y = start_point - pos;

  det = 1.0 / vedge(direction, vel);  
  t_middle = vedge(direction, y) * det;

  if( t_middle < 0 ) return infinity;

  d = size + thickness;
  if( det > 0 )
    {
      t = (vedge(direction, y) - d) * det;
      s = (vedge(vel,y) - d * dot(vel, direction)) * det;
    }
  else
    {
      t = (vedge(direction, y) + d) * det;
      s = (vedge(vel,y) + d * dot(vel, direction)) * det;
    }
  if( s < 0 || s > length ) return infinity;
  if( t < 0 && ((d=-dot(y, direction)) < 0.0 || d > length ) ) return infinity;

  return t;
}

Vector2D
Line::get_normal(const Vector2D& pos)
{
  if( vedge( direction , pos-start_point ) > 0.0 )
    return rotate90(direction);
  else
    return -rotate90(direction);
}

bool
Line::within_distance(const Vector2D& pos, const double size)
{
  Vector2D y = start_point - pos;
  
  return ((fabs(vedge(y, direction)) <= (size+thickness)) &&
          (-dot(y,direction) >= 0 && -dot(y,direction) <= length));
}

void
Line::draw_shape(bool erase)
{
  if( erase )
    the_gui.draw_line(last_drawn_start_point, last_drawn_direction, last_drawn_length, 
                      last_drawn_thickness, *(the_arena.get_background_colour_p()));
  last_drawn_start_point = start_point;
  last_drawn_direction = direction;
  last_drawn_length = length;
  last_drawn_thickness = thickness;
  the_gui.draw_line(start_point,direction,length,thickness,colour);
}

Circle::Circle()
{
  center = Vector2D(0.0, 0.0);
  radius = 0.0;
  last_drawn_center = Vector2D(-infinity,-infinity);
  last_drawn_radius = 0.0;
}

Circle::Circle(const Vector2D& c, const double r)
{
  last_drawn_center = center = c;
  last_drawn_radius = radius = r;
}

// Circle::Circle(const Vector2D& c, const double r, const double b_c, const double hardn)
// {
//   last_drawn_center = center = c;
//   last_drawn_radius = radius = r;
//   bounce_coeff = b_c;
//   hardness_coeff = hardn;
// }

inline double
Circle::get_distance(const Vector2D& pos, const Vector2D& vel, const double size)
{
  Vector2D y = center - pos;
  double speedsqr = lengthsqr(vel);
  double dt = dot(vel, y);
  double r = size+radius;
  double c = dt*dt + speedsqr * (r*r - lengthsqr(y));
  if( c < 0.0 || dt <= 0.0) return infinity;
  return (dt - sqrt(c))/speedsqr;
}

bool
Circle::within_distance(const Vector2D& pos, const double size)
{
  return (lengthsqr(center-pos) <= (size+radius)*(size+radius));
}

Vector2D
Circle::get_normal(const Vector2D& pos)
{
  return unit(pos - center);
}

void
Circle::draw_shape(bool erase)
{
  if( erase )
    the_gui.draw_circle(last_drawn_center, last_drawn_radius,
                        *(the_arena.get_background_colour_p()), true);
  last_drawn_center = center;
  last_drawn_radius = radius;
  the_gui.draw_circle(center,radius,colour,true);
}

InnerCircle::InnerCircle()
{
  center = Vector2D(0.0, 0.0);
  radius = 0.0;
  last_drawn_center = Vector2D(-infinity,-infinity);
  last_drawn_radius = 0.0;
}

InnerCircle::InnerCircle(const Vector2D& c, const double r)
{
  last_drawn_center = center = c;
  last_drawn_radius = radius = r;
}

// InnerCircle::InnerCircle(const Vector2D& c, const double r, const double b_c, const double hardn)
// {
//   last_drawn_center = center = c;
//   last_drawn_radius = radius = r;
//   bounce_coeff = b_c;
//   hardness_coeff = hardn;
// }

inline double
InnerCircle::get_distance(const Vector2D& pos, const Vector2D& vel, const double size)
{
  Vector2D y = center - pos;
  double speedsqr = lengthsqr(vel);
  double dt = dot(vel, y);
  double c = dt*dt + speedsqr*((size-radius)*(size-radius) - lengthsqr(y));
  return (dt + sqrt(c))/speedsqr;
}

bool
InnerCircle::within_distance(const Vector2D& pos, const double size)
{
  return (lengthsqr(center-pos) > (size-radius)*(size-radius));
}

Vector2D
InnerCircle::get_normal(const Vector2D& pos)
{
  return unit(center - pos);
}

void
InnerCircle::draw_shape(bool erase)
{
  if( erase )
    the_gui.draw_circle(last_drawn_center, last_drawn_radius,
                        *(the_arena.get_background_colour_p()), true);
  last_drawn_center = center;
  last_drawn_radius = radius;
  the_gui.draw_rectangle(the_arena.get_boundary()[0],
                         the_arena.get_boundary()[1],
                         colour, true);
  the_gui.draw_circle(center,radius,*(the_arena.get_background_colour_p()),true);
}
