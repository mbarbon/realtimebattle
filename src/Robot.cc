/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-1999  Erik Ouchterlony and Ragnar Ouchterlony

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <iostream.h>
#include <math.h>

#ifdef TIME_WITH_SYS_TIME 
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include <sys/resource.h>
#include <sys/stat.h>
#include <stdarg.h>

#include "Robot.h"
#include "String.h"
#include "ArenaController.h"
#include "ArenaRealTime.h"
#include "Various.h"
#include "Options.h"
#include "Wall.h"
#include "Shot.h"
#include "Extras.h"
#include "ArenaWindow.h"

#ifndef NO_GRAPHICS
# include "ScoreWindow.h"
#endif

//  #include "Gui.h"
//  #include "MovingObject.h"
//  #include "Arena.h"
//  #include "Extras.h"
//  #include "Options.h"
//  #include "Messagetypes.h"

Robot::Robot(const String& filename)
{
  velocity = Vector2D(0.0, 0.0);
  acceleration = 0.0;
  robot_filename = filename;
  plain_robot_name = "";
  robot_name = "";
  robot_name_uniqueness_number = 0;
  //  robot_dir= getenv("RTB_ROBOTDIR");
  extra_air_resistance = 0.0;
  process_running = false;

  send_usr_signal = false;
  signal_to_send = 0;
  send_rotation_reached = 0;
  alive = false;
  died_this_round = false;
  total_points = 0.0;
  
  has_competed = false;

  id = the_arena.increase_robot_count();

  instreamp = NULL;
  outstreamp = NULL;
  pipes[0] = -1;  
  pipes[1] = -1;
  pid = -1;
  last_drawn_robot_center = Vector2D(infinity,infinity);
}

// Constructor used by ArenaReplay. No process needed.
//
Robot::Robot(const int r_id, const long int col, const String& name)
{
  id = r_id;
  robot_name = name;
  set_colour( col );

  process_running = false;
  alive = false;
  total_points = 0.0;
  
  has_competed = false;

  last_drawn_robot_center = Vector2D(infinity,infinity);
  radius = the_opts.get_d(OPTION_ROBOT_RADIUS);
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
    Error(true, "Couldn't setup pipe_in for robot " + robot_filename, "Robot::start_process");

  if (pipe (pipe_out)) 
    Error(true, "Couldn't setup pipe_out for robot " + robot_filename, "Robot::start_process");

  if( (pid = fork()) < 0 )
    Error(true, "Couldn't fork childprocess for robot " + robot_filename, "Robot::start_process");

  if(pid == 0)   // Child process, to be the new robot
    {
      // Make pipe_out the standard input for the robot
      close(pipe_out[1]);
      dup2(pipe_out[0], STDIN_FILENO);

      // Make pipe_in the standard output
      dup2(pipe_in[1],STDOUT_FILENO);
      close(pipe_in[0]);

      // Make the pipes non-blocking
      int pd_flags;
      if( (pd_flags = fcntl(pipe_out[0], F_GETFL, 0)) == -1 ) 
        Error(true, "Couldn't get pd_flags for pipe_out in robot " + robot_filename, 
              "Robot::start_process, child");
      pd_flags |= O_NONBLOCK;
      if( fcntl(pipe_out[0], F_SETFL, pd_flags) == -1 ) 
        Error(true, "Couldn't change pd_flags for pipe_out in robot " + robot_filename, 
              "Robot::start_process, child");

      // Should the robots outputpipe be NON_BLOCKING ?

      if( (pd_flags = fcntl(pipe_in[1], F_GETFL, 0)) == -1 ) 
        Error(true, "Couldn't get pd_flags for pipe_in in robot " + robot_filename, 
              "Robot::start_process, child");
      pd_flags |= O_NONBLOCK;
      if( fcntl(pipe_in[1], F_SETFL, pd_flags) == -1 ) 
        Error(true, "Couldn't change pd_flags for pipe_in in robot " + robot_filename, 
              "Robot::start_process, child");
      
      // Check file attributes

      struct stat filestat;
      if( 0 != stat( robot_filename.chars(), &filestat ) ) 
        Error(true, "Couldn't get stats for robot " + robot_filename, "Robot::start_process, child");
      if( !S_ISREG( filestat.st_mode) )
        Error(true, "Robot file isn't regular, error for robot " + robot_filename, 
              "Robot::start_process, child");
      if( !(filestat.st_mode & S_IXOTH) )
        Error(true, "Robot file isn't executable for user, error for robot " + robot_filename, 
              "Robot::start_process, child");
      if( (filestat.st_mode & S_ISUID) )
        Error(true, "Set user ID is not allowed, error for robot " + robot_filename, 
              "Robot::start_process, child");

      // Lower priority by one
     
      int old;
      if( (old = getpriority (PRIO_PROCESS, 0)) == -1 )
        Error(true, "Couldn't get priority for robot " + robot_filename, "Robot::start_process, child");
      if( setpriority (PRIO_PROCESS, 0, old + 1) == -1)
        Error(true, "Couldn't set priority for robot " + robot_filename, "Robot::start_process, child");
      
      // Close all pipes not belonging to the robot
      
      Robot* robotp;

      ListIterator<Robot> li;
      for( the_arena.get_all_robots_in_sequence()->first(li); li.ok(); li++ )
        {
          robotp = li();
          if( robotp != this ) robotp->delete_pipes();
        }

        if( the_arena.get_game_mode() != ArenaBase::DEBUG_MODE )
          {
            struct rlimit res_limit;

//        // Deny file access
          
//            if( getrlimit( RLIMIT_NOFILE, &res_limit ) == -1 )
//              Error(true, "Couldn't get file limits for robot " + robot_filename, 
//                    "Robot::start_process, child");
          
//            //res_limit.rlim_cur = 7;   // Don't know why, but it is the lowest number that works
//            if( setrlimit( RLIMIT_NOFILE, &res_limit ) == -1 )
//              Error(true, "Couldn't limit file access for robot " + robot_filename, 
//                    "Robot::start_process, child");
          

          // Forbid creation of child processes
          
#ifdef HAVE_RLIMIT_NPROC
          if( getrlimit( RLIMIT_NPROC, &res_limit ) == -1 )
            Error(true, "Couldn't get proc limits for robot " + robot_filename, 
                  "Robot::start_process, child");
          
          res_limit.rlim_cur = 0;
          if( setrlimit( RLIMIT_NPROC, &res_limit ) == -1 )
            Error(true, "Couldn't limit child processes for robot " + robot_filename, 
                  "Robot::start_process, child");
#endif
        }

      // Execute process. Should not return!
      if( execl(robot_filename.chars(), robot_filename.chars(), NULL) == -1 )
        Error(true, "Couldn't open robot " + robot_filename, "Robot::start_process, child");

      Error(true, "Robot didn't execute, SHOULD NEVER HAPPEN!, error for " + robot_filename, 
            "Robot::start_process, child");
    }
  else
    {
      close(pipe_out[0]);     // Close input side of pipe_out
      close(pipe_in[1]);      // Close output side of pipe_in  
      
      pipes[0] = pipe_out[1];
      pipes[1] = pipe_in[0];

      // Make the pipes non-blocking
      int pd_flags;
      if( (pd_flags = fcntl(pipe_in[0], F_GETFL, 0)) == -1 ) 
        Error(true, "Couldn't get pd_flags for pipe_in in robot " + robot_filename, 
              "Robot::start_process, parent");
      pd_flags |= O_NONBLOCK;
      if( fcntl(pipe_in[0], F_SETFL, pd_flags) == -1 ) 
        Error(true, "Couldn't change pd_flags for pipe_in in robot " + robot_filename, 
              "Robot::start_process, parent");
      if( (pd_flags = fcntl(pipe_out[1], F_GETFL, 0)) == -1 ) 
        Error(true, "Couldn't get pd_flags for pipe_out in robot " + robot_filename, 
              "Robot::start_process, parent");
      pd_flags |= O_NONBLOCK;
      if( fcntl(pipe_out[1], F_SETFL, pd_flags) == -1 ) 
        Error(true, "Couldn't change pd_flags for pipe_out in robot " + robot_filename, 
              "Robot::start_process, parent");

      outstreamp = new ofstream(pipe_out[1]);
      instreamp = new ifstream(pipe_in[0]);
    }

  // wait some time to let process start up

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 40000;  //  1/25 s 

  select(FD_SETSIZE, NULL, NULL, NULL, &timeout);
  
  set_values_at_process_start_up();
}

