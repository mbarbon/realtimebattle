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
  Vector2D y = start_point - pos;
  double v,w,det,s1,s2;
  det = 1.0/vedge(direction, vel);
  s1 = ((v=vedge(vel,y)) + (w=((size + thickness) * dot(vel, direction))))*det;
  s2 = (v-w)*det;
  if( ( s1 < 0 || s1 > length ) && ( s2 < 0 || s2 > length ) ) return infinity;
  double t1,t2;
  s1 < 0.0 || s1 > length ? t1 = infinity : t1 = (vedge(direction, y) + (size+thickness))*det;
  s2 < 0.0 || s2 > length ? t2 = infinity : t2 = (vedge(direction, y) - (size+thickness))*det;
  
  t1 = min(t1,t2);
  if ( t1 <= 0.0 ) t1 = infinity;
  return t1;
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
  double speedsqr = vel[0]*vel[0] + vel[1]*vel[1];
  double dt = dot(vel, y);
  double c = dt*dt + speedsqr*((size+radius)*(size+radius) - lengthsqr(y));
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
