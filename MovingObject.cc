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
  last_drawn_robot_center = Vector2D(infinity,infinity);
}

Robot::~Robot()
{
  if( is_process_running() ) kill_process_forcefully();
  delete_pipes();
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
      
      pipes[0] = pipe_out[1];
      pipes[1] = pipe_out[0];

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
  send_message(SAVE_DATA);
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
Robot::delete_pipes()
{
  if( instreamp != NULL ) delete instreamp;
  if( outstreamp != NULL ) delete outstreamp;
  close(pipes[0]);
  close(pipes[1]);
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
   the_arena->get_the_gui()->draw_circle(last_drawn_center,last_drawn_radius,*(the_arena->get_background_colour_p()),true);
}

void
Robot::set_stats(int robots_killed_same_time)
{
  int adjust = robots_killed_same_time - 1;
  position_this_game -= adjust; 
  display_place();
  double points_this_game = (double)(the_arena->get_robots_per_game() - position_this_game) - ((double)adjust) * 0.5 + 1.0;

  stat_t* statp = new stat_t
    (
     the_arena->get_sequence_nr(),
     the_arena->get_games_per_sequence() - the_arena->get_games_remaining_in_sequence(),
     position_this_game,
     points_this_game,   
     the_arena->get_total_time(),
     get_total_points() + points_this_game
     );

  g_list_append(statistics, statp);
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
  return( the_arena->get_games_per_sequence() -
          the_arena->get_games_remaining_in_sequence() != 1 
          ? ((stat_t*)g_list_last(statistics)->data)->position
          : 0 );
}

void
Robot::update_radar_and_cannon(const double timestep)
{
  radar_angle += timestep*radar_speed;
  cannon_angle += timestep*cannon_speed;
  robot_angle += timestep*robot_angle_speed;
  shot_energy = min( opts.get_shot_max_energy(), 
                     shot_energy+timestep*opts.get_shot_energy_increase_speed() );
  object_type closest_shape;
  void* col_obj;
  double dist = the_arena->
    get_shortest_distance(center, Vector2D(cos(radar_angle+robot_angle), 
                                           sin(radar_angle+robot_angle)), 
                          0.0, closest_shape, col_obj);
  send_message(RADAR, dist, closest_shape, radar_angle);
  send_message(INFO, the_arena->get_total_time(), length(velocity), cannon_angle); 
}

void
bounce_on_wall(class Robot& robot, const Shape& wall, const Vector2D& normal)
{
  double h, p, b;
  
  if( -dot(normal, Vector2D(cos(robot.robot_angle), sin(robot.robot_angle))) > opts.get_robot_cos_frontsize() )
    {
      h = opts.get_robot_front_hardness();
      b = opts.get_robot_front_bounce_coeff();
      p = opts.get_robot_front_protection();
    }
  else
    {
      h = robot.hardness_coeff;
      b = robot.bounce_coeff;
      p = robot.protection_coeff;
    }  

  double e = b * wall.bounce_coeff;
  Vector2D start_vel = robot.velocity;
  robot.velocity -= (1.0 + e) * dot(normal, robot.velocity) * normal;

  double en_diff = 0.5 * robot.mass * lengthsqr(start_vel - robot.velocity);
  double injury = en_diff * 0.5 * (h + wall.hardness_coeff ) * (1.0-e) * (1.0-p);
  robot.change_energy(-injury);

  robot.send_message(COLLISION, WALL, -injury, -angle(normal)-robot.robot_angle);
}

void
bounce_on_robot(Robot& robot1, Robot& robot2, const Vector2D& normal)
{
  double h1, h2, p1, p2, b1, b2;
  Vector2D dir1_2 = unit(robot2.center - robot1.center);
  
  if( dot(dir1_2, Vector2D(cos(robot1.robot_angle), sin(robot1.robot_angle))) > opts.get_robot_cos_frontsize() )
    {
      h1 = opts.get_robot_front_hardness();
      b1 = opts.get_robot_front_bounce_coeff();
      p1 = opts.get_robot_front_protection();
    }
  else
    {
      h1 = robot1.hardness_coeff;
      b1 = robot1.bounce_coeff;
      p1 = robot1.protection_coeff;
    }

  if( -dot(dir1_2, Vector2D(cos(robot2.robot_angle), sin(robot2.robot_angle))) > opts.get_robot_cos_frontsize() )
    {
      h2 = opts.get_robot_front_hardness();
      b2 = opts.get_robot_front_bounce_coeff();
      p2 = opts.get_robot_front_protection();
    }
  else
    {
      h2 = robot2.hardness_coeff;
      b2 = robot2.bounce_coeff;
      p2 = robot2.protection_coeff;
    }

  double e = b1*b2;
  Vector2D start_vel1 = robot1.velocity;
  Vector2D start_vel2 = robot2.velocity;
  double mass_quotient = robot1.mass / robot2.mass;
  Vector2D tmp = ((1.0 + e) / ( 1 + mass_quotient )) * dot(robot2.velocity - robot1.velocity, normal) * normal;
  robot1.velocity += tmp;
  robot2.velocity -= mass_quotient * tmp;

  double an = angle(normal);
  double en_diff = 0.5 * robot1.mass * lengthsqr(start_vel1 - robot1.velocity);
  double injury = en_diff * 0.5 * (h1 + h2) * (1.0-e) * (1.0-p1);
  robot1.change_energy(-injury);
  robot1.send_message(COLLISION, ROBOT, -injury, an-robot1.robot_angle);

  en_diff = 0.5 * robot2.mass * lengthsqr(start_vel2 - robot2.velocity);
  injury = en_diff * 0.5 * (h1 + h2) * (1.0-e) * (1.0-p2);
  robot2.change_energy(-injury);
  robot2.send_message(COLLISION, ROBOT, -injury, -an-robot2.robot_angle);
}

void
Robot::set_initial_values(const Vector2D& pos, const double angle)
{
  center = pos;
  robot_angle = angle;
  cannon_angle = 0.0;
  radar_angle = 0.0;
  shot_energy = 0.0;
  radius = opts.get_robot_radius();
  protection_coeff = opts.get_robot_protection();
  hardness_coeff = opts.get_robot_hardness();
  bounce_coeff = opts.get_robot_bounce_coeff();
  mass = opts.get_robot_mass();
  energy = opts.get_robot_start_energy();
  velocity = Vector2D(0.0, 0.0);
  position_this_game = 0;
}

void
Robot::change_velocity(const double timestep)
{
  Vector2D dir = Vector2D(cos(robot_angle),sin(robot_angle));
  double gt = opts.get_grav_const() * timestep;
  double fric = opts.get_roll_friction() * (1.0 - break_percent) + 
    opts.get_slide_friction() * break_percent;
  velocity = -velocity* min(opts.get_air_resistance() * timestep, 0.5) +
    timestep*acceleration*dir + 
    dot(velocity, dir) * max(0.0, 1.0-gt*fric) * dir +
    vedge(dir, velocity) * max(0.0, 1.0-gt*opts.get_slide_friction()) * rotate90(dir);
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
      center += time_to_collision*velocity;
      Vector2D new_center = center - eps*velocity;
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
            Shot* shotp =(Shot*)colliding_object;
            double en =  -shotp->get_energy();
            change_energy( en );
            send_message(COLLISION, SHOT, en, angle(shotp->get_center()-center)-robot_angle);
            shotp->die();
            g_list_remove((the_arena->get_object_lists())[SHOT], shotp);
            delete shotp;
          }
          break;
        case COOKIE:
          {
            Cookie* cookiep =(Cookie*)colliding_object;
            double en =  cookiep->get_energy();
            change_energy( en );
            send_message(COLLISION, COOKIE, en, angle(cookiep->get_center()-center)-robot_angle);
            cookiep->die();
            g_list_remove((the_arena->get_object_lists())[COOKIE], cookiep);
            delete cookiep;
          }
          break;
        case MINE:
          {
            Mine* minep =(Mine*)colliding_object;
            double en =  -minep->get_energy();
            change_energy( en );
            send_message(COLLISION, MINE, en, angle(minep->get_center()-center)-robot_angle);
            minep->die();
            g_list_remove((the_arena->get_object_lists())[MINE], minep);
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
            long home_colour, away_colour;

            *instreamp >> hex >> home_colour >> away_colour >> dec;
  
            // TODO: check if colour is already allocated! 
            colour = make_gdk_color(the_arena->find_free_color(home_colour, away_colour, this));
          }
          break;
        case ROTATE:
          { 
            int bits;
            double rot_speed;
            *instreamp >> bits >> rot_speed;
            
            if( (bits & 1) == 1) 
              robot_angle_speed = max(min(rot_speed, opts.get_robot_max_rotate()),
                                      -opts.get_robot_max_rotate());  // between -max_rot and +max_rot
            if( (bits & 2) == 2) 
              cannon_speed = max(min(rot_speed, opts.get_robot_cannon_max_rotate()), 
                                 -opts.get_robot_cannon_max_rotate());
            if( (bits & 4) == 4) 
              radar_speed = max(min(rot_speed, opts.get_robot_radar_max_rotate()), 
                                -opts.get_robot_radar_max_rotate());
          }
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
            en = min(en, shot_energy);
            if( en < opts.get_shot_min_energy() ) break;
            shot_energy -= en;

            Vector2D dir = Vector2D(cos(cannon_angle+robot_angle),sin(cannon_angle+robot_angle));
            double shot_radius = opts.get_shot_radius();
            Vector2D shot_center = center + (radius+1.5*shot_radius)*dir;
            if( the_arena->space_available( shot_center, shot_radius + eps ) )
              {
                Shot* shotp = new Shot( shot_center, shot_radius,
                                        velocity + dir * opts.get_shot_speed(),
                                        the_arena, en );
                g_list_append((the_arena->get_object_lists())[SHOT], shotp);
              }
            else  // No space for shot, direct hit!!
              { 
                void* col_obj;
                object_type cl_shape;
                double dist;
                if( (dist = the_arena->get_shortest_distance( center, dir, shot_radius+eps, cl_shape, col_obj)) > radius+1.5*shot_radius )
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
                                           angle(center+dir*radius-robotp->get_center()) - robotp->get_robot_angle());
                    }
                    break;
                  case SHOT:
                    ((Shot*)col_obj)->die();
                    break;
                  case COOKIE:
                    {
                      Cookie* cookiep =(Cookie*)col_obj;
                      cookiep->die();
                      g_list_remove((the_arena->get_object_lists())[COOKIE], cookiep);
                      delete cookiep;
                    }
                    break;
                  case MINE:
                    {
                      Mine* minep =(Mine*)col_obj;
                      minep->die();
                      g_list_remove((the_arena->get_object_lists())[MINE], minep);
                      delete minep;
                    }
                    break;
                  default:
                    throw Error("Shot on unknown object", "Robot::get_messages");
                  }
              }
            change_energy(-en * opts.get_shooting_penalty() );
          }
          break;
        case ACCELERATE:
          {
            double acc;
            *instreamp >> acc;
            if( acc < opts.get_min_acceleration() || acc > opts.get_max_acceleration() )
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
  energy = min(energy+energy_diff, opts.get_robot_max_energy());
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
  gtk_clist_set_text(GTK_CLIST(the_arena->get_the_gui()->get_score_clist()),
                     the_arena->get_the_gui()->get_robot_nr( this, the_arena->get_all_robots_in_sequence()),
                     2, str_energy);
}

