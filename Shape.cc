#include "Arena.h"
#include "math.h"

void
Shape::set_colour( int red, int green, int blue )
{
  colour.red = red;
  colour.green = green;
  colour.blue = blue;
  //  gdk_color_alloc (colormap, &colour);
}

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
Line::get_normal(const Vector2D& pos, const Vector2D& vel, const double size)
{
  return Vector2D(0.0, 0.0);  
}

bool
Line::within_distance(const Vector2D& pos, const double size)
{
  Vector2D y = start_point - pos;
  
  return ((fabs(vedge(y, direction)) <= (size+thickness)) &&
          (-dot(y,direction) >= 0 && -dot(y,direction) <= length));
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
  return ((center[0]-pos[0])*(center[0]-pos[0])+
          (center[1]-pos[1])*(center[1]-pos[1]) 
          <= (size+radius)*(size+radius));
}

Vector2D
Circle::get_normal(const Vector2D& pos, const Vector2D& vel, const double size)
{
  return Vector2D(0.0, 0.0);
}
