#include <fcntl.h>
//#include <errno.h>
#include <unistd.h>
//#include <strstream.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/stat.h>

#include "MovingObject.h"
#include "Arena.h"
#include "Extras.h"
#include "Error.h"
#include "Options.h"
#include "messagetypes.h"
#include "gui.h"

Robot::Robot(const String& filename)
{
  velocity = Vector2D(0.0, 0.0);
  acceleration = 0.0;
  robot_filename = filename;
  plain_robot_name = "";
  robot_name = "";
  robot_name_uniqueness_number = 0;
  //  robot_dir= getenv("RTB_ROBOTDIR");
  statistics = g_list_alloc();
  extra_air_resistance = 0.0;
  process_running = false;
  send_usr_signal = false;
  alive = false;
  points = 0;

  instreamp = NULL;
  outstreamp = NULL;
  pipes[0] = -1;  
  pipes[1] = -1;
  pid = -1;
  last_drawn_robot_center = Vector2D(infinity,infinity);
}

Robot::~Robot()
{
  if( is_process_running() ) kill_process_forcefully();
  delete_pipes();

  stat_t* statp;
  for(GList* gl=g_list_next(statistics); gl != NULL; )
    {
      statp = (stat_t*)(gl->data);
      delete statp;
      gl=g_list_next(gl);
      g_list_remove(statistics, statp);
    }
  g_list_free(statistics);
} 

void
Robot::start_process()
{
  int pipe_in[2], pipe_out[2];
  if (pipe (pipe_in))
    throw Error("Couldn't setup pipe_in for robot ", 
                robot_filename, "Robot::Robot");
  if (pipe (pipe_out))
    throw Error("Couldn't setup pipe_out for robot ", 
                robot_filename, "Robot::Robot");

  if( (pid = fork()) < 0 )
    throw Error("Couldn't fork childprocess for robot ", 
                robot_filename, "Robot::Robot");

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
                    robot_filename, "Robot::Robot, child");
      pd_flags |= O_NONBLOCK;
      if( fcntl(pipe_out[0], F_SETFL, pd_flags) == -1 ) 
        throw Error("Couldn't change pd_flags for robot ", 
                    robot_filename, "Robot::Robot, child");
      
      // Check file attributes

      struct stat filestat;
      if( 0 != stat( robot_filename.chars(), &filestat ) ) 
        throw Error("Couldn't get stats for robot ", robot_filename, "Robot::Robot, child");
      if( !S_ISREG( filestat.st_mode) )
        throw Error("Robot file isn't regular, error for robot ", robot_filename, "Robot::Robot, child");
      if( !(filestat.st_mode & S_IXOTH) )
        throw Error("Robot file isn't executable for user, error for robot ", 
                    robot_filename, "Robot::Robot, child");
      if( (filestat.st_mode & S_ISUID) )
        throw Error("Set user ID is not allowed, error for robot ", 
                    robot_filename, "Robot::Robot, child");

      // Lower priority by one
     
      int old;
      if( (old = getpriority (PRIO_PROCESS, 0)) == -1 )
        throw Error("Couldn't get priority for robot ", robot_filename, "Robot::Robot, child");
      if( setpriority (PRIO_PROCESS, 0, old + 1) == -1)
        throw Error("Couldn't set priority for robot ", robot_filename, "Robot::Robot, child");

      // Deny file access

      struct rlimit res_limit;

      if( getrlimit( RLIMIT_NOFILE, &res_limit ) == -1 )
        throw Error("Couldn't get file limits for robot ", robot_filename, "Robot::Robot, child");

      // TODO: Deny file access

      //res_limit.rlim_cur = ?;
      if( setrlimit( RLIMIT_NOFILE, &res_limit ) == -1 )
        throw Error("Couldn't limit file access for robot ", robot_filename, "Robot::Robot, child");

      // Forbid creation of child processes

      if( getrlimit( RLIMIT_NPROC, &res_limit ) == -1 )
        throw Error("Couldn't get proc limits for robot ", robot_filename, "Robot::Robot, child");

      res_limit.rlim_cur = 0;
      if( setrlimit( RLIMIT_NPROC, &res_limit ) == -1 )
        throw Error("Couldn't limit child processes for robot ", robot_filename, "Robot::Robot, child");

      // Execute process. Should not return!
      if( execl(robot_filename.chars(), robot_filename.chars(), NULL) == -1 )
        throw Error("Couldn't open robot ", robot_filename, "Robot::Robot, child");

      throw Error("Robot didn't execute, SHOULD NEVER HAPPEN!, error for ", 
                  robot_filename, "Robot::Robot, child");
    }
  else
    {
      close(pipe_out[0]);     // Close input side of pipe_out
      close(pipe_in[1]);      // Close output side of pipe_in  
      
      pipes[0] = pipe_out[1];
      pipes[1] = pipe_out[0];

      // Make the pipes non-blocking
      int pd_flags;
      if( (pd_flags = fcntl(pipe_in[0], F_GETFL, 0)) == -1 ) 
        throw Error("Couldn't get pd_flags for pipe_in in robot ", 
                    robot_filename, "Robot::Robot, parent");
      pd_flags |= O_NONBLOCK;
      if( fcntl(pipe_in[0], F_SETFL, pd_flags) == -1 ) 
        throw Error("Couldn't change pd_flags for pipe_in in robot ", 
                    robot_filename, "Robot::Robot, parent");

      outstreamp = new ofstream(pipe_out[1]);
      instreamp = new ifstream(pipe_in[0]);

      if( g_list_next(statistics) == NULL )
        {
          send_message(INITIALIZE, 1);        // first sequence !
        }
      else
        {
          send_message(INITIALIZE, 0);        // not first sequence !
          send_message(YOUR_NAME, robot_name.chars());
          send_message(YOUR_COLOUR, gdk2hex_colour(colour));
        } 
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
  send_message(SAVE_DATA);
  send_signal();
}

