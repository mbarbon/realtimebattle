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

#ifndef __ROBOT__
#define __ROBOT__

#ifndef NO_GRAPHICS
# include <gdk/gdk.h>
#endif

#include <fstream.h>
#include <sys/types.h>
#include <unistd.h>


#include "Messagetypes.h"

#include "Structs.h"
#include "MovingObject.h"
#include "String.h"
#include "List.h"

class Robot : public MovingObject
{
public:
  Robot(const String& filename);
  Robot(const int r_id, const long int col, const String& name);
  ~Robot();

  //arenaobject_t get_arenaobject_t() { return ROBOT; }
  
  void move(const double timestep);  
  void change_velocity(const double timestep);  
  void update_radar_and_cannon(const double timestep);  
  bool update_rotation(rotation_t& angle, const double timestep);
  void bounce_on_wall(const double bounce_c, const double hardness_c, const Vector2D& normal);
  friend void bounce_on_robot(Robot& robot1, Robot& robot2, const Vector2D& normal);
  void change_energy(const double energy_diff);
  void change_position( const double x, const double y, 
                        const double robot_a, const double cannon_a, 
                        const double radar_a, const double energy );

  void check_name_uniqueness();
  void get_messages();
  void send_message(enum message_to_robot_type ...);
  void set_values_before_game(const Vector2D& pos, double angle);
  void set_values_at_process_start_up();
  void set_stats(int robots_killed_same_time);
  void start_process();
  bool is_process_running();
  void check_process();
  void send_signal();
  void end_process();
  void delete_pipes();
  void kill_process_forcefully();
  void live();
  void die();

  String get_robot_name() { return robot_name; }
  bool is_alive() { return alive; }
  double get_energy() { return energy; }
  pid_t get_pid() { return pid; }
  List<stat_t>* get_statistics() { return &statistics; }
  ofstream* get_outstreamp() { return outstreamp; }  
  int get_position_this_game() { return position_this_game; }
  double get_total_points() { return total_points; }
  void add_points(double pts) { points_this_game += pts; total_points += pts; }
  int get_last_position();
  rotation_t get_robot_angle() { return robot_angle; }

  bool set_and_get_has_competed() 
    { if( has_competed) return true; else { has_competed = true; return false; } }

#ifndef NO_GRAPHICS

  int get_row_in_score_clist() { return row_in_score_clist; }
  void get_score_pixmap( GdkWindow* win, GdkPixmap*& pixm, GdkBitmap*& bitm );
  void get_stat_pixmap( GdkWindow* win, GdkPixmap*& pixm, GdkBitmap*& bitm );

  void set_row_in_score_clist( int row ) { row_in_score_clist = row; }
  void reset_last_displayed();
  void display_score();
  void draw_radar_and_cannon();

#endif

private:
  message_from_robot_type name2msg_from_robot_type(char*);
  void move(const double timestep, int iterstep, const double eps);
  bool check_state_for_message(const message_from_robot_type msg_t, const enum state_t state1,
                               const enum state_t state2 = NO_STATE);
  //  void save_data(const bool bin, const bool rewrite);
  //  void load_data(const bool bin);
  

  bool alive;
  bool process_running;
  bool has_saved;

  bool send_usr_signal;
  int signal_to_send;
  int  send_rotation_reached;

  bool has_competed;

  double energy; 
  double extra_air_resistance;
  double brake_percent;

  rotation_t robot_angle;
  rotation_t cannon_angle;
  rotation_t radar_angle;

  double acceleration;
  double shot_energy;

  Vector2D last_drawn_robot_center;
  double last_drawn_radar_angle;
  double last_drawn_cannon_angle;

  List<stat_t> statistics;

  int robot_name_uniqueness_number;
  class String plain_robot_name;
  class String robot_name;

  class String robot_filename;
  //class String robot_dir;
  double total_points;
  double points_this_game;
  int position_this_game;
  double time_survived_in_sequence;

  double cpu_next_limit;
  double cpu_warning_limit;
  double cpu_timeout;

  ifstream* instreamp;
  ofstream* outstreamp;
  int pipes[2];
  pid_t pid;    

  int row_in_score_clist;

  int last_displayed_energy;
  int last_displayed_place;
  int last_displayed_last_place;
  long last_displayed_score;

#ifndef NO_GRAPHICS
  pixmap_t score_pixmap;
  pixmap_t stat_pixmap;
#endif
};

#endif __ROBOT__
