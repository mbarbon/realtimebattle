#include "Arena.h"

Line::Line(const Vector2D& sp, const Vector2D& d, const double len, const double th)
{
  start_point = sp;
  direction = d;
  length = len;
  thickness = th;
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

Arena::Arena(const char* filename)
{
  ifstream file(filename);
  if( !file ) throw FileError();

  parse_file(file);
}

Arena::~Arena()
{
  //  if( filep ) delete filep;
}

void
Arena::parse_file(istream& file)
{
  char text[20];
  double number1, number2;
  int max_shapes;
  Vector2D vec1, vec2;

  file >> ws >> text;
  if( strcmp(text, "max_shapes" ) == 0 )
    {
      file >> ws >> max_shapes;
      shapes = new Shape*[max_shapes];
    }

  do
    {
      file >> ws >> text;
      
      if( strcmp(text, "circle" ) == 0 )
        {
          file >> ws >> vec1;
          file >> ws >> number1;
          shapes[number_of_shapes++] = new Circle(vec1, number1);      
        }
      //  else if( strcmp(text, "outer_circle" ) == 0 )
      //    {
      //    }
      //   else if( strcmp(text, "arc" ) == 0 )
      //     {
      //     }
      else if( strcmp(text, "line" ) == 0 )
        {
          file >> ws >> vec1;      // start_point
          file >> ws >> vec2;      // end_point
          file >> ws >> number2;   // thickness
          
          shapes[number_of_shapes++] = new Line(vec1, unit(vec2-vec1), length(vec2-vec1), number2);      
        }
      //   else if( strcmp(text, "polygon" ) == 0 )
      //     {
      //     }
      //   else if( strcmp(text, "closed_polygon" ) == 0 )
      //     {
      //     }
    } while( text[0] != '\0' );
}

double
Arena::get_shortest_distance(const Vector2D& pos, const Vector2D& dir, const double size, int closest_shape)
{
  closest_shape = -1;
  double dist = infinity;
  double d;

  for(int i=0; i<number_of_shapes; i++)
    {
      d = shapes[i]->get_distance(pos, dir, size);
      if( d < dist)
        {
          closest_shape = i;
          dist = d;
        }
    }

  return dist;
}



