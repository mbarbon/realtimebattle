#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <strstream.h>
#include "Arena.h"

Robot::Robot(char* filename)
{
  velocity = Vector2D(0.0, 0.0);
  robot_filename = *g_string_new(filename);
  robot_name = *g_string_new("");
  robot_dir= *g_string_new(getenv("RTB_ROBOTDIR"));
  extra_air_resistance = 0.0;
  process_running = false;
}

Robot::~Robot()
{
  if( is_process_running() ) kill_process_forcefully();
} 

void
Robot::start_process()
{
  int pipe_in[2], pipe_out[2];
  if (pipe (pipe_in))
    throw Error("Couldn't setup pipe_in for robot ", 
                robot_filename.str, "Robot::Robot");
  if (pipe (pipe_out))
    throw Error("Couldn't setup pipe_out for robot ", 
                robot_filename.str, "Robot::Robot");

  if( (pid = fork()) < 0 )
    throw Error("Couldn't fork childprocess for robot ", 
                robot_filename.str, "Robot::Robot");

  if(pid == 0)   // Child process, to be the new robot
    {
      // Make pipe_out the standard input for the robot
      dup2(pipe_out[0],STDIN_FILENO);
      close(pipe_out[1]);

      // Make pipe_in the standard output
      dup2(pipe_in[1],STDOUT_FILENO);   
      close(pipe_in[0]);

      // Make the pipes non-blocking
      int pd_flags;
      if( (pd_flags = fcntl(pipe_out[0], F_GETFL, 0)) == -1 ) 
        throw Error("Couldn't get pd_flags for robot ", 
                    robot_filename.str, "Robot::Robot, child");
      pd_flags |= O_NONBLOCK;
      if( fcntl(pipe_out[0], F_SETFL, pd_flags) == -1 ) 
        throw Error("Couldn't change pd_flags for robot ", 
                    robot_filename.str, "Robot::Robot, child");

      // Execute process. Should not return!
      execl(robot_filename.str, robot_filename.str, NULL);

      cerr << "Couldn't open robot " << robot_filename.str 
           << "  Error in Robot::Robot" << endl;
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
        throw Error("Couldn't get pd_flags for pipe_in in robot ", 
                    robot_filename.str, "Robot::Robot, parent");
      pd_flags |= O_NONBLOCK;
      if( fcntl(pipe_in[0], F_SETFL, pd_flags) == -1 ) 
        throw Error("Couldn't change pd_flags for pipe_in in robot ", 
                    robot_filename.str, "Robot::Robot, parent");

      send_message(INITIALIZE);
    }
  process_running = true;
}

bool
Robot::is_process_running()
{
  // Check process!

  return process_running;   // temporary!
}

void
Robot::end_process()
{
  send_message(EXIT_ROBOT);
}

void
Robot::kill_process_forcefully()
{
  // kill -9
}

void
Robot::update_radar_and_cannon(const double timestep)
{
  radar_angle += timestep*radar_speed;
  cannon_angle += timestep*cannon_speed;
  Shape* closest_shape;
  double dist = the_arena->get_shortest_distance(center, velocity, 0.0, closest_shape);
  send_message(RADAR, dist, ((ArenaObject*)closest_shape)->get_object_type());
}

void
Robot::move(const double timestep)
{
  Shape* closest_shape;
  double dist = the_arena->get_shortest_distance(center, velocity, radius, closest_shape);
  if( dist > timestep )
    {
      center += timestep*velocity;
    }
  else
    {
      alive = false; // not entirely complete ...
    }
}

void
Robot::send_message(const message_to_robot_type msg_type ...)
{
  va_list args;
  va_start(args, msg_type);
  outstream << message_to_robot[msg_type].msg << " ";
  for(int i=0; i<message_to_robot[msg_type].number_of_args; i++)
    {
      switch(message_to_robot[msg_type].arg_type[i])
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
Robot::get_messages()
{
  char buffer[81];
  strstream ss;
  char* msg_name;
  message_from_robot_type msg_t;

  instream.getline(buffer, 80, '\n');
  while( buffer[0] != '\0' )
    {
      ss << buffer;
      ss >> msg_name;
      msg_t = name2msg_from_robot_type(msg_name);

      switch(msg_t)
        {
        case UNKNOWN:
          send_message(WARNING, UNKNOWN_MESSAGE, msg_name);
          break;
        case ROTATE: 
          break;
        default:
          throw Error("Message_type not implemented, ", msg_name, "Arena::Arena");
        }
          
      instream.getline(buffer, 80, '\n');
    }
}

message_from_robot_type
Robot::name2msg_from_robot_type(char* msg_name)
{
  for(int i=0; message_from_robot[i].msg[0] != '\0'; i++)
    {
      if( strcmp(message_from_robot[i].msg, msg_name) == 0 )
        return (message_from_robot_type)i;
    }
  return UNKNOWN;
}

void
Robot::change_energy(const double energy_diff)
{
  energy += energy_diff;
}

void
Robot::set_colour( int red, int green, int blue )
{
  colour.red = red;
  colour.green = green;
  colour.blue = blue;
  //  gdk_color_alloc (colormap, &colour);
}
