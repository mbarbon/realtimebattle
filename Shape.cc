#include "Arena.h"

Line::Line()
{
  start_point = Vector2D(0.0, 0.0);
  direction = Vector2D(0.0, 0.0);
  length = 0.0;
  thickness = 0.0;
}

Line::Line(const Vector2D& sp, const Vector2D& d, const double len, const double th)
{
  start_point = sp;
  direction = d;
  length = len;
  thickness = th;
}

Circle::Circle()
{
  center = Vector2D(0.0, 0.0);
  radius = 0.0;
}

Circle::Circle(const Vector2D& c, const double r)
{
  center = c;
  radius = r;
}

inline double
Line::get_distance(const Vector2D& pos, const Vector2D& dir, const double size)
{
  Vector2D y = start_point - pos;
  double v,w,det,s1,s2;
  det = 1.0/vedge(direction, dir);
  s1 = ((v=vedge(dir,y)) + (w=((size + thickness) * dot(dir, direction))))*det;
  s2 = (v-w)*det;
  if( ( s1 < 0 || s1 > length ) && ( s2 < 0 || s2 > length ) ) return infinity;
  double t1,t2;
  s1 < 0.0 || s1 > length ? t1 = infinity : t1 = (vedge(direction, y) + (size+thickness))*det;
  s2 < 0.0 || s2 > length ? t2 = infinity : t2 = (vedge(direction, y) - (size+thickness))*det;
  
  t1 = min(t1,t2);
  if ( t1 <= 0.0 ) t1 = infinity;
  return t1;
}


inline double
Circle::get_distance(const Vector2D& pos, const Vector2D& dir, const double size)
{
  Vector2D y = center - pos;
  double dt = dot(dir, y);
  double c = dt*dt + (size+radius)*(size+radius) - lengthsqr(y);
  if( c < 0.0 || dt <= 0.0) return infinity;
  return dt - sqrt(c);
}

