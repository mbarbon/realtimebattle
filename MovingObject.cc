#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <strstream.h>
#include <signal.h>
#include "Arena.h"

Robot::Robot(char* filename, Arena* ap)
{
  velocity = Vector2D(0.0, 0.0);
  acceleration = 0.0;
  robot_filename = *g_string_new(filename);
  robot_name = *g_string_new("");
  robot_dir= *g_string_new(getenv("RTB_ROBOTDIR"));
  statistics = g_list_alloc();
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
  send_signal();
}

void
Robot::send_signal()
{
  kill(pid, SIGUSR1);
}

void
Robot::kill_process_forcefully()
{
  kill(pid, SIGKILL);
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
   send_message(DEAD);
   position_this_game = the_arena->get_robots_left();
   the_arena->get_the_gui()->draw_circle(last_drawn_center,last_drawn_radius,the_arena->get_the_gui()->get_background_color(),true);
}

void
Robot::set_stats(int robots_killed_same_time)
{
  stat_t* sp;
  position_this_game += robots_killed_same_time - 1;
  display_place();
  double points_this_game = the_arena->get_robots_per_game() - position_this_game + position_this_game * 0.5;
  stat_t* statp = new stat_t
    (
     the_arena->get_sequence_nr(),
     the_arena->get_games_per_sequence() - the_arena->get_sequences_remaining(),
     points_this_game,
     the_arena->get_total_time(),
     (sp = (stat_t*)g_list_last(statistics)->data) != NULL ? sp->total_points + points : points
     );

  g_list_append(statistics, statp);
}

void
Robot::update_radar_and_cannon(const double timestep)
{
  radar_angle += timestep*radar_speed;
  cannon_angle += timestep*cannon_speed;
  robot_angle += timestep*robot_angle_speed;
  object_type closest_shape;
  void* col_obj;
  double dist = the_arena->get_shortest_distance(center, Vector2D(cos(radar_angle), sin(radar_angle)), 0.0, closest_shape, col_obj);
  send_message(RADAR, dist, closest_shape, radar_angle);
  send_message(INFO, the_arena->get_total_time(), length(velocity), cannon_angle); 
}

void
bounce_on_wall(class Robot& robot, const Shape& wall, const Vector2D& normal)
{
  double e = robot.bounce_coeff * wall.bounce_coeff;
  Vector2D start_vel = robot.velocity;
  robot.velocity -= (1.0 + e) * dot(normal, robot.velocity) * normal;

  double en_diff = 0.5 * robot.mass * lengthsqr(start_vel - robot.velocity);
  double injury = en_diff * 0.5 * (robot.hardness_coeff + wall.hardness_coeff ) * (1.0-e) * (1.0-robot.protection_coeff);
  robot.change_energy(-injury);

  robot.send_message(COLLISION, WALL, -injury);
}

void
bounce_on_robot(Robot& robot1, Robot& robot2, const Vector2D& normal)
{
  double e = robot1.bounce_coeff * robot2.bounce_coeff;  
  Vector2D start_vel1 = robot1.velocity;
  Vector2D start_vel2 = robot2.velocity;
  double mass_quotient = robot1.mass / robot2.mass;
  Vector2D tmp = ((1.0 + e) / ( 1 + mass_quotient )) * dot(robot2.velocity - robot1.velocity, normal) * normal;
  robot1.velocity += tmp;
  robot2.velocity -= mass_quotient * tmp;
  
  double en_diff = 0.5 * robot1.mass * lengthsqr(start_vel1 - robot1.velocity);
  double injury = en_diff * 0.5 * (robot1.hardness_coeff + robot2.hardness_coeff ) * (1.0-e) * (1.0-robot1.protection_coeff);
  robot1.change_energy(-injury);
  robot1.send_message(COLLISION, ROBOT, -injury);

  en_diff = 0.5 * robot2.mass * lengthsqr(start_vel2 - robot2.velocity);
  injury = en_diff * 0.5 * (robot1.hardness_coeff + robot2.hardness_coeff ) * (1.0-e) * (1.0-robot2.protection_coeff);
  robot2.change_energy(-injury);
  robot2.send_message(COLLISION, ROBOT, -injury);
}

void
Robot::set_initial_values(const Vector2D& pos, const double angle)
{
  center = pos;
  robot_angle = angle;
  cannon_angle = angle;
  radar_angle = angle;
  radius = the_arena->get_robot_radius();
  protection_coeff = the_arena->get_robot_protection();
  hardness_coeff = the_arena->get_robot_hardness();
  bounce_coeff = the_arena->get_robot_bounce_coeff();
  mass = the_arena->get_robot_mass();
  energy = the_arena->get_start_energy();
  velocity = Vector2D(0.0, 0.0);
}