bool
Robot::is_process_running()
{
  return process_running;
}

void
Robot::check_process()
{
  String procfilename = "/proc/" + String(pid) + "/stat";

  if( is_process_running() )
    {
      ifstream procfile(procfilename.chars());
      if( !procfile ) 
        {
          process_running = false;
          return;
        }

      char buf[16];

      for(int i=0; i<13; i++)
        procfile >> buf;
      
      int jiffies;
      
      procfile >> jiffies;

      double current_cpu = (double)jiffies / 100.0;
      double tot_time = the_arena.get_total_time() + time_survived_in_sequence;
      if( !alive ) tot_time = time_survived_in_sequence;

      if( current_cpu > cpu_next_limit )
        {
          if( tot_time >= cpu_timeout )
            {
              // add time
              cpu_warning_limit = cpu_next_limit + 
                the_opts.get_d(OPTION_CPU_EXTRA) * the_opts.get_d(OPTION_CPU_WARNING_PERCENT);
              cpu_next_limit += the_opts.get_d(OPTION_CPU_EXTRA);
              cpu_timeout = tot_time + the_opts.get_d(OPTION_CPU_PERIOD);
            }
          else
            {
              // cpu limit exceeded, robot disqualified
              die();
              // add time for next game
              cpu_warning_limit = cpu_next_limit + 
                the_opts.get_d(OPTION_CPU_EXTRA) * the_opts.get_d(OPTION_CPU_WARNING_PERCENT);
              cpu_next_limit += the_opts.get_d(OPTION_CPU_EXTRA);
              cpu_timeout = tot_time + the_opts.get_d(OPTION_CPU_PERIOD);
            }
        }
      else if( current_cpu > cpu_warning_limit && tot_time < cpu_timeout )
        {
          send_message( WARNING, PROCESS_TIME_LOW, String(cpu_next_limit - current_cpu).chars());
          cpu_warning_limit = cpu_next_limit;
        }
    }
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
  if( send_usr_signal )
    kill(pid, signal_to_send);
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
      died_this_round = true;
#ifndef NO_GRAPHICS
      if( !no_graphics )
        {
          the_gui.get_arenawindow_p()->
            draw_circle( last_drawn_center, last_drawn_radius,
                         *(the_gui.get_bg_gdk_colour_p()), true );
          last_drawn_robot_center = Vector2D( infinity, infinity );
        }
#endif
    }
}

void
Robot::set_stats(const int robots_killed_same_time)
{
  died_this_round = false;

  int adjust = robots_killed_same_time - 1;
  position_this_game = the_arena.get_robots_left() - adjust;
  add_points( 1.0 + ((double)adjust) * 0.5 );

  time_survived_in_sequence += the_arena.get_total_time();

#ifndef NO_GRAPHICS
  if( !no_graphics ) display_score();
#endif

  if( the_arena_controller.is_realtime() )
    {
      send_message(DEAD);
      send_signal();

      realtime_arena.print_to_logfile('D', (int)'R', id, points_this_game,
                                      position_this_game);
    }

  stat_t* statp = new stat_t
    (
     the_arena.get_sequence_nr(),
     the_arena.get_game_nr(),
     position_this_game,
     points_this_game,   
     the_arena.get_total_time(),
     total_points
     );

  statistics.insert_last( statp );
}

// Version of set_stats used by ArenaReplay
//
void
Robot::set_stats(const double pnts, const int pos, const double time_survived)
{
  position_this_game = pos;
  total_points = total_points - points_this_game + pnts;
  points_this_game = pnts;

  time_survived_in_sequence += time_survived;

#ifndef NO_GRAPHICS
  if( !no_graphics ) display_score();
#endif

stat_t* statp = new stat_t
    (
     the_arena.get_sequence_nr(),
     the_arena.get_game_nr(),
     position_this_game,
     points_this_game,   
     time_survived,
     total_points
     );

  statistics.insert_last( statp );
}

void
Robot::change_position( const double x, const double y, 
                        const double robot_a, const double cannon_a, 
                        const double radar_a, const double en )
{
  center = Vector2D(x, y);
  robot_angle.pos  = robot_a;
  cannon_angle.pos = cannon_a;
  radar_angle.pos  = radar_a;
  energy = en;
#ifndef NO_GRAPHICS  
  if( !no_graphics )  display_score();
#endif
}

void
Robot::check_name_uniqueness()
{
  Robot* robotp;
  int first_avail = 0;

  robot_name = plain_robot_name;
  
  ListIterator<Robot> li;
  for( the_arena.get_all_robots_in_tournament()->first(li); li.ok(); li++ )
    {
      robotp = li();
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

//  double
//  Robot::get_total_points()
//  {
//    ListIterator<stat_t> li;
//    statistics.last(li);
//    stat_t* sp = li();
//    return( sp != NULL ? sp->total_points + points : points );
//  }

int
Robot::get_last_position()
{
  if( the_arena.get_game_nr() == 1 ) return 0; // first game

  ListIterator<stat_t> li;
  statistics.last(li);

  if( !li.ok() ) return 0;
  return li()->position;
}

bool
Robot::update_rotation(rotation_t& angle, const double timestep)
{
  angle.pos += timestep * angle.vel;
  bool rot_reached = false;

  if( angle.pos >= angle.right && angle.mode == ROTATE_TO_RIGHT )
    {
      angle.set( angle.right, 0.0, -infinity, infinity, NORMAL_ROT);
      if( send_rotation_reached >= 1 ) rot_reached = true;
    }

  if( angle.pos >= angle.right && angle.mode == SWEEP_RIGHT )
    {
      angle.set( angle.right, -angle.vel, angle.left, angle.right, SWEEP_LEFT);
      if( send_rotation_reached >= 2 ) rot_reached = true;
    }
  
  if( angle.pos <= angle.left && angle.mode == ROTATE_TO_LEFT )
    {      
      angle.set( angle.left, 0.0, -infinity, infinity, NORMAL_ROT);
      if( send_rotation_reached >= 1 ) rot_reached = true;
    }

  if( angle.pos <= angle.left && angle.mode == SWEEP_LEFT )
    {
      angle.set( angle.left, -angle.vel, angle.left, angle.right, SWEEP_RIGHT);
      if( send_rotation_reached >= 2 ) rot_reached = true;
    }

  return rot_reached;
}

void
Robot::update_radar_and_cannon(const double timestep)
{
  int rot_reached = 0;
  if( update_rotation(robot_angle, timestep) )  rot_reached += 1;
  if( update_rotation(cannon_angle, timestep) ) rot_reached += 2;
  if( update_rotation(radar_angle, timestep) )  rot_reached += 4;

  if( rot_reached > 0 ) send_message(ROTATION_REACHED, rot_reached);

  shot_energy = min( the_opts.get_d(OPTION_SHOT_MAX_ENERGY), 
                     shot_energy+timestep*the_opts.get_d(OPTION_SHOT_ENERGY_INCREASE_SPEED) );

  object_type closest_arenaobject;
  Shape* col_obj;
  double dist = the_arena.
    get_shortest_distance(center, angle2vec(radar_angle.pos+robot_angle.pos),
                          0.0, closest_arenaobject, col_obj, this);

  send_message(RADAR, dist, closest_arenaobject, radar_angle.pos);
  if( closest_arenaobject == ROBOT )
    {
      double lvls = (double)the_opts.get_l(OPTION_ROBOT_ENERGY_LEVELS);
      double en = ((Robot*)col_obj)->get_energy();
      send_message(ROBOT_INFO, rint( en / lvls ) * lvls, 0);
    }
  send_message(INFO, the_arena.get_total_time(), length(velocity), cannon_angle.pos); 
}

void
Robot::bounce_on_wall(const double bounce_c, const double hardness_c, const Vector2D& normal)
{
  double h, p, b;
  
  if( -dot(normal, angle2vec(robot_angle.pos)) > cos(the_opts.get_d(OPTION_ROBOT_FRONTSIZE)*0.5) )
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

  double e = b * bounce_c;
  Vector2D start_vel = velocity;
  velocity -= (1.0 + e) * dot(normal, velocity) * normal;

  double en_diff = 0.5 * the_opts.get_d(OPTION_ROBOT_MASS) * lengthsqr(start_vel - velocity);
  double injury = en_diff * 0.5 * (h + hardness_c ) * (1.0-e) * (1.0-p);
  change_energy(-injury);

  send_message(COLLISION, WALL, -vec2angle(normal)-robot_angle.pos);
}

void
bounce_on_robot(Robot& robot1, Robot& robot2, const Vector2D& normal)
{
  double h1, h2, p1, p2, b1, b2;
  Vector2D dir1_2 = unit(robot2.center - robot1.center);
  
  if( dot(dir1_2, angle2vec(robot1.robot_angle.pos)) > cos(the_opts.get_d(OPTION_ROBOT_FRONTSIZE)*0.5) )
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

  if( -dot(dir1_2, angle2vec(robot2.robot_angle.pos)) > cos(the_opts.get_d(OPTION_ROBOT_FRONTSIZE)*0.5) )
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
  robot1.send_message(COLLISION, ROBOT, an-robot1.robot_angle.pos);

  en_diff = 0.5 * mass * lengthsqr(start_vel2 - robot2.velocity);
  injury = en_diff * 0.5 * (h1 + h2) * (1.0-e) * (1.0-p2);
  robot2.change_energy(-injury);
  robot2.send_message(COLLISION, ROBOT, -an-robot2.robot_angle.pos);
}

void
Robot::set_values_before_game(const Vector2D& pos, const double angle)
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
  points_this_game = 0.0;
  brake_percent = 0.0;
  acceleration = 0.0;
}

void
Robot::set_values_at_process_start_up()
{
  process_running = true;
  has_saved = false;
  time_survived_in_sequence = 0.0;
  cpu_next_limit = the_opts.get_d(OPTION_CPU_START_LIMIT);
  cpu_warning_limit = cpu_next_limit * the_opts.get_d(OPTION_CPU_WARNING_PERCENT);
  cpu_timeout = 0.0;

  if( statistics.is_empty() )
    {
      send_message(INITIALIZE, 1);        // first sequence !
    }
  else
    {
      send_message(INITIALIZE, 0);        // not first sequence !
      send_message(YOUR_NAME, robot_name.chars());
      int long col = rgb_colour;
      int long newcol = realtime_arena.find_free_colour(col, col, this);
      if( col != newcol ) set_colour( newcol );
      // TODO: probably free color!
      send_message(YOUR_COLOUR, newcol);
    } 
}

void
Robot::change_velocity(const double timestep)
{
  Vector2D dir = angle2vec(robot_angle.pos);
  double gt = the_opts.get_d(OPTION_GRAV_CONST) * timestep;
  double fric = the_opts.get_d(OPTION_ROLL_FRICTION) * (1.0 - brake_percent) + 
    the_opts.get_d(OPTION_SLIDE_FRICTION) * brake_percent;
  velocity = -velocity* min(the_opts.get_d(OPTION_AIR_RESISTANCE) * timestep, 0.5) +
    timestep*acceleration*dir + 
    dot(velocity, dir) * max(0.0, 1.0-gt*fric) * dir +
    vedge(dir, velocity) * max(0.0, 1.0-gt*the_opts.get_d(OPTION_SLIDE_FRICTION)) * rotate90(dir);
}

void
Robot::move(const double timestep)
{
  move(timestep, 1, timestep / 50.0);

  if( is_alive() )
    realtime_arena.print_to_logfile('R', id, center[0], center[1],
                                    robot_angle.pos, cannon_angle.pos, 
                                    radar_angle.pos, energy);

}

void
Robot::move(const double timestep, int iterstep, const double eps)
{
  object_type closest_shape;
  Shape* colliding_object;
  double time_to_collision = 
    the_arena.get_shortest_distance(center, velocity, radius, 
                                    closest_shape, colliding_object, this);
  if( time_to_collision > timestep )
    {
      center += timestep*velocity;
    }
  else
    {
      if( iterstep > 10 ) time_to_collision = max( time_to_collision , eps );
      double time_remaining = timestep - time_to_collision; 
      center += time_to_collision*velocity;
      //      Vector2D new_center = center - min(eps, time_to_collision)*velocity;
      

      switch( closest_shape )
        {
        case WALL:
          {
            Vector2D normal = colliding_object->get_normal(center);
            bounce_on_wall(colliding_object->get_bounce_coeff(), 
                           colliding_object->get_hardness_coeff(), normal);
            center += normal*eps;
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
            send_message(COLLISION, SHOT, vec2angle(shotp->get_center()-center)-robot_angle.pos);
            shotp->die();
            the_arena.get_object_lists()[SHOT].remove( shotp );
          }
          break;
        case COOKIE:
          {
            Cookie* cookiep =(Cookie*)colliding_object;
            double en =  cookiep->get_energy();
            change_energy( en );
            send_message(COLLISION, COOKIE, vec2angle(cookiep->get_center()-center)-robot_angle.pos);
            cookiep->die();
            the_arena.get_object_lists()[COOKIE].remove( cookiep );
          }
          break;
        case MINE:
          {
            Mine* minep =(Mine*)colliding_object;
            double en =  -minep->get_energy();
            change_energy( en );
            send_message(COLLISION, MINE, vec2angle(minep->get_center()-center)-robot_angle.pos);
            minep->die();
            the_arena.get_object_lists()[MINE].remove( minep );
          }
          break;
        default:
          Error(true, "Collided with unknown object", "Robot::move");
          break;
        }
      
      //      center = new_center;
      //if( iterstep % 10 == 0 ) 
      //  cout << "Iterstep: " << iterstep << "   time_remaining: " << time_remaining
      //       << "  Collided on: " << closest_shape << "   time_to_col: " << time_to_collision << endl;
      if( iterstep > 65 ) Error(true, "Too many bounces, must be a bug!", "Robot::move");
      if( alive && time_remaining > 0.0 ) move( time_remaining, iterstep + 1, eps );
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
          Error(true, "Couldn't send message, no arg_type", "Robot::send_message");
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
          Error(true, "Couldn't send message, unknown arg_type", "Robot::send_message");
        }
    }
  *outstreamp << endl;
}