void
Robot::send_signal()
{
  if( send_usr_signal )
    kill(pid, SIGUSR1);
}

void
Robot::kill_process_forcefully()
{
  kill(pid, SIGKILL);
  process_running = false;
}

void
Robot::delete_pipes()
{
  if( instreamp != NULL ) delete instreamp;
  instreamp = NULL;
  if( outstreamp != NULL ) delete outstreamp;
  outstreamp = NULL;
  if( pipes[0] != -1 ) 
    {
      close(pipes[0]);
      pipes[0] = -1;
    }
  if( pipes[1] != -1 ) 
    {
      close(pipes[1]);
      pipes[1] = -1;
    }
  
}

void
Robot::live()
{
   alive = true; 
}

void
Robot::die()
{
  if( alive )
    {
      alive = false;
      position_this_game = -1;
      the_gui.draw_circle(last_drawn_center,last_drawn_radius,*(the_arena.get_background_colour_p()),true);
    }
}

void
Robot::set_stats(int robots_killed_same_time)
{
  int adjust = robots_killed_same_time - 1;
  position_this_game = the_arena.get_robots_left() - adjust; 
  display_place();
  double points_this_game = (double)(the_arena.get_robots_per_game() - position_this_game) - ((double)adjust) * 0.5 + 1.0;

  send_message(DEAD);
  send_signal();

  stat_t* statp = new stat_t
    (
     the_arena.get_sequence_nr(),
     the_arena.get_games_per_sequence() - the_arena.get_games_remaining_in_sequence(),
     position_this_game,
     points_this_game,   
     the_arena.get_total_time(),
     get_total_points() + points_this_game
     );

  g_list_append(statistics, statp);
}

void
Robot::check_name_uniqueness()
{
  Robot* robotp;
  int first_avail = 0;

  robot_name = plain_robot_name;
  
  GList* gl = g_list_next(the_arena.get_all_robots_in_tournament());
  for(; gl != NULL; gl = g_list_next(gl))
    {
      robotp = (Robot*)gl->data;
      if( robotp != this && plain_robot_name == robotp->plain_robot_name )
        {
          if( robotp->robot_name_uniqueness_number == 0 )
            {
              robotp->robot_name_uniqueness_number = 1;
              robotp->robot_name += "(1)";
            }

          first_avail = max( robotp->robot_name_uniqueness_number + 1, first_avail );

          if( robot_name_uniqueness_number == robotp->robot_name_uniqueness_number 
              || robot_name_uniqueness_number == 0 )
            robot_name_uniqueness_number = first_avail;
        }
    }

  if( robot_name_uniqueness_number > 0 )
      robot_name += ('(' + (String)robot_name_uniqueness_number + ')');
}

