#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <strstream.h>
#include "Arena.h"

Robot::Robot(char* filename, Arena* ap)
{
  velocity = Vector2D(0.0, 0.0);
  acceleration = 0.0;
  robot_filename = *g_string_new(filename);
  robot_name = *g_string_new("");
  robot_dir= *g_string_new(getenv("RTB_ROBOTDIR"));
  extra_air_resistance = 0.0;
  process_running = false;
  the_arena = ap;
  instreamp = NULL;
  outstreamp = NULL;
}

Robot::~Robot()
{
  if( is_process_running() ) kill_process_forcefully();
  if( instreamp != NULL ) delete instreamp;
  if( outstreamp != NULL ) delete outstreamp;
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

      // TODO: Restrict resources
      // Restrict different resources
      // E.g. priority, file_access, 

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

      outstreamp = new ofstream(pipe_out[1]);
      instreamp = new ifstream(pipe_in[0]);

      send_message(INITIALIZE);
    }
  process_running = true;
}

bool
Robot::is_process_running()
{
  // TODO: Check if the process is running

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
  // TODO: Kill process forcefully
  // kill -9
}

void
Robot::live()
{
   alive = true; 
}

void
Robot::die()
{
   alive = false;
   // set points
}


void
Robot::update_radar_and_cannon(const double timestep)
{
  radar_angle += timestep*radar_speed;
  cannon_angle += timestep*cannon_speed;
  robot_angle += timestep*robot_angle_speed;
  object_type closest_shape;
  double dist = the_arena->get_shortest_distance(center, velocity, 0.0, closest_shape);
  send_message(RADAR, dist, closest_shape);
}

void
Robot::set_initial_position_and_direction(const Vector2D& pos, double angle, double size)
{
  center = pos;
  robot_angle = angle;
  radius = size;
  velocity = Vector2D(0.7, 0.0); // Just for testing! 
}

void
Robot::move(const double timestep)
{
  velocity = 0.99*velocity + timestep*acceleration*Vector2D(cos(robot_angle),sin(robot_angle));  // TODO: acceleration update
  object_type closest_shape;
  double dist = the_arena->get_shortest_distance(center, velocity, radius, closest_shape);
  if( dist > timestep )
    {
      center += timestep*velocity;
    }
  else
    {
      alive = false; // TODO: Bouncing et al
    }
}

void
Robot::send_message(const message_to_robot_type msg_type ...)
{
  va_list args;
  va_start(args, msg_type);
  *outstreamp << message_to_robot[msg_type].msg << " ";
  for(int i=0; i<message_to_robot[msg_type].number_of_args; i++)
    {
      switch(message_to_robot[msg_type].arg_type[i])
        {
        case NONE: 
          throw Error("Couldn't send message, no arg_type", "Robot::send_message");
          break;
        case INT:
          *outstreamp << va_arg(args, int) << " ";
          break;
        case DOUBLE:
          *outstreamp << va_arg(args, double) << " ";
          break;
        case STRING:
          *outstreamp << va_arg(args, char*) << " ";
          break;
        default:
          throw Error("Couldn't send message, unknown arg_type", "Robot::send_message");
        }
    }
  *outstreamp << endl;
}

void
Robot::get_messages()
{
  char buffer[81];
  char text[81];
  char msg_name[81];
  message_from_robot_type msg_t;

  *instreamp >> ws;
  instreamp->clear();
  instreamp->peek();
  while( !instreamp->eof() )
    {
      *instreamp >> msg_name;
      msg_t = name2msg_from_robot_type(msg_name);

      switch(msg_t)
        {
        case UNKNOWN_MESSAGE_FROM_ROBOT:
          cout << "Server: Warning sent for message: " << msg_name << endl;
          send_message(WARNING, UNKNOWN_MESSAGE, msg_name);
          instreamp->get(buffer, 80, '\n');
          break;
        case NAME:
          *instreamp >> text;
          g_string_assign(&robot_name, text);
          // TODO: Tell gui to change name
          break;
        case ROTATE: 
          int bits;
          double rot_speed;
          *instreamp >> bits >> rot_speed;
          if( bits & 1 == 1) robot_angle_speed = rot_speed;
          if( bits & 2 == 2) cannon_speed = rot_speed;
          if( bits & 4 == 4) radar_speed = rot_speed;
          break;
        case PRINT:
          instreamp->get(text, 80, '\n');
          cout << "Text read: " << text << endl;
          the_arena->get_the_gui()->print_to_message_output(robot_name.str, text, colour);
          break;
        case SHOOT:
          // pang!
          // TODO: Shooting
          break;
        case ACCELERATE:
          // TODO: acceleration
          break;
        default:
          throw Error("Message_type not implemented, ", msg_name, "Arena::Arena");
        }

      *instreamp >> ws;
      instreamp->clear();
      instreamp->peek();
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
  return UNKNOWN_MESSAGE_FROM_ROBOT;
}

void
Robot::change_energy(const double energy_diff)
{
  energy += energy_diff;
}