// This is the function which gets messages from the robot,
// parses them and take appropriate action. It is certainly 
// too long, but who cares :-)
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
      //      cerr << "Got message: " << msg_name << endl;

      *instreamp >> ws;

      switch(msg_t)
        {
        case UNKNOWN_MESSAGE_FROM_ROBOT:
          //cout << "Server: Warning sent for message: " << msg_name << endl;
          send_message(WARNING, UNKNOWN_MESSAGE, msg_name);
          instreamp->get(buffer, 80, '\n');
          break;
        case ROBOT_OPTION:
          if( check_state_for_message(msg_t, STARTING_ROBOTS) )
            {
              int opt_nr, val;
              *instreamp >> opt_nr;
              switch(opt_nr)
                {
                case SEND_SIGNAL:
                  *instreamp >> val;
                  send_usr_signal = (val == true);
                  signal_to_send = SIGUSR1;
                  send_signal();
                  break;
                case SIGNAL:
                  *instreamp >> val;
                  if( val > 0 && val < NSIG )
                    {
                      signal_to_send = val;
                      send_usr_signal = true;
                      send_signal();

                    }
                  else
                    {                      
                      if( val >= NSIG ) send_message(WARNING, UNKNOWN_OPTION, msg_name);
                      signal_to_send = 0;
                      send_usr_signal = false;
                    }
                  break;

                case SEND_ROTATION_REACHED:
                  *instreamp >> val;
                  if( val < 0 ) val = 0;
                  if( val > 2 ) val = 2;
                  send_rotation_reached = val;
                  break;
                default:
                  send_message(WARNING, UNKNOWN_OPTION, msg_name);
                  break;
                }
            }
          break;
        case NAME:
          if( check_state_for_message(msg_t, STARTING_ROBOTS) )
            {
              instreamp->get(text, 80, '\n');
              plain_robot_name = text;
              check_name_uniqueness();
            }
          break;
        case COLOUR:
          if( check_state_for_message(msg_t, STARTING_ROBOTS) )
            {
              long home_colour, away_colour;
              
              *instreamp >> hex >> home_colour >> away_colour >> dec;
              
              // TODO: check if colour is already allocated! 
              set_colour( realtime_arena.find_free_colour(home_colour, away_colour, this) );
            }
          break;
        case ROTATE:
          if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
            { 
              int bits;
              double rot_speed;
              *instreamp >> bits >> rot_speed;
              
              double rot_sign = sgn(rot_speed);
              rot_speed = fabs(rot_speed);
              if( bits & 1 ) rot_speed = min( rot_speed, the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE) );
              if( bits & 2 ) rot_speed = min( rot_speed, the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE) );
              if( bits & 4 ) rot_speed = min( rot_speed, the_opts.get_d(OPTION_ROBOT_RADAR_MAX_ROTATE) );
              rot_speed *= rot_sign;

              if( bits & 1 ) 
                robot_angle.set( robot_angle.pos, rot_speed,
                                 -infinity, infinity, NORMAL_ROT );
              if( bits & 2 ) 
                cannon_angle.set( cannon_angle.pos, rot_speed,  
                                  -infinity, infinity, NORMAL_ROT );
              if( bits & 4 )
                radar_angle.set( radar_angle.pos, rot_speed,
                                 -infinity, infinity, NORMAL_ROT );
            }
          break;

        case ROTATE_TO:
          if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
          {
            int bits;
            double rot_speed, rot_end_angle, rot_amount;
            *instreamp >> bits >> rot_speed >> rot_end_angle;
            rot_end_angle = max(min(rot_end_angle, infinity), -infinity);

            rot_speed = fabs(rot_speed);
            if( bits & 2 ) rot_speed = min( rot_speed, the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE) );
            if( bits & 4 ) rot_speed = min( rot_speed, the_opts.get_d(OPTION_ROBOT_RADAR_MAX_ROTATE) );

            if( bits & 2 )
              {
                cannon_angle.pos -= rint( (cannon_angle.pos-rot_end_angle) / (2.0*M_PI) ) * 2.0 * M_PI;
                rot_amount = rot_end_angle - cannon_angle.pos;
                if( rot_amount > 0 )
                  cannon_angle.set( cannon_angle.pos, rot_speed, 
                                    -infinity, cannon_angle.pos + rot_amount, 
                                    ROTATE_TO_RIGHT );
                else
                  cannon_angle.set( cannon_angle.pos, -rot_speed,
                                    cannon_angle.pos + rot_amount, infinity, 
                                    ROTATE_TO_LEFT );
              }
            if( bits & 4 )
              {
                radar_angle.pos -= rint( (radar_angle.pos-rot_end_angle) / (2.0*M_PI) ) * 2.0 * M_PI;
                rot_amount = rot_end_angle - radar_angle.pos;
                if( rot_amount > 0 )
                  radar_angle.set( radar_angle.pos, rot_speed,
                                   -infinity, radar_angle.pos + rot_amount, 
                                   ROTATE_TO_RIGHT );
                else
                  radar_angle.set( radar_angle.pos, -rot_speed,
                                   radar_angle.pos + rot_amount, infinity, 
                                   ROTATE_TO_LEFT );
              }            
          }
          break;

        case ROTATE_AMOUNT:
          if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
          {
            int bits;
            double rot_speed, rot_amount;
            *instreamp >> bits >> rot_speed >> rot_amount;

            rot_speed = fabs(rot_speed);
            if( bits & 1 ) rot_speed = min( rot_speed, the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE) );
            if( bits & 2 ) rot_speed = min( rot_speed, the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE) );
            if( bits & 4 ) rot_speed = min( rot_speed, the_opts.get_d(OPTION_ROBOT_RADAR_MAX_ROTATE) );

            if( bits & 1 )
              {
                if( rot_amount > 0 )
                  robot_angle.set( robot_angle.pos, rot_speed,
                                   -infinity, robot_angle.pos + rot_amount, 
                                   ROTATE_TO_RIGHT );
                else
                  robot_angle.set( robot_angle.pos, -rot_speed,
                                   robot_angle.pos + rot_amount, infinity, 
                                   ROTATE_TO_LEFT );
              }
            if( bits & 2 )
              {
                if( rot_amount > 0 )
                  cannon_angle.set( cannon_angle.pos, rot_speed,
                                    -infinity, cannon_angle.pos + rot_amount, 
                                    ROTATE_TO_RIGHT );
                else
                  cannon_angle.set( cannon_angle.pos, -rot_speed,
                                    cannon_angle.pos + rot_amount, infinity, 
                                    ROTATE_TO_LEFT );
              }
            if( bits & 4 )
              {
                if( rot_amount > 0 )
                  radar_angle.set( radar_angle.pos, rot_speed,
                                   -infinity, radar_angle.pos + rot_amount, 
                                   ROTATE_TO_RIGHT );
                else
                  radar_angle.set( radar_angle.pos, -rot_speed,
                                   radar_angle.pos + rot_amount, infinity, 
                                   ROTATE_TO_LEFT );
              }
          }
          break;
        case SWEEP:
          if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
          {
            int bits;
            double rot_speed, sweep_left, sweep_right;
            *instreamp >> bits >> rot_speed >> sweep_left >> sweep_right;
            sweep_left = max(min(sweep_left, infinity), -infinity);
            sweep_right = max(min(sweep_right, infinity), -infinity);
            rotation_mode_t rot_dir;
            rot_dir = ( rot_speed < 0 ? SWEEP_LEFT :  SWEEP_RIGHT );

            if( bits & 2 ) rot_speed = min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_CANNON_MAX_ROTATE) );
            if( bits & 4 ) rot_speed = min( fabs(rot_speed), the_opts.get_d(OPTION_ROBOT_RADAR_MAX_ROTATE) );
            
            if( bits & 2 )
              {
                cannon_angle.pos -= rint( (cannon_angle.pos - 0.5*(sweep_left+sweep_right)) / 
                                          (2.0*M_PI) ) * 2.0 * M_PI;
                if( fabs(cannon_angle.vel) > 1e-10 ) 
                  rot_dir = ( cannon_angle.vel < 0 ? SWEEP_LEFT :  SWEEP_RIGHT );
                  
                if( cannon_angle.pos <= sweep_left  ) rot_dir = SWEEP_RIGHT;
                if( cannon_angle.pos >= sweep_right ) rot_dir = SWEEP_LEFT;

                double cannon_speed = rot_speed;
                if( rot_dir == SWEEP_LEFT ) cannon_speed = -rot_speed;    
                cannon_angle.set( cannon_angle.pos, cannon_speed, 
                                  sweep_left, sweep_right, rot_dir );
              }
            if( bits & 4 )
              {
                radar_angle.pos -= rint( (radar_angle.pos - 0.5*(sweep_left+sweep_right)) / 
                                         (2.0*M_PI) ) * 2.0 * M_PI;
                if( fabs(radar_angle.vel) > 1e-10 ) 
                  rot_dir = ( radar_angle.vel < 0 ? SWEEP_LEFT :  SWEEP_RIGHT );
                  
                if( radar_angle.pos <= sweep_left  ) rot_dir = SWEEP_RIGHT;
                if( radar_angle.pos >= sweep_right ) rot_dir = SWEEP_LEFT;
                
                double radar_speed = rot_speed;
                if( rot_dir == SWEEP_LEFT ) radar_speed = -rot_speed;    
                radar_angle.set( radar_angle.pos, radar_speed,
                                 sweep_left, sweep_right, rot_dir );
              }
          }
          break;
        case PRINT:
          {
            instreamp->get(text, 80, '\n');
            realtime_arena.print_to_logfile('P', id, text);
            the_arena.print_message( robot_name, text );
          }
          break;

        case DEBUG:
          {
            instreamp->get(text, 80, '\n');
            realtime_arena.print_to_logfile('P', id, text);
            if( realtime_arena.get_game_mode() == ArenaBase::DEBUG_MODE )
              the_arena.print_message( robot_name, text );
          }
          break;

        case DEBUG_LINE:
          if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
            {
              if( !the_arena.is_max_debug_level() )
                {
                  send_message(WARNING, MESSAGE_SENT_IN_ILLEGAL_STATE,
                               message_from_robot[msg_t].msg);
                }
#ifndef NO_GRAPHICS
              else if( !no_graphics )
                {
                  double a1, d1, a2, d2;
                  *instreamp >> a1 >> d1 >> a2 >> d2;
                  
                  Vector2D start = d1 * angle2vec(a1 + robot_angle.pos);
                  Vector2D direction = d2 * angle2vec(a2 + robot_angle.pos) - start;
                  
                  the_gui.get_arenawindow_p()->
                    draw_line(start + center, direction, 1.0, gdk_colour);
                }
#endif NO_GRAPHICS              
            }
          break;

        case DEBUG_CIRCLE:
         if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
            {
              if( !the_arena.is_max_debug_level() )
                {
                  send_message(WARNING, MESSAGE_SENT_IN_ILLEGAL_STATE,
                               message_from_robot[msg_t].msg);
                }
#ifndef NO_GRAPHICS
              else if( !no_graphics )
                {
                  double a, d, r;
                  *instreamp >> a >> d >> r;

                  Vector2D c = d * angle2vec(a + robot_angle.pos) + center;

                  the_gui.get_arenawindow_p()->
                    draw_circle(c, r, gdk_colour, false);
                }
#endif NO_GRAPHICS
            }         
         break;          

        case SHOOT:
          if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
            {
              double en;
              *instreamp >> en;
              en = min(en, shot_energy);
              if( en < the_opts.get_d(OPTION_SHOT_MIN_ENERGY) ) break;
              shot_energy -= en;
              
              Vector2D dir = angle2vec(cannon_angle.pos+robot_angle.pos);
              double shot_radius = the_opts.get_d(OPTION_SHOT_RADIUS);
              Vector2D shot_center = center + (radius+1.5*shot_radius)*dir;
              Vector2D shot_vel = velocity + dir * the_opts.get_d(OPTION_SHOT_SPEED);

              if( realtime_arena.space_available( shot_center, shot_radius*1.00001 ) )
                {
                  Shot* shotp = new Shot( shot_center, shot_vel, en );
                  realtime_arena.get_object_lists()[SHOT].insert_last( shotp );

                  realtime_arena.print_to_logfile('S', shotp->get_id(), shot_center[0], shot_center[1], 
                                   shot_vel[0], shot_vel[1]);
                }
              else  // No space for shot, direct hit!!
                { 
                  Shape* col_obj;
                  object_type cl_shape;
                  double dist;
                  if( (dist = realtime_arena.get_shortest_distance
                       ( center, dir, shot_radius*1.00001, 
                         cl_shape, col_obj, this))    >   radius+1.5*shot_radius )
                    {
                      //cerr << "Shot has space available after all?" <<  endl;
                      cerr << "dist: " << dist << "      r+1.5sh_r: " << radius+1.5*shot_radius << endl;
                      cerr << "col_shape: " << cl_shape << endl; 
                      Error(true, "Shot has space available after all?", "Robot::get_messages");                  
                    }
                  switch(cl_shape)
                    {
                    case WALL:
                      break;
                    case ROBOT:
                      {
                        Robot* robotp = (Robot*)col_obj;
                        robotp->change_energy(-en);
                        robotp->send_message(COLLISION, SHOT, 
                                             vec2angle(center+dir*radius-robotp->get_center()) - robotp->get_robot_angle().pos);
                      }
                      break;
                    case SHOT:
                      shot_collision((Shot*)col_obj, shot_vel, en);
                      break;
                    case COOKIE:
                      {
                        Cookie* cookiep =(Cookie*)col_obj;
                        cookiep->die();
                        the_arena.get_object_lists()[COOKIE].remove( cookiep );
                      }
                      break;
                    case MINE:
                      {
                        Mine* minep =(Mine*)col_obj;
                        minep->die();
                        the_arena.get_object_lists()[MINE].remove( minep );
                      }
                      break;
                    default:
                      Error(true, "Shot on unknown object", "Robot::get_messages");
                    }
                }
              change_energy(-en * realtime_arena.get_shooting_penalty() );
            }
          break;
        case ACCELERATE:
          if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
          {
            double acc;
            *instreamp >> acc;
            acc = max( acc, the_opts.get_d(OPTION_ROBOT_MIN_ACCELERATION) );
            acc = min( acc, the_opts.get_d(OPTION_ROBOT_MAX_ACCELERATION) );
            acceleration = acc;
          }
          break;
        case BREAK:  // Included only for compatibility reasons
          send_message(WARNING, OBSOLETE_KEYWORD, msg_name);
        case BRAKE:
          if( check_state_for_message(msg_t, GAME_IN_PROGRESS) )
          {
            double brk;
            *instreamp >> brk;
            brk = max( brk, 0.0);
            brk = min( brk, 1.0);
            brake_percent = brk;
          } 
          break;
//          case LOAD_DATA:
//            if( check_state_for_message(msg_t, STARTING_ROBOTS) )
//              {
//                bool bin;
//                *instreamp >> bin;
//                load_data(bin);
//              }
//            break;
//          case SAVE_DATA_FINISHED:
//            if( check_state_for_message(msg_t, SHUTTING_DOWN_ROBOTS) )
//              {
//                send_message(EXIT_ROBOT);
//                send_signal();
//              }
//            break;
//          case BIN_DATA_FROM:
//            if( check_state_for_message(msg_t, SHUTTING_DOWN_ROBOTS) )
//              {
//                save_data(true, has_saved);
//                has_saved = true;
//              }
//            break;
//          case ASCII_DATA_FROM:
//            if( check_state_for_message(msg_t, SHUTTING_DOWN_ROBOTS) )
//              {
//                save_data(false, has_saved);
//                has_saved = true;
//              }
//            break;
        default:
          Error(true, "Message_type not implemented, " + (String)msg_name, "Robot::get_messages");
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

bool
Robot::check_state_for_message(const message_from_robot_type msg_t, const state_t state1, const state_t state2)
{
  if( the_arena.get_state() != state1 && the_arena.get_state() != state2 )
    {
      //cout << "Server: Warning sent for message: " << msg_name << "     State: " << the_arena.get_state() << endl;

      if( the_arena.get_state() != BEFORE_GAME_START )
        send_message(WARNING, MESSAGE_SENT_IN_ILLEGAL_STATE, 
                     message_from_robot[msg_t].msg);
      char buffer[80];
      instreamp->get(buffer, 80, '\n');
      return false;
    }
  
  return true;
}

//  void
//  Robot::save_data(const bool binary, const bool append)
//  {
//    cerr << "Sorry, save_data is broken at the moment!" << endl;
//    return;

//    String filename;

//  //    if( robot_name_uniqueness_number == 0 )
//  //      filename = the_gui.get_robotdir() + plain_robot_name + ".robotdata";
//  //    else
//  //      filename = the_gui.get_robotdir() + robot_name + ".robotdata";

//    int mode = _IO_OUTPUT;  
//    if( append ) mode = _IO_APPEND;

//    ofstream file(filename.chars(), mode);

//    if( !file ) Error(true, "Couldn't open file " + filename, "Robot::save_bin_data");

//    if( binary )
//      {
//        int bytes;
//        char c;
//        *instreamp >> bytes;

//        // TODO: check if data is avalable (peek), wait otherwise (select)
      
//        for(int i=0; i < bytes; i++)
//          {
//            instreamp->get(c);
//            file.put(c);
//          }
//      }
//    else
//      {
//        char buf[256];
      
//        instreamp->get(buf, 256 ,'\n');
//        file << buf << endl;

//        // TODO: continue saving if next message is SAVE_DATA
//      }
//  }

//  void
//  Robot::load_data(const bool binary)
//  {
//    cerr << "Sorry, load_data is broken at the moment!" << endl;
//    return;

