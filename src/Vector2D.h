

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