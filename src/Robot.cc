/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2000  Erik Ouchterlony and Ragnar Ouchterlony

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
# include <config.h>
#endif

#include <signal.h>
#include <iostream.h>
#include <math.h>
#include <string>

#include "Robot.h"
#include "ArenaController.h"
#include "Arena.h"
#include "Various.h"
#include "OptionHandler.h"
#include "Wall.h"
#include "Shot.h"
#include "String.h"
#include "EventHandler.h"
#include "Rotation.h"
#include "Process.h"


Robot::Robot(const string& filename)
{
  //  velocity = Vector2D(0.0, 0.0);
  //  acceleration = 0.0;

  robot_filename = filename;  

  plain_robot_name = "";
  robot_name = "";
  robot_name_uniqueness_number = 0;

  send_rotation_reached = 0;
  killed = true;

  id = 0;//the_arena.increase_robot_count();

  process = NULL;
}

// Constructor used by ArenaReplay. No process needed.
//
Robot::Robot(const int r_id, const long int col, const string& name)
{
  id = r_id;
  robot_name = name;
  set_colour( col );

  process = NULL;
  killed = true;

  radius = the_opts.get_d(OPTION_ROBOT_RADIUS);
}


Robot::~Robot()
{
  if( process != NULL )
    {
      delete process;
    }
} 

void
Robot::check_name_uniqueness()
{
//    int first_avail = 0;

//    robot_name = plain_robot_name;
  
//    vector<Robot>::iterator li;
//    for( li = the_arena.get_all_robots_in_tournament()->begin();
//         li != the_arena.get_all_robots_in_tournament()->end(); li++ )
//      {
//        if( &(*li) != this && plain_robot_name == (*li).plain_robot_name )
//          {
//            if( (*li).robot_name_uniqueness_number == 0 )
//              {
//                (*li).robot_name_uniqueness_number = 1;
//                (*li).robot_name += "(1)";
//              }

//            first_avail = max( (*li).robot_name_uniqueness_number + 1, first_avail );

//            if( robot_name_uniqueness_number == (*li).robot_name_uniqueness_number 
//                || robot_name_uniqueness_number == 0 )
//              robot_name_uniqueness_number = first_avail;
//          }
//      }

//    if( robot_name_uniqueness_number > 0 )
//      robot_name += ('(' + int2string(robot_name_uniqueness_number) + ')');
}



//
// Note that 'angle' is _not_ relative to the robot
//
double
Robot::get_bounce_coeff( const double angle )
{
  if( cos(angle - robot_angle.angle ) > cos(the_opts.get_d(OPTION_ROBOT_FRONTSIZE)) )
    return the_opts.get_d(OPTION_ROBOT_FRONT_BOUNCE_COEFF);
  else
    return the_opts.get_d(OPTION_ROBOT_BOUNCE_COEFF);
}

double
Robot::get_hardness_coeff( const double angle )
{
  if( cos(angle - robot_angle.angle ) > cos(the_opts.get_d(OPTION_ROBOT_FRONTSIZE)) )
    return the_opts.get_d(OPTION_ROBOT_FRONT_HARDNESS);
  else
    return the_opts.get_d(OPTION_ROBOT_HARDNESS);
}

void
Robot::bounce_on_wall(const double bounce_c, const double hardness_c, const Vector2D& normal)
{
  double angle = vec2angle(-normal);

  double e = get_bounce_coeff( angle ) * bounce_c;
  double h = get_hardness_coeff( angle ) * hardness_c;

  Vector2D start_vel = velocity;  
  velocity -= (1.0 + e) * dot(normal, velocity) * normal;

  injury_from_collision( 0.5 * get_mass() * h * lengthsqr(start_vel - velocity), angle );
}

void
Robot::set_values_before_game(const Vector2D& pos, const double angle)
{
  center = pos;
  robot_angle.set_rot (angle, 0.0, -DBL_MAX, DBL_MAX, NORMAL_ROT);
  radius = the_opts.get_d(OPTION_ROBOT_RADIUS);
  health = the_opts.get_d(OPTION_ROBOT_START_HEALTH);
  velocity = Vector2D(0.0, 0.0);
  brake_percentage = 0.0;
  acceleration = Vector2D(0.0, 0.0);
}

void
Robot::start_process()
{
  if( process != NULL )
    delete process;

  process = new Process( robot_filename, this );
}

void
Robot::set_values_at_process_start_up()
{
//    time_survived_in_sequence = 0.0;

//    if( statistics.empty() )       // first sequence !
//      {
//        send_message(INITIALIZE, 1);  
//        colour_given = false;
//        name_given = false;
//      }
//    else        // not first sequence !
//      {
//        send_message(INITIALIZE, 0);
//        send_message(YOUR_NAME, robot_name.c_str());
//        int long col = rgb_colour;
//        int long newcol = the_arena.find_free_colour(col, col, this);
//        if( col != newcol ) set_colour( newcol );
//        // TODO: probably free color!
//        send_message(YOUR_COLOUR, newcol);
//      } 
}

void Robot::update( )
{
  double current_time = the_eventhandler.get_game_time();
  double timestep = current_time - last_update_time;
  last_update_time = current_time;


  update_velocity(timestep);
  robot_angle.update(timestep, send_rotation_reached);

  move(timestep);

  get_messages();  
}

//  void
//  Robot::update_velocity(const double timestep)
//  {
//    Vector2D dir = angle2vec(robot_angle.angle);
//    double gt = the_opts.get_d(OPTION_GRAV_CONST);
//    double slide_fric = the_opts.get_d(OPTION_SLIDE_FRICTION);
//    double fric = the_opts.get_d(OPTION_ROLL_FRICTION) * (1.0 - brake_percent) + 
//      slide_fric * brake_percent;

//    double air_res = the_opts.get_d(OPTION_AIR_RESISTANCE);

//    velocity = 
//      dot(velocity, dir) * max(0.0, 1.0-gt*fric*timestep) * dir +
//      -velocity* min(air_res * timestep, 0.5) +
//      timestep*acceleration + 
//      vedge(dir, velocity) * max(0.0, 1.0-gt*slide_fric*timestep) * rotate90(dir);
//  }

/*
void
Robot::move(const double timestep)
{
  move(timestep, 1, timestep / 50.0);

  if( is_alive() )
    the_arena.print_to_logfile('R', id, center[0], center[1],
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
*/

void
Robot::send_message(const message_to_robot_type msg_type ...)
{
  va_list args;
  va_start(args, msg_type);

  ofstream* pout = process->get_outstreamp();

  *pout << message_to_robot[msg_type].msg << " ";
  for(int i=0; i<message_to_robot[msg_type].number_of_args; i++)
    {
      switch(message_to_robot[msg_type].arg_type[i])
        {
        case NONE: 
          Error(true, "Couldn't send message, no arg_type", "Robot::send_message");
          break;
        case INT:
          *pout << va_arg(args, int) << " ";
          break;
        case DOUBLE:
          *pout << va_arg(args, double) << " ";
          break;
        case STRING:
          *pout << va_arg(args, char*) << " ";
          break;   
        case HEX:
          *pout << hex << va_arg(args, int) << " ";
          break;
        default:
          Error(true, "Couldn't send message, unknown arg_type", "Robot::send_message");
        }
    }
  *pout << endl;
}


// This is the function which gets messages from the robot,
// parses them and take appropriate action. It is certainly 
// too long, but who cares :-)
void
Robot::get_messages()
{
  char buffer[256];
  ifstream* pin = process->get_instreamp();

  *pin >> ws;
  pin->clear();
  pin->peek();
  while( !pin->eof() )
    {
      pin->get(buffer, 255, '\n');

      

      *pin >> ws;
      pin->clear();
      pin->peek();
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
//    if( the_arena.get_state() != state1 && the_arena.get_state() != state2 )
//      {
//        //cout << "Server: Warning sent for message: " << msg_name << "     State: " << the_arena.get_state() << endl;

//        if( the_arena.get_state() != BEFORE_GAME_START )
//          send_message(WARNING, MESSAGE_SENT_IN_ILLEGAL_STATE, 
//                       message_from_robot[msg_t].msg);
//        char buffer[80];
//        process->get_instreamp()->get(buffer, 80, '\n');
//        return false;
//      }
  
  return true;
}


void
Robot::change_health(const double health_diff)
{
  health = min(health+health_diff, the_opts.get_d(OPTION_ROBOT_MAX_HEALTH));
//  #ifndef NO_GRAPHICS  
//    if( !no_graphics )  display_score();
//  #endif
  if( health <= 0.0 ) killed = true;
}

void
Robot::injury_from_collision(const double en, const double angle)
{

}