//    String filename;

//    ifstream file;


//  //    if( robot_name_uniqueness_number == 0 )
//  //      {
//  //        filename = the_gui.get_robotdir() + "RobotSave/" + plain_robot_name + ".robotdata";
//  //        file.open(filename.chars());
//  //      }
//  //    if( !file )
//  //      {
//  //        filename = the_gui.get_robotdir() + "RobotSave/" + robot_name + ".robotdata";
//  //        file.open(filename.chars());
//  //      }

//    if (!file) 
//      {
//        send_message(LOAD_DATA_FINISHED);
//        return;
//      }

//    char buf[256];
//    if( binary )
//      {
//        char c;
//        int i;
//        while( !file.eof() )
//          {
//            for(i=0; i < 255 && file.get(c) ; i++)
//              buf[i] = c;

//            buf[i] = '\0';
//            send_message(BIN_DATA_TO, i, buf);
//          }
//      }
//    else
//      {
//        while( !file.eof() )
//          {
          
//            file.get(buf, 256 ,'\n');
//            send_message(ASCII_DATA_TO, buf);
//          }
//      }

//    send_message(LOAD_DATA_FINISHED);
//  }

void
Robot::change_energy(const double energy_diff)
{
  energy = min(energy+energy_diff, the_opts.get_d(OPTION_ROBOT_MAX_ENERGY));
#ifndef NO_GRAPHICS  
  if( !no_graphics )  display_score();
#endif
  if( energy <= 0.0 ) die();
}