double
Robot::get_total_points()
{
  stat_t* sp = (stat_t*)g_list_last(statistics)->data;
  return( sp != NULL ? sp->total_points + points : points );
}

int
Robot::get_last_position()
{
  return( the_arena.get_games_per_sequence() -
          the_arena.get_games_remaining_in_sequence() != 1 
          ? ((stat_t*)g_list_last(statistics)->data)->position
          : 0 );
}

void
Robot::update_rotation(rotation_t& angle, const double timestep)
{
  angle.pos += timestep * angle.vel;
  
  if( angle.pos >= angle.right && angle.mode == ROTATE_TO_RIGHT )
    angle.set( angle.pos, 0.0, -infinity, infinity, NORMAL_ROT);

  if( angle.pos >= angle.right && angle.mode == SWEEP_RIGHT )
    angle.set( angle.pos, -angle.vel, angle.left, angle.right, SWEEP_LEFT);
  
  if( angle.pos <= angle.left && angle.mode == ROTATE_TO_LEFT )
    angle.set( angle.pos, 0.0, -infinity, infinity, NORMAL_ROT);

  if( angle.pos <= angle.left && angle.mode == SWEEP_LEFT )
    angle.set( angle.pos, -angle.vel, angle.left, angle.right, SWEEP_RIGHT);
}

void
Robot::update_radar_and_cannon(const double timestep)
{
  update_rotation(robot_angle, timestep);
  update_rotation(cannon_angle, timestep);
  update_rotation(radar_angle, timestep);
  shot_energy = min( the_opts.get_d(OPTION_SHOT_MAX_ENERGY), 
                     shot_energy+timestep*the_opts.get_d(OPTION_SHOT_ENERGY_INCREASE_SPEED) );
  object_type closest_shape;
  void* col_obj;
  double dist = the_arena.
    get_shortest_distance(center, angle2vec(radar_angle.pos+robot_angle.pos),
                          0.0, closest_shape, col_obj);
  send_message(RADAR, dist, closest_shape, radar_angle.pos);
  send_message(INFO, the_arena.get_total_time(), length(velocity), cannon_angle.pos); 
}

void
bounce_on_wall(class Robot& robot, const class Wall& wall, const Vector2D& normal)
{
  double h, p, b;
  
  if( -dot(normal, angle2vec(robot.robot_angle.pos)) > the_opts.get_d(OPTION_ROBOT_COS_FRONTSIZE) )
    {
      h = the_opts.get_d(OPTION_ROBOT_FRONT_HARDNESS);
      b = the_opts.get_d(OPTION_ROBOT_FRONT_BOUNCE_COEFF);
      p = the_opts.get_d(OPTION_ROBOT_FRONT_PROTECTION);
    }
  else
    {
      h = the_opts.get_d(OPTION_ROBOT_HARDNESS);
      b = the_opts.get_d(OPTION_ROBOT_BOUNCE_COEFF);
      p = the_opts.get_d(OPTION_ROBOT_PROTECTION);
    }  

  double e = b * wall.bounce_coeff;
  Vector2D start_vel = robot.velocity;
  robot.velocity -= (1.0 + e) * dot(normal, robot.velocity) * normal;

  double en_diff = 0.5 * the_opts.get_d(OPTION_ROBOT_MASS) * lengthsqr(start_vel - robot.velocity);
  double injury = en_diff * 0.5 * (h + wall.hardness_coeff ) * (1.0-e) * (1.0-p);
  robot.change_energy(-injury);

  robot.send_message(COLLISION, WALL, -injury, -vec2angle(normal)-robot.robot_angle.pos);
}