void
Robot::change_velocity(const double timestep)
{
  Vector2D dir = Vector2D(cos(robot_angle),sin(robot_angle));
  double gt = the_arena->get_grav_const() * timestep;
  velocity = -velocity* (the_arena->get_air_resistance() * timestep) +
    timestep*acceleration*dir + 
    dot(velocity, dir) * max(0.0, 1.0-gt*the_arena->get_roll_friction()) * dir +
    vedge(dir, velocity) * max(0.0, 1.0-gt*the_arena->get_slide_friction()) * rotate90(dir);
}

void
Robot::move(const double timestep)
{
  move(timestep, 1);
}

void
Robot::move(const double timestep, int iterstep)
{
  object_type closest_shape;
  void* colliding_object;
  double time_to_collision = the_arena->get_shortest_distance(center, velocity, radius, closest_shape, colliding_object);
  if( time_to_collision > timestep )
    {
      center += timestep*velocity;
    }
  else
    {
      double time_remaining = timestep - time_to_collision; 
      center += (time_to_collision - 0.00001)*velocity;

      switch( closest_shape )
        {
        case WALL:
          {
            Vector2D normal = ((Shape*)(WallCircle*)colliding_object)->get_normal(center);
            bounce_on_wall(*this, *(Shape*)(WallCircle*)colliding_object, normal);
          }
          break;
        case ROBOT:
          {
            Vector2D normal = ((Robot*)colliding_object)->get_normal(center);
            bounce_on_robot(*this, *(Robot*)colliding_object, normal);
            time_remaining = 0.0;
          }
          break;
        case SHOT:
          {
            double en =  -((Shot*)colliding_object)->get_energy();
            change_energy( en );
            send_message(COLLISION, SHOT, en);
            Shot* shotp =(Shot*)colliding_object;
            shotp->die();
            g_list_remove((the_arena->get_object_lists())[SHOT], shotp);
            delete shotp;
          }
          break;
        case COOKIE:
          break;
        case MINE:
          break;
        default:
          throw Error("Collided with unknown object", "Robot::move");
          break;
        }
      if( iterstep > 20 ) throw Error("Too many bounces, must be a bug!", "Robot::move");
      if( alive && time_remaining > 0.0 ) move( time_remaining, iterstep + 1 );
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
          if( the_arena->get_state() != Arena::STARTING_ROBOTS ) 
            {
              cout << "Server: Warning sent for message: " << msg_name << "     State: " << the_arena->get_state() << endl;
              send_message(WARNING, MESSAGE_SENT_IN_ILLEGAL_STATE, msg_name);
              instreamp->get(buffer, 80, '\n');
              break;
            }
          *instreamp >> text;
          g_string_assign(&robot_name, text);
          // TODO: Tell gui to change name
          break;
        case COLOUR:
          if( the_arena->get_state() != Arena::STARTING_ROBOTS ) break;
          {
            bool got_colour=false;
            long colour,robot_colour[2];
            GdkColor tmp_colour;

            *instreamp >> hex >> robot_colour[0] >> robot_colour[1] >> dec;
            GList* all_robots_in_sequence;
            GList* gl;

            all_robots_in_sequence = the_arena->get_all_robots_in_sequence();
            while( !got_colour )
              {
                bool not_colour[2] = { false, false };
                for(gl = g_list_next(all_robots_in_sequence); gl != NULL; gl = g_list_next(gl))
                  {
                    if((Robot *)gl->data != this)
                      {
                        for(int i=0; i<2; i++)
                          {
                            bool red=false, green=false, blue=false;

                            tmp_colour = ((Robot*)gl->data)->get_colour();
                            if((tmp_colour.red + 0x5000 >= ((robot_colour[i] & 0xff0000) >> 16 ) * 0x101) &&
                               (tmp_colour.red - 0x5000 <= ((robot_colour[i] & 0xff0000) >> 16 ) * 0x101))
                              red=true;
                            if((tmp_colour.green + 0x5000 >= ((robot_colour[i] & 0x00ff00) >> 8 ) * 0x101) &&
                               (tmp_colour.green - 0x5000 <= ((robot_colour[i] & 0x00ff00) >> 8 ) * 0x101))
                              green=true;
                            if((tmp_colour.blue + 0x5000 >= (robot_colour[i] & 0x0000ff) * 0x101) &&
                               (tmp_colour.blue - 0x5000 <= (robot_colour[i] & 0x0000ff) * 0x101))
                              blue=true;
                            if(red && green && blue)
                              not_colour[i] = true;
                          }
                      }
                  }
                if(!not_colour[0])
                  {
                    colour = robot_colour[0];
                    got_colour = true;
                  }
                else if(!not_colour[1])
                  {
                    colour = robot_colour[1];
                    got_colour = true;
                  }
                else
                  {
                    robot_colour[0] = rand();
                    robot_colour[1] = rand();
                    got_colour = false;
                  }
              }

            set_colour( colour );
          }
          break;
        case ROTATE: 
          int bits;
          double rot_speed;
          *instreamp >> bits >> rot_speed;
          if( (bits & 1) == 1) robot_angle_speed = rot_speed;
          if( (bits & 2) == 2) cannon_speed = rot_speed;
          if( (bits & 4) == 4) radar_speed = rot_speed;
          break;
        case PRINT:
          instreamp->get(text, 80, '\n');
          the_arena->get_the_gui()->print_to_message_output(robot_name.str, text, colour);
          break;
        case SHOOT:
          if( the_arena->get_state() != Arena::GAME_IN_PROGRESS ) break;
          {
            double en;
            *instreamp >> en;
            Vector2D dir = Vector2D(cos(cannon_angle),sin(cannon_angle));
            double shot_radius = the_arena->get_shot_radius();
            Vector2D shot_center = center + (radius+1.5*shot_radius)*dir;
            if( the_arena->space_available( shot_center, shot_radius + 0.00001 ) )
              {
                Shot* shotp = new Shot( shot_center, shot_radius,
                                        velocity + dir * the_arena->get_shot_speed(),
                                        the_arena, en );
                change_energy(-en * the_arena->get_shooting_penalty() );
                g_list_append((the_arena->get_object_lists())[SHOT], shotp);
              }
            else
              {  // TODO: shooting: direct hit if too close
              }

          }
          break;
        case ACCELERATE:
          double acc;
          *instreamp >> acc;
          if( acc < the_arena->get_min_acceleration() || acc > the_arena->get_max_acceleration() )
            send_message(WARNING, VARIABLE_OUT_OF_RANGE, msg_name);            
          else
            acceleration = acc;
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
  display_energy();
  if( energy <= 0.0 ) die();
}

void
Robot::display_energy()
{
  strstream ss;
  char str_energy[25];

  ss << (int)energy;
  ss >> str_energy;
  gtk_entry_set_text (GTK_ENTRY (widget_energy), str_energy);
}

void
Robot::display_place()
{
  strstream ss;
  char str_place[25];

  ss << position_this_game;
  ss >> str_place;

  cerr << "display_place(): String: " << str_place << endl;
  gtk_entry_set_text (GTK_ENTRY (widget_place), str_place);
}

void
Robot::set_gtk_widgets( GtkWidget * en, GtkWidget * pl, GtkWidget * la, GtkWidget * sc )
{
  widget_energy = en;
  widget_place = pl;
  widget_last = la;
  widget_score = sc;
}

Shot::Shot(const Vector2D& c, const double r, 
           const Vector2D& vel, Arena* ap, const double en ) 
  : ArenaObject(ap), MovingObject(vel), Circle(c, r)
{
  alive = true;
  energy = en;
}

void
Shot::move(const double timestep)
{
  object_type closest_shape;
  void* colliding_object;
  double time_to_collision = the_arena->get_shortest_distance(center, velocity, radius, closest_shape, colliding_object);
  if( time_to_collision > timestep )
    {
      center += timestep*velocity;
    }
  else
    {
      switch( closest_shape )
        {
        case WALL:
          // TODO: shot explode on wall ?
          die();
          break;
        case ROBOT:
          ((Robot*)colliding_object)->change_energy(-energy);
          ((Robot*)colliding_object)->send_message(COLLISION, SHOT, -energy);
          die();
          break;
        case SHOT:
          // TODO: shot explode on shot ?
          ((Shot*)colliding_object)->die();
          die();
          break;
        case COOKIE:
          break;
        case MINE:
          break;
        default:
          throw Error("Collided with unknown object", "Robot::move");
          break;
        }
    }
}

void
Shot::die()
{
   alive = false;
   the_arena->get_the_gui()->draw_circle(last_drawn_center,last_drawn_radius,the_arena->get_the_gui()->get_background_color(),true);
}