#ifndef NO_GRAPHICS

void
Robot::reset_last_displayed()
{
  last_displayed_energy = -1;
  last_displayed_place = 0;
  last_displayed_last_place = 0;
  last_displayed_score = -1;
}

void
Robot::display_score()
{
  if( last_displayed_energy != (int)energy )
    {
      last_displayed_energy = (int)energy;
      gtk_clist_set_text(GTK_CLIST(the_gui.get_scorewindow_p()->get_clist()),
                         row_in_score_clist,
                         2, String((int)energy).non_const_chars());
    }

  if( last_displayed_place != position_this_game )
    {
      last_displayed_place = position_this_game;
      gtk_clist_set_text(GTK_CLIST(the_gui.get_scorewindow_p()->get_clist()),
                         row_in_score_clist,
                         3, String(position_this_game).non_const_chars());
    }

  if(get_last_position() != 0)
    if( last_displayed_last_place != get_last_position() )
      {
        last_displayed_last_place = get_last_position();
        gtk_clist_set_text(GTK_CLIST(the_gui.get_scorewindow_p()->get_clist()),
                           row_in_score_clist,
                           4, String(get_last_position()).non_const_chars());
      }

  
  if( last_displayed_score != (int)(10 * get_total_points()) )
    {
      last_displayed_score = (int)(10 * get_total_points());
      gtk_clist_set_text(GTK_CLIST(the_gui.get_scorewindow_p()->get_clist()),
                         row_in_score_clist,
                         5, String(get_total_points()).non_const_chars());
    }
}

void
Robot::draw_radar_and_cannon()
{
  double scale = the_gui.get_arenawindow_p()->get_drawing_area_scale();

  if( radius*scale < 2.5 ) return;
  // Draw Cannon
  the_gui.get_arenawindow_p()->
    draw_line( center,
               angle2vec(cannon_angle.pos+robot_angle.pos),
               radius - the_opts.get_d(OPTION_SHOT_RADIUS) - 1.0 / scale,
               the_opts.get_d(OPTION_SHOT_RADIUS),
               *(the_gui.get_fg_gdk_colour_p()) );

  // Draw radar lines
  Vector2D radar_dir = angle2vec(radar_angle.pos+robot_angle.pos);
  the_gui.get_arenawindow_p()->
    draw_line( center - radius * 0.25 * radar_dir,
               rotate( radar_dir, M_PI / 4.0 ),
               radius / 1.5,
               radius / 20.0,
               *(the_gui.get_fg_gdk_colour_p()) );
  the_gui.get_arenawindow_p()->
    draw_line( center - radius * 0.25 * radar_dir,
               rotate( radar_dir, - (M_PI / 4.0) ),
               radius / 1.5,
               radius / 20.0,
               *(the_gui.get_fg_gdk_colour_p()) );

  // Draw robot angle line
  the_gui.get_arenawindow_p()->
    draw_line( center,
               angle2vec(robot_angle.pos),
               radius * 0.9 - 2.0 / scale,
               *(the_gui.get_fg_gdk_colour_p()) );
}

void
Robot::get_score_pixmap( GdkWindow* win, GdkPixmap*& pixm, GdkBitmap*& bitm )
{
  score_pixmap.get_pixmap( gdk_colour, win, pixm, bitm ); 
}

void
Robot::get_stat_pixmap( GdkWindow* win, GdkPixmap*& pixm, GdkBitmap*& bitm )
{
  stat_pixmap.get_pixmap( gdk_colour, win, pixm, bitm ); 
}

#endif NO_GRAPHICS
