/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2000  Erik Ouchterlony and Ragnar Ouchterlony

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>

#include "GeometricalObjects.h"

#define max(a,b) ((a) > (b) ? (a) : (b))

Line::Line()
{
  start_point = Vector2D(0.0, 0.0);
  direction = Vector2D(0.0, 0.0);
  length = 0.0;
  thickness = 0.0;
}

Line::Line(const Vector2D& sp, const Vector2D& d, const double len, const double th) :
  start_point( sp ),  direction( d ), length( len ), thickness( th )
{
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

  if( t_middle < 0 ) return DBL_MAX;

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
  if( s < 0 || s > length ) return DBL_MAX;
  if( t < 0 && ((d=-dot(y, direction)) < 0.0 || d > length ) ) return DBL_MAX;

  return max(t, 0.0);
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

Circle::Circle(const Vector2D& c, const double r, const long int col) : Shape(col)
{
  center = c;
  radius = r;
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
  if( c < 0.0 || dt <= 0.0) return DBL_MAX;
  return max( (dt - sqrt(c))/speedsqr, 0.0);
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

InnerCircle::InnerCircle()
{
  center = Vector2D(0.0, 0.0);
  radius = 0.0;
}

InnerCircle::InnerCircle(const Vector2D& c, const double r)
{
  center = c;
  radius = r;
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
  if( speedsqr == 0.0 ) return DBL_MAX;
  double dt = dot(vel, y);
  double c = dt*dt + speedsqr*((size-radius)*(size-radius) - lengthsqr(y));
  return max( (dt + sqrt(c))/speedsqr, 0.0 );
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


// ------- Arc -----------


Arc::Arc()
{
  center = Vector2D(0.0, 0.0);
  inner_radius = outer_radius = mid_radiussqr = 0.0;
  start_angle = -M_PI; end_angle = M_PI;
}

Arc::Arc(const Vector2D& c, const double r1, const double r2,
         const double a1, const double a2 )
{
  center = c;
  inner_radius = r1;
  outer_radius = r2;
  mid_radiussqr = 0.25 * (r1 + r2) * (r1 + r2);
  start_angle = a1;
  end_angle = a2;
}

// Arc::Arc(const Vector2D& c, const double r, const double b_c, const double hardn)
// {
//   center = c;
//   radius = r;
//   bounce_coeff = b_c;
//   hardness_coeff = hardn;
// }

inline double
Arc::get_distance(const Vector2D& pos, const Vector2D& vel, const double size)
{
  Vector2D d = center - pos;
  double speedsqr = lengthsqr(vel);
  if( speedsqr == 0.0 ) return DBL_MAX;

  double c, r, t;
  double dt = dot(vel, d);

  if( lengthsqr( d ) > mid_radiussqr )
    {
      // Outside circle
  
      r = size + outer_radius;
      c = dt*dt + speedsqr * (r*r - lengthsqr(d));
      if( c < 0.0 || dt <= 0.0) return DBL_MAX;
      t = max( (dt - sqrt(c))/speedsqr, 0);

      if( within_angle( vec2angle( vel * t - d ) ) )
        return t;

      // Did hit the outer circle, but not within the angles.
      // Can still hit inner circle.
    }

  

  // Test inner circle
      
  r = size - inner_radius;
  
  c = dt*dt + speedsqr*( r*r - lengthsqr(d) );
  if( c < 0.0 ) return DBL_MAX;  // Can happen if object outside circle
  
  t =  max( (dt + sqrt(c))/speedsqr, 0 );
  
  if( !within_angle( vec2angle( vel * t - d  ) ) )
    return DBL_MAX;


  return t;
}

bool
Arc::within_distance(const Vector2D& pos, const double size)
{
  Vector2D d = pos - center;
  double l = lengthsqr( d );

  return( l <= (size + outer_radius)*(size + outer_radius) &&
          ( l >= (size - inner_radius)*(size - inner_radius) || size >= inner_radius ) &&
          within_angle( vec2angle( d ) ) );
}


// a, start_angle and end_angle should all be betwwen -pi and pi
bool
Arc::within_angle( const double a )
{
  if( start_angle <= end_angle )
    return ( a >= start_angle && a <= end_angle );
  else
    return ( a >= start_angle || a <= end_angle );
}

Vector2D
Arc::get_normal(const Vector2D& pos)
{
  Vector2D d = center - pos;

  if( lengthsqr( d ) < mid_radiussqr )
    return unit( d );
  else
    return unit( -d );
}

