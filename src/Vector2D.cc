#include <math.h>
#include "Vector2D.h"

Vector2D::Vector2D()
{
  for( int i=0; i<2; i++)
    vector[i] = 0.0;
}

Vector2D::Vector2D(const Vector2D& old)
{
  copy_vector(old);
}

Vector2D::Vector2D(double x, double y)
{
  vector[0] = x;
  vector[1] = y;
}

Vector2D&
Vector2D::copy_vector(const Vector2D& other)
{
  for( int i=0; i<2; i++)
    vector[i] = other.vector[i];
  return( *this );
}


Vector2D
operator+(const Vector2D& vec1, const Vector2D& vec2)
{
  Vector2D sum(vec1);
  for( int i=0; i<2; i++)
    sum.vector[i] += vec2.vector[i];
  return( sum );
}

Vector2D
operator-(const Vector2D& vec1, const Vector2D& vec2)
{
  Vector2D diff(vec1);
  for( int i=0; i<2; i++)
    diff.vector[i] -= vec2.vector[i];
  return( diff );
}

Vector2D
operator-(const Vector2D& vec)
{
  Vector2D neg;

  for( int i=0; i<2; i++)
    neg.vector[i] = -vec.vector[i];
  return( neg );
}

double
dot(const Vector2D& vec1, const Vector2D& vec2)
{
  double prod = 0.0;
  for( int i=0; i<2; i++)
    prod += vec1.vector[i] * vec2.vector[i];
  return( prod );
}

double
vedge(const Vector2D& vec1, const Vector2D& vec2)
{
  return( vec1[0]*vec2[1]-vec1[1]*vec2[0] ); 
}

Vector2D
rotate(const Vector2D& vec, const double angle)
{
  return( Vector2D(cos(angle)*vec[0]-sin(angle)*vec[1], 
                   sin(angle)*vec[0]+cos(angle)*vec[1]) ); 
}

Vector2D
rotate90(const Vector2D& vec)
{
  return( Vector2D(-vec[1], vec[0]) );
}

Vector2D
operator*(const Vector2D& vec, const double factor)
{
  Vector2D prod(vec);

  for( int i=0; i<2; i++)
    prod.vector[i] *= factor;
  return( prod );
}

Vector2D
operator*(const double factor, const Vector2D& vec)
{
  return( vec*factor );
}

Vector2D
operator/(const Vector2D& vec, const double denom)
{
  Vector2D quotient(vec);

  for( int i=0; i<2; i++)
    quotient.vector[i] /= denom;
  return( quotient );
}

Vector2D&
Vector2D::operator=(const Vector2D& other)
{
  copy_vector(other);
  return( *this );
}

int
operator==(const Vector2D& vec1, const Vector2D& vec2)
{
  return( vec1.vector[0] == vec2.vector[0] && 
          vec1.vector[1] == vec2.vector[1] ); 
}

ostream&
operator<<(ostream& os, const Vector2D& vec)
{
  os.setf(ios::fixed , ios::floatfield);
  os.precision(6);
  os << "( " << vec.vector[0] << " , " << vec.vector[1] << " )";
  return( os );
}

istream&
operator>>(istream& is, Vector2D& vec)
{
  is >> vec.vector[0] >> vec.vector[1];
  return( is );
}

double
Vector2D::operator[](int index) const
{
  if( index >= 0 && index <= 1 ) return( vector[index] );
  throw Range();
}

Vector2D&
Vector2D::operator+=(const Vector2D& other)
{
  for( int i=0; i<2; i++)
    vector[i] += other.vector[i];  
  return( *this );
}

Vector2D&
Vector2D::operator-=(const Vector2D& other)
{
  for( int i=0; i<2; i++)
    vector[i] -= other.vector[i];  
  return( *this );
}

Vector2D&
Vector2D::operator*=(const double factor)
{
  for( int i=0; i<2; i++)
    vector[i] *= factor;
  return( *this );
}

Vector2D&
Vector2D::operator/=(const double denum)
{
  for( int i=0; i<2; i++)
    vector[i] /= denum;  
  return( *this );
}

double
lengthsqr(const Vector2D& vec)
{
  return( vec.vector[0]*vec.vector[0] + 
               vec.vector[1]*vec.vector[1] );
}

double
length(const Vector2D& vec)
{
  return( sqrt(vec.vector[0]*vec.vector[0] + 
               vec.vector[1]*vec.vector[1]) );
}

double
vec2angle(const Vector2D& vec)
{
  return atan2(vec.vector[0], vec.vector[1]);
}

Vector2D
angle2vec(const double angle)
{
  return Vector2D(cos(angle),sin(angle));
}


Vector2D&
Vector2D::normalize()
{
  *this /= length(*this);
  return( *this ); 
}

Vector2D
unit(const Vector2D& vec)
{
  Vector2D uvec(vec);
  uvec /= length(vec);
  return( uvec ); 
}