void
bounce_on_robot(Robot& robot1, Robot& robot2, const Vector2D& normal)
{
  double h1, h2, p1, p2, b1, b2;
  Vector2D dir1_2 = unit(robot2.center - robot1.center);
  
  if( dot(dir1_2, angle2vec(robot1.robot_angle.pos)) > the_opts.get_d(OPTION_ROBOT_COS_FRONTSIZE) )
    {
      h1 = the_opts.get_d(OPTION_ROBOT_FRONT_HARDNESS);
      b1 = the_opts.get_d(OPTION_ROBOT_FRONT_BOUNCE_COEFF);
      p1 = the_opts.get_d(OPTION_ROBOT_FRONT_PROTECTION);
    }
  else
    {
      h1 = the_opts.get_d(OPTION_ROBOT_HARDNESS);
      b1 = the_opts.get_d(OPTION_ROBOT_BOUNCE_COEFF);
      p1 = the_opts.get_d(OPTION_ROBOT_PROTECTION);
    }

  if( -dot(dir1_2, angle2vec(robot2.robot_angle.pos)) > the_opts.get_d(OPTION_ROBOT_COS_FRONTSIZE) )
    {
      h2 = the_opts.get_d(OPTION_ROBOT_FRONT_HARDNESS);
      b2 = the_opts.get_d(OPTION_ROBOT_FRONT_BOUNCE_COEFF);
      p2 = the_opts.get_d(OPTION_ROBOT_FRONT_PROTECTION);
    }
  else
    {
      h2 = the_opts.get_d(OPTION_ROBOT_HARDNESS);
      b2 = the_opts.get_d(OPTION_ROBOT_BOUNCE_COEFF);
      p2 = the_opts.get_d(OPTION_ROBOT_PROTECTION);
    }

  double e = b1*b2;
  Vector2D start_vel1 = robot1.velocity;
  Vector2D start_vel2 = robot2.velocity;
  double mass = the_opts.get_d(OPTION_ROBOT_MASS);
  Vector2D tmp = ((1.0 + e) / 2.0) * dot(robot2.velocity - robot1.velocity, normal) * normal;
  robot1.velocity += tmp;
  robot2.velocity -= tmp;

  double an = vec2angle(normal);
  double en_diff = 0.5 * mass * lengthsqr(start_vel1 - robot1.velocity);
  double injury = en_diff * 0.5 * (h1 + h2) * (1.0-e) * (1.0-p1);
  robot1.change_energy(-injury);
  robot1.send_message(COLLISION, ROBOT, -injury, an-robot1.robot_angle.pos);

  en_diff = 0.5 * mass * lengthsqr(start_vel2 - robot2.velocity);
  injury = en_diff * 0.5 * (h1 + h2) * (1.0-e) * (1.0-p2);
  robot2.change_energy(-injury);
  robot2.send_message(COLLISION, ROBOT, -injury, -an-robot2.robot_angle.pos);
}

void
Robot::set_initial_values(const Vector2D& pos, const double angle)
{
  center = pos;
  robot_angle.set (angle, 0.0, -infinity, infinity, NORMAL_ROT);
  cannon_angle.set(0.0,   0.0, -infinity, infinity, NORMAL_ROT);
  radar_angle.set (0.0,   0.0, -infinity, infinity, NORMAL_ROT);
  shot_energy = 0.0;
  radius = the_opts.get_d(OPTION_ROBOT_RADIUS);
  energy = the_opts.get_d(OPTION_ROBOT_START_ENERGY);
  velocity = Vector2D(0.0, 0.0);
  position_this_game = 0;
  break_percent = 0.0;
  acceleration = 0.0;
}