void
Robot::display_place()
{
  strstream ss;
  char str_place[25];

  ss << position_this_game;
  ss >> str_place;

  gtk_clist_set_text(GTK_CLIST(the_arena->get_the_gui()->get_score_clist()),
                     the_arena->get_the_gui()->get_robot_nr(this, the_arena->get_all_robots_in_sequence()),
                     3, str_place);
}

void
Robot::display_last()
{
  strstream ss;
  char str_last[25];

  if(get_last_position() != 0)
    {
      ss << get_last_position();
      ss >> str_last;

      gtk_clist_set_text(GTK_CLIST(the_arena->get_the_gui()->get_score_clist()),
                         the_arena->get_the_gui()->get_robot_nr(this, the_arena->get_all_robots_in_sequence()),
                         4, str_last);
    }
}

void
Robot::display_score()
{
  strstream ss;
  char str_score[25];

  ss << get_total_points();
  ss >> str_score;

  gtk_clist_set_text(GTK_CLIST(the_arena->get_the_gui()->get_score_clist()),
                     the_arena->get_the_gui()->get_robot_nr(this, the_arena->get_all_robots_in_sequence()),
                     5, str_score);
}

void
Robot::draw_radar_and_cannon( Gui& the_gui )
{
  // Draw Cannon
  the_gui.draw_line( center,
                     Vector2D(cos(cannon_angle+robot_angle),sin(cannon_angle+robot_angle)),
                     opts.get_robot_radius() - opts.get_shot_radius(),
                     opts.get_shot_radius(),
                     *(the_arena->get_foreground_colour_p()) );

  // Draw radar lines
  Vector2D radar_dir( cos(radar_angle+robot_angle),sin(radar_angle+robot_angle) );
  the_gui.draw_line( center - opts.get_robot_radius() * 0.25 * radar_dir,
                     rotate( radar_dir, M_PI / 4.0 ),
                     opts.get_robot_radius() / 1.5,
                     opts.get_robot_radius() / 20.0,
                     *(the_arena->get_foreground_colour_p()) );
  the_gui.draw_line( center - opts.get_robot_radius() * 0.25 * radar_dir,
                     rotate( radar_dir, - (M_PI / 4.0) ),
                     opts.get_robot_radius() / 1.5,
                     opts.get_robot_radius() / 20.0,
                     *(the_arena->get_foreground_colour_p()) );
  
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
          {
            Robot* robotp = (Robot*)colliding_object;
            robotp->change_energy(-energy);
            robotp->send_message(COLLISION, SHOT, -energy,
                                 angle(center-robotp->get_center()) - robotp->get_robot_angle());
            die();
          }
          break;
        case SHOT:
          // TODO: shot explode on shot ?
          ((Shot*)colliding_object)->die();
          die();
          break;
        case COOKIE:
          {
            Cookie* cookiep =(Cookie*)colliding_object;
            cookiep->die();
            g_list_remove((the_arena->get_object_lists())[COOKIE], cookiep);
            delete cookiep;
            die();
          }
          break;
        case MINE:
          {
            Mine* minep =(Mine*)colliding_object;
            minep->die();
            g_list_remove((the_arena->get_object_lists())[MINE], minep);
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
   the_arena->get_the_gui()->draw_circle(last_drawn_center,last_drawn_radius,*(the_arena->get_background_colour_p()),true);
}
