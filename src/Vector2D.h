/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998  Erik Ouchterlony and Ragnar Ouchterlony

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

#ifndef __VECTOR2D__
#define __VECTOR2D__

#include <iostream.h>

class Vector2D
{
public:
  Vector2D();
  Vector2D(const Vector2D&);
  Vector2D(double,double);
  ~Vector2D() {}
  
  friend Vector2D operator+(const  Vector2D&, const Vector2D&);
  friend Vector2D operator-(const Vector2D&, const Vector2D&);
  friend Vector2D operator-(const Vector2D&);
  Vector2D& operator=(const Vector2D&);
  friend int operator==(const Vector2D&, const Vector2D&);
  //friend double operator*(const Vector2D&, const Vector2D&);  
  friend Vector2D operator*(const Vector2D&, const double);  
  friend Vector2D operator*(const double, const Vector2D&);  
  friend Vector2D operator/(const Vector2D&, const double);  
  friend ostream& operator<<(ostream&, const Vector2D&);  
  friend istream& operator>>(istream&, Vector2D&);  

  Vector2D& operator+=(const Vector2D&);
  Vector2D& operator-=(const Vector2D&);
  Vector2D& operator*=(const double);
  Vector2D& operator/=(const double);

  double operator[](int) const;

  friend double length(const Vector2D&);  
  friend double lengthsqr(const Vector2D&);  
  friend double vec2angle(const Vector2D&);
  friend Vector2D angle2vec(const double);
  Vector2D& normalize();  
  friend Vector2D unit(const Vector2D&);  
  friend double vedge(const Vector2D&, const Vector2D&);  
  friend Vector2D rotate(const Vector2D&, const double angle);  
  friend Vector2D rotate90(const Vector2D&);  
  friend double dot(const Vector2D&, const Vector2D&);  

private:
  double vector[2];

  Vector2D& copy_vector(const Vector2D&);

  class Range {};
};

#endif __VECTOR2D__