void
Robot::change_velocity(const double timestep)
{
  Vector2D dir = angle2vec(robot_angle.pos);
  double gt = the_opts.get_d(OPTION_GRAV_CONST) * timestep;
  double fric = the_opts.get_d(OPTION_ROLL_FRICTION) * (1.0 - break_percent) + 
    the_opts.get_d(OPTION_SLIDE_FRICTION) * break_percent;
  velocity = -velocity* min(the_opts.get_d(OPTION_AIR_RESISTANCE) * timestep, 0.5) +
    timestep*acceleration*dir + 
    dot(velocity, dir) * max(0.0, 1.0-gt*fric) * dir +
    vedge(dir, velocity) * max(0.0, 1.0-gt*the_opts.get_d(OPTION_SLIDE_FRICTION)) * rotate90(dir);
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
  double time_to_collision = the_arena.get_shortest_distance(center, velocity, radius, closest_shape, colliding_object);
  if( time_to_collision > timestep )
    {
      center += timestep*velocity;
    }
  else
    {
      double time_remaining = timestep - time_to_collision; 
      center += time_to_collision*velocity;
      Vector2D new_center = center - min(eps, time_to_collision)*velocity;
      switch( closest_shape )
        {
        case WALL:
          {
            Vector2D normal = ((Shape*)(WallCircle*)colliding_object)->get_normal(center);
            bounce_on_wall(*this, *(Wall*)(WallCircle*)colliding_object, normal);
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
            Shot* shotp =(Shot*)colliding_object;
            double en =  -shotp->get_energy();
            change_energy( en );
            send_message(COLLISION, SHOT, en, vec2angle(shotp->get_center()-center)-robot_angle.pos);
            shotp->die();
            g_list_remove((the_arena.get_object_lists())[SHOT], shotp);
            delete shotp;
          }
          break;
        case COOKIE:
          {
            Cookie* cookiep =(Cookie*)colliding_object;
            double en =  cookiep->get_energy();
            change_energy( en );
            send_message(COLLISION, COOKIE, en, vec2angle(cookiep->get_center()-center)-robot_angle.pos);
            cookiep->die();
            g_list_remove((the_arena.get_object_lists())[COOKIE], cookiep);
            delete cookiep;
          }
          break;
        case MINE:
          {
            Mine* minep =(Mine*)colliding_object;
            double en =  -minep->get_energy();
            change_energy( en );
            send_message(COLLISION, MINE, en, vec2angle(minep->get_center()-center)-robot_angle.pos);
            minep->die();
            g_list_remove((the_arena.get_object_lists())[MINE], minep);
            delete minep;
          }
          break;
        default:
          throw Error("Collided with unknown object", "Robot::move");
          break;
        }
      
      center = new_center;
      if( iterstep > 5 ) 
        cout << "Iterstep: " << iterstep << "   time_remaining: " << time_remaining << endl;
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
        case HEX:
          *outstreamp << hex << va_arg(args, int) << " ";
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
        case ROBOT_OPTION:
          {
            int opt_nr, val;
            *instreamp >> opt_nr;
            switch(opt_nr)
              {
              case SEND_SIGNAL:
                *instreamp >> val;
                send_usr_signal = (val == true);
                send_signal();
                break;
              default:
                send_message(WARNING, UNKNOWN_OPTION, msg_name);
                break;
              }
              }
          break;
        case NAME:
          if( the_arena.get_state() != Arena::STARTING_ROBOTS ) 
            {
              cout << "Server: Warning sent for message: " << msg_name << "     State: " << the_arena.get_state() << endl;
              send_message(WARNING, MESSAGE_SENT_IN_ILLEGAL_STATE, msg_name);
              instreamp->get(buffer, 80, '\n');
              break;
            }
          *instreamp >> text;
          plain_robot_name = text;
          check_name_uniqueness();
          break;
        case COLOUR:
          if( the_arena.get_state() != Arena::STARTING_ROBOTS ) break;
          {
            long home_colour, away_colour;

            *instreamp >> hex >> home_colour >> away_colour >> dec;
  
            // TODO: check if colour is already allocated! 
            colour = make_gdk_colour(the_arena.find_free_colour(home_colour, away_colour, this));
          }
          break;
        case ROTATE:
          { 
            int bits;
            double rot_speed;
            *instreamp >> bits >> rot_speed;
            
            if( bits & 1 ) 
              robot_angle.set( robot_angle.pos,  
                               max(min(rot_speed, the_opts.get_d(OPTION_ROBOT_MAX_ROTATE)),
                                   -the_opts.get_d(OPTION_ROBOT_MAX_ROTATE)),  // between -max_rot and +max_rot                  
                               -infinity, infinity, NORMAL_ROT );
            if( bits & 2 ) 
              cannon_angle.set( cannon_angle.pos,  
                                max(min(rot_speed, the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE)),
                                    -the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE)),  // between -max_rot and +max_rot                  
                                -infinity, infinity, NORMAL_ROT );
            if(bits & 4 )
              radar_angle.set( radar_angle.pos,  
                                max(min(rot_speed, the_opts.get_d(OPTION_ROBOT_RADAR_MAX_ROTATE)),
                                    -the_opts.get_d(OPTION_ROBOT_RADAR_MAX_ROTATE)),  // between -max_rot and +max_rot                  
                                -infinity, infinity, NORMAL_ROT );
          }
          break;
        case ROTATE_TO:
          {
            int bits;
            double rot_speed, rot_end_angle, rot_amount;
            *instreamp >> bits >> rot_speed >> rot_end_angle;
            if( bits & 2 )
              {
                cannon_angle.pos -= rint( (cannon_angle.pos-rot_end_angle) / (2.0*M_PI) ) * 2.0 * M_PI;
                rot_amount = rot_end_angle - cannon_angle.pos;
                if( rot_amount > 0 )
                  cannon_angle.set( cannon_angle.pos, 
                                     min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE) ),
                                     -infinity, cannon_angle.pos + rot_amount, ROTATE_TO_RIGHT );
                else
                  cannon_angle.set( cannon_angle.pos, 
                                    -min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE) ),
                                    cannon_angle.pos + rot_amount, infinity, ROTATE_TO_LEFT );
              }
            if( bits & 4 )
              {
                radar_angle.pos -= rint( (radar_angle.pos-rot_end_angle) / (2.0*M_PI) ) * 2.0 * M_PI;
                rot_amount = rot_end_angle - radar_angle.pos;
                if( rot_amount > 0 )
                  radar_angle.set( radar_angle.pos, 
                                   min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_RADAR_MAX_ROTATE) ),
                                   -infinity, radar_angle.pos + rot_amount, ROTATE_TO_RIGHT );
                else
                  radar_angle.set( radar_angle.pos, 
                                   -min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_RADAR_MAX_ROTATE) ),
                                   radar_angle.pos + rot_amount, infinity, ROTATE_TO_LEFT );
              }            
          }
          break;
        case ROTATE_AMOUNT:
          {
            int bits;
            double rot_speed, rot_amount;
            *instreamp >> bits >> rot_speed >> rot_amount;
            if( bits & 1 )
              {
                if( rot_amount > 0 )
                    robot_angle.set( robot_angle.pos, 
                                     min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_MAX_ROTATE) ),
                                     -infinity, robot_angle.pos + rot_amount, ROTATE_TO_RIGHT );
                else
                  robot_angle.set( robot_angle.pos, 
                                   -min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_MAX_ROTATE) ),
                                   robot_angle.pos + rot_amount, infinity, ROTATE_TO_LEFT );
              }
            if( bits & 2 )
              {
                if( rot_amount > 0 )
                  cannon_angle.set( cannon_angle.pos, 
                                    min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE) ),
                                    -infinity, cannon_angle.pos + rot_amount, ROTATE_TO_RIGHT );
                else
                  cannon_angle.set( cannon_angle.pos, 
                                    -min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE) ),
                                    cannon_angle.pos + rot_amount, infinity, ROTATE_TO_LEFT );
              }
            if( bits & 4 )
              {
                if( rot_amount > 0 )
                  radar_angle.set( radar_angle.pos, 
                                   min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_RADAR_MAX_ROTATE) ),
                                   -infinity, radar_angle.pos + rot_amount, ROTATE_TO_RIGHT );
                else
                  radar_angle.set( radar_angle.pos, 
                                   -min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_RADAR_MAX_ROTATE) ),
                                   radar_angle.pos + rot_amount, infinity, ROTATE_TO_LEFT );
              }
          }
        case SWEEP:
          {
            int bits;
            double rot_speed, sweep_left, sweep_right;
            *instreamp >> bits >> rot_speed >> sweep_left >> sweep_right;
            if( bits & 2 )
              {
                cannon_angle.pos -= rint( (cannon_angle.pos - 0.5*(sweep_left+sweep_right)) / (2.0*M_PI) ) * 2.0 * M_PI;
                if( cannon_angle.pos < 0.5*(sweep_left+sweep_right) )
                  cannon_angle.set( cannon_angle.pos, 
                                    min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE) ),
                                    sweep_left, sweep_right, SWEEP_RIGHT );
                else
                  cannon_angle.set( cannon_angle.pos, 
                                    -min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE) ),
                                    sweep_left, sweep_right, SWEEP_LEFT );
              }
            if( bits & 4 )
              {
                radar_angle.pos -= rint( (radar_angle.pos - 0.5*(sweep_left+sweep_right)) / (2.0*M_PI) ) * 2.0 * M_PI;
                if( radar_angle.pos < 0.5*(sweep_left+sweep_right) )
                  radar_angle.set( radar_angle.pos, 
                                   min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_RADAR_MAX_ROTATE) ),
                                   sweep_left, sweep_right, SWEEP_RIGHT );
                else
                  radar_angle.set( radar_angle.pos, 
                                   -min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_RADAR_MAX_ROTATE) ),
                                   sweep_left, sweep_right, SWEEP_LEFT );
              }
          }
          break;
        case PRINT:
          instreamp->get(text, 80, '\n');
          the_gui.print_to_message_output(robot_name, text, colour);
          break;
        case SHOOT:
          if( the_arena.get_state() != Arena::GAME_IN_PROGRESS ) break;
          {
            double en;
            *instreamp >> en;
            en = min(en, shot_energy);
            if( en < the_opts.get_d(OPTION_SHOT_MIN_ENERGY) ) break;
            shot_energy -= en;

            Vector2D dir = angle2vec(cannon_angle.pos+robot_angle.pos);
            double shot_radius = the_opts.get_d(OPTION_SHOT_RADIUS);
            Vector2D shot_center = center + (radius+1.5*shot_radius)*dir;
            if( the_arena.space_available( shot_center, shot_radius + eps ) )
              {
                Shot* shotp = new Shot( shot_center, shot_radius,
                                        velocity + dir * the_opts.get_d(OPTION_SHOT_SPEED), en );
                g_list_append((the_arena.get_object_lists())[SHOT], shotp);
              }
            else  // No space for shot, direct hit!!
              { 
                void* col_obj;
                object_type cl_shape;
                double dist;
                if( (dist = the_arena.get_shortest_distance( center, dir, shot_radius+eps, cl_shape, col_obj)) > radius+1.5*shot_radius )
                  {
                    cerr << "Shot has space available after all?" <<  endl;
                    cerr << "dist: " << dist << "      r+1.5sh_r: " << radius+1.5*shot_radius << endl;
                    cerr << "col_shape: " << cl_shape << endl; 
                    //                  throw Error("Shot has space available after all?", "Robot::get_messages");                  
                  }
                switch(cl_shape)
                  {
                  case WALL:
                    break;
                  case ROBOT:
                    {
                      Robot* robotp = (Robot*)col_obj;
                      robotp->change_energy(-en);
                      robotp->send_message(COLLISION, SHOT, -en, 
                                           vec2angle(center+dir*radius-robotp->get_center()) - robotp->get_robot_angle().pos);
                    }
                    break;
                  case SHOT:
                    ((Shot*)col_obj)->die();
                    break;
                  case COOKIE:
                    {
                      Cookie* cookiep =(Cookie*)col_obj;
                      cookiep->die();
                      g_list_remove((the_arena.get_object_lists())[COOKIE], cookiep);
                      delete cookiep;
                    }
                    break;
                  case MINE:
                    {
                      Mine* minep =(Mine*)col_obj;
                      minep->die();
                      g_list_remove((the_arena.get_object_lists())[MINE], minep);
                      delete minep;
                    }
                    break;
                  default:
                    throw Error("Shot on unknown object", "Robot::get_messages");
                  }
              }
            change_energy(-en * the_opts.get_d(OPTION_SHOOTING_PENALTY) );
          }
          break;
        case ACCELERATE:
          {
            double acc;
            *instreamp >> acc;
            if( acc < the_opts.get_d(OPTION_MIN_ACCELERATION) || acc > the_opts.get_d(OPTION_MAX_ACCELERATION) )
              send_message(WARNING, VARIABLE_OUT_OF_RANGE, msg_name);            
            else
              acceleration = acc;
          }
          break;
        case BREAK:
          {
            double brk;
            *instreamp >> brk;
            if( brk < 0.0 || brk > 1.0 )
              send_message(WARNING, VARIABLE_OUT_OF_RANGE, msg_name);            
            else
              break_percent = brk;
          } 
          break;
        case LOAD_DATA:
          // TODO: Load data
          break;
        case SAVE_DATA_FINISHED:
          send_message(EXIT_ROBOT);
          send_signal();
          break;
        case BIN_DATA_FROM:
          // TODO: Receive data
          break;
        default:
          throw Error("Message_type not implemented, ", msg_name, "Robot::get_messages");
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
  energy = min(energy+energy_diff, the_opts.get_d(OPTION_ROBOT_MAX_ENERGY));
  display_energy();
  if( energy <= 0.0 ) die();
}

