#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
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

Cookie::Cookie(const Vector2D& c, const double r, const double e)
{
  my_shape = Circle(c, r);
  energy = e;
}

int
Cookie::touch_action(class Robot& robot)
{
  robot.change_energy(energy);
  return( 1 );
}


Robot::Robot(char* filename)
{
  my_shape = Circle();
  velocity = Vector2D(0.0, 0.0);
  robot_filename = *g_string_new(filename);
  robot_name = *g_string_new("");
  robot_dir= *g_string_new(getenv("RTB_ROBOTDIR"));
  extra_air_resistance = 0.0;
  
  int pipe_in[2], pipe_out[2];
  if (pipe (pipe_in))
    throw Error("Couldn't setup pipe_in for robot ", robot_filename.str, "Robot::Robot");
  if (pipe (pipe_out))
    throw Error("Couldn't setup pipe_out for robot ", robot_filename.str, "Robot::Robot");

  if( (robot_pid = fork()) < 0 )
    throw Error("Couldn't fork childprocess for robot ", robot_filename.str, "Robot::Robot");

  if(robot_pid == 0)   // Child process, to be the new robot
    {
      dup2(pipe_out[0],STDIN_FILENO);    // Make pipe_out the standard input for the robot
      close(pipe_out[1]);

      dup2(pipe_in[1],STDOUT_FILENO);   // Make pipe_in the standard output
      close(pipe_in[0]);

      // Make the pipes non-blocking
      int pd_flags;
      if( (pd_flags = fcntl(pipe_out[0], F_GETFL, 0)) == -1 ) 
        throw Error("Couldn't get pd_flags for robot ", robot_filename.str, "Robot::Robot, child");
      pd_flags |= O_NONBLOCK;
      if( fcntl(pipe_out[0], F_SETFL, pd_flags) == -1 ) 
        throw Error("Couldn't change pd_flags for robot ", robot_filename.str, "Robot::Robot, child");

      execl(robot_filename.str, robot_filename.str, NULL);  // Should not return!
      cerr << "Couldn't open robot " << robot_filename.str << "  Error in Robot::Robot" << endl;
      cerr << "Errno: " << errno << endl;
      exit(EXIT_FAILURE);
    }
  else
    {
      close(pipe_out[0]);     // Close input side of pipe_out
      close(pipe_in[1]);      // Close output side of pipe_in  

      // Make the pipes non-blocking
      int pd_flags;
      if( (pd_flags = fcntl(pipe_in[0], F_GETFL, 0)) == -1 ) 
        throw Error("Couldn't get pd_flags for pipe_in in robot ", robot_filename.str, "Robot::Robot, parent");
      pd_flags |= O_NONBLOCK;
      if( fcntl(pipe_in[0], F_SETFL, pd_flags) == -1 ) 
        throw Error("Couldn't change pd_flags for pipe_in in robot ", robot_filename.str, "Robot::Robot, parent");

      send_message(INITIALIZE);
    }
}

Robot::~Robot()
{
  send_message(EXIT_ROBOT);
} 

void
Robot::update_radar_and_cannon()
{
  radar_angle += radar_speed;
  cannon_angle += cannon_speed;
  SolidObject* closest_shape;
  double dist = the_arena->get_shortest_distance(my_shape.get_center(), velocity, 0.0, closest_shape);
  send_message(RADAR, dist, closest_shape->get_object_type());
}

void
Robot::move(const double& timestep)
{
  SolidObject* closest_shape;
  double dist = the_arena->get_shortest_distance(my_shape.get_center(), velocity, my_shape.get_radius(), closest_shape);
  if( dist > timestep )
    {
      my_shape.add_to_center(timestep*velocity);
    }
  else
    {
      alive = false; // not entirely complete ...
    }
}

void
Robot::send_message(const message_type msg_type ...)
{
  va_list args;
  va_start(args, msg_type);
  outstream << message[msg_type].msg << " ";
  for(int i=0; i<message[msg_type].number_of_args; i++)
    {
      switch(message[msg_type].arg_type[i])
        {
        case NONE: 
          throw Error("Couldn't send message, no arg_type", "Robot::send_message");
          break;
        case INT:
          outstream << va_arg(args, int) << " ";
          break;
        case DOUBLE:
          outstream << va_arg(args, double) << " ";
          break;
        case STRING:
          outstream << va_arg(args, char*) << " ";
          break;
        default:
          throw Error("Couldn't send message, unknown arg_type", "Robot::send_message");
        }
    }
  outstream << endl;
}

void
Robot::change_energy(const double energy_diff)
{
  energy += energy_diff;
}

Arena::Arena(char* filename)
{
  ifstream file(filename);
  if( !file ) throw Error("Couldn't open file", filename, "Arena::Arena");

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

  Vector2D vec1, vec2;
  Shape* shapep;

  do
    {
      file >> ws >> text;
      
      if( strcmp(text, "circle" ) == 0 )
        {
          file >> ws >> vec1;
          file >> ws >> number1;

          shapep = new Circle(vec1, number1);
          g_list_append(&object_lists[WALL], shapep);
          g_list_append(&solid_objects, shapep);
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
          
          shapep = new Line(vec1, unit(vec2-vec1), length(vec2-vec1), number2);      
          g_list_append(&object_lists[WALL], shapep);
          g_list_append(&solid_objects, shapep);
        }
      //   else if( strcmp(text, "polygon" ) == 0 )
      //     {
      //     }
      //   else if( strcmp(text, "closed_polygon" ) == 0 )
      //     {
      //     }
    } while( text[0] != '\0' );
}

void
Arena::add_to_list(class ArenaObject& obj)
{
  g_list_append(&object_lists[obj.get_object_type()], &obj);
}  

void
Arena::add_to_solid_object_list(class SolidObject& obj)
{
  g_list_append(&solid_objects, &obj);
}
  
void
Arena::remove_from_list(class ArenaObject& obj)
{
  g_list_remove(&object_lists[obj.get_object_type()], &obj);
}  

void
Arena::remove_from_solid_object_list(class SolidObject& obj)
{
  g_list_remove(&solid_objects, &obj);
}  


double
Arena::get_shortest_distance(const Vector2D& pos, const Vector2D& dir, const double size, SolidObject* closest_shape)
{
  double dist = infinity;
  double d;
  SolidObject* objp;
  closest_shape = NULL;
  GList* gl;

  for(gl = g_list_first(&solid_objects); gl->next != NULL; gl = g_list_next(gl))
    {
      objp = (SolidObject*)gl->data;
      objp->get_distance(pos, dir, size);
      if( d < dist)
        {
          closest_shape = objp;
          dist = d;
        }
    }

  return dist;
}

void
Arena::update_robots()
{
  GList* gl;
  Robot* robotp;

  for(gl = g_list_first(&object_lists[ROBOT]); gl->next != NULL; gl = g_list_next(gl))
    {
      robotp = (Robot*)gl->data;
      if( robotp->is_alive() )
        {
          robotp->move(timestep);
          if( !robotp->is_alive() ) remove_from_solid_object_list(*(SolidObject*)robotp);
          
          robotp->update_radar_and_cannon();  
        }      
    }
}
  
Error::Error(char* strp, char* funcp) 
{ 
  error_stringp = g_string_new(strp); 
  error_string2p = g_string_new(""); 
  c = '\0'; 
  function_stringp = g_string_new(funcp); 
}

Error::Error(char* strp, char* str2p, char* funcp) 
{ 
  error_stringp = g_string_new(strp); 
  error_string2p = g_string_new(str2p); 
  c = '\0'; 
  function_stringp = g_string_new(funcp); 
}

Error::Error(char* strp, char ch, char* funcp) 
{ 
  error_stringp = g_string_new(strp); 
  error_string2p = g_string_new(""); 
  c = ch; 
  function_stringp = g_string_new(funcp);
}

void 
Error::print_message() 
{ 
  if ( c != '\0' )
    cerr << "Error in " << function_stringp << ": " << error_stringp 
         << " " << c << endl; 
  else if (error_string2p != NULL) 
    cerr << "Error in " << function_stringp << ": " << error_stringp 
         << " " << error_string2p << endl;
  else
    cerr << "Error in " << function_stringp << ": " << error_stringp 
         << endl;         
}

