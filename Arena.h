#include <fstream.h>
#include "Vector2D.h"

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

static const double infinity = 1.0e10;

// ---------------------  Shape ---------------------

class Shape 
{
public:
  virtual double get_distance(const Vector2D& pos, const Vector2D& dir, const double size) = 0;
  //virtual void get_args(istream&) = 0;

  class ShapeError {};
};

// ---------------------  Shape : Line ---------------------

class Line : public Shape 
{
public:
  //Line();
  Line(const Vector2D& sp, const Vector2D& d, const double len, const double th);
  ~Line() {}

  double get_distance(const Vector2D& pos, const Vector2D& dir, const double size);

private:
  Vector2D start_point;
  Vector2D direction;
  double length;
  double thickness;
};

// ---------------------  Shape : Circle ---------------------

class Circle : public Shape 
{
public:
  //Circle();
  Circle(const Vector2D& c, const double r); 
  ~Circle() {}

  double get_distance(const Vector2D& pos, const Vector2D& dir, const double size);
  
private:
  Vector2D center;
  double radius;
  //  double radiussqr;
};

// ---------------------  Arena ---------------------


class Arena 
{
public:
  Arena(const char*);
  ~Arena();

  double get_shortest_distance(const Vector2D& pos, const Vector2D& dir, const double size, int closest_shape);

  class FileError {};

private:
  void parse_file(istream&);

  Shape** shapes;
  int number_of_shapes;
};