void
Robot::display_energy()
{
  gtk_clist_set_text(GTK_CLIST(the_gui.get_score_clist()),
                     the_gui.get_robot_nr( this, the_arena.get_all_robots_in_sequence()),
                     2, String((int)energy).non_const_chars());
}

void
Robot::display_place()
{
  gtk_clist_set_text(GTK_CLIST(the_gui.get_score_clist()),
                     the_gui.get_robot_nr(this, the_arena.get_all_robots_in_sequence()),
                     3, String(position_this_game).non_const_chars());
}

void
Robot::display_last()
{
  if(get_last_position() != 0)
    {
      gtk_clist_set_text(GTK_CLIST(the_gui.get_score_clist()),
                         the_gui.get_robot_nr(this, the_arena.get_all_robots_in_sequence()),
                         4, String(get_last_position()).non_const_chars());
    }
}

void
Robot::display_score()
{
  gtk_clist_set_text(GTK_CLIST(the_gui.get_score_clist()),
                     the_gui.get_robot_nr(this, the_arena.get_all_robots_in_sequence()),
                     5, String(get_total_points()).non_const_chars());
}

void
Robot::draw_radar_and_cannon()
{
  // Draw Cannon
  the_gui.draw_line( center,
                     angle2vec(cannon_angle.pos+robot_angle.pos),
                     the_opts.get_d(OPTION_ROBOT_RADIUS) - the_opts.get_d(OPTION_SHOT_RADIUS),
                     the_opts.get_d(OPTION_SHOT_RADIUS),
                     *(the_arena.get_foreground_colour_p()) );

  // Draw radar lines
  Vector2D radar_dir = angle2vec(radar_angle.pos+robot_angle.pos);
  the_gui.draw_line( center - the_opts.get_d(OPTION_ROBOT_RADIUS) * 0.25 * radar_dir,
                     rotate( radar_dir, M_PI / 4.0 ),
                     the_opts.get_d(OPTION_ROBOT_RADIUS) / 1.5,
                     the_opts.get_d(OPTION_ROBOT_RADIUS) / 20.0,
                     *(the_arena.get_foreground_colour_p()) );
  the_gui.draw_line( center - the_opts.get_d(OPTION_ROBOT_RADIUS) * 0.25 * radar_dir,
                     rotate( radar_dir, - (M_PI / 4.0) ),
                     the_opts.get_d(OPTION_ROBOT_RADIUS) / 1.5,
                     the_opts.get_d(OPTION_ROBOT_RADIUS) / 20.0,
                     *(the_arena.get_foreground_colour_p()) );
  
}

Shot::Shot(const Vector2D& c, const double r, 
           const Vector2D& vel, const double en ) 
  : MovingObject(vel), Circle(c, r)
{
  alive = true;
  energy = en;
}

void
Shot::move(const double timestep)
{
  object_type closest_shape;
  void* colliding_object;
  double time_to_collision = the_arena.get_shortest_distance(center, velocity, radius, closest_shape, colliding_object);
  if( time_to_collision > timestep )
    {
      center += timestep*velocity;
    }
  else
    {
      switch( closest_shape )
        {
        case WALL:
          die();
          break;
        case ROBOT:
          {
            Robot* robotp = (Robot*)colliding_object;
            robotp->change_energy(-energy);
            robotp->send_message(COLLISION, SHOT, -energy,
                                 vec2angle(center-robotp->get_center()) - robotp->get_robot_angle().pos);
            die();
          }
          break;
        case SHOT:
          ((Shot*)colliding_object)->die();
          die();
          break;
        case COOKIE:
          {
            Cookie* cookiep =(Cookie*)colliding_object;
            cookiep->die();
            g_list_remove((the_arena.get_object_lists())[COOKIE], cookiep);
            delete cookiep;
            die();
          }
          break;
        case MINE:
          {
            Mine* minep =(Mine*)colliding_object;
            minep->die();
            g_list_remove((the_arena.get_object_lists())[MINE], minep);
            delete minep;
            die();
          }
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
   the_gui.draw_circle(last_drawn_center,last_drawn_radius,*(the_arena.get_background_colour_p()),true);
}
