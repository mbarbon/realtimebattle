
#ifndef __MOVING_OBJECT__
#define __MOVING_OBJECT__

#include <fstream.h>
#include <glib.h>
#include "Arena.h"
#include "Shape.h"

// ----------------  MovingObject : ArenaObject  ---------------------

class MovingObject : public virtual ArenaObject
{
public:
  MovingObject() {}
  MovingObject(const Vector2D& vel) : velocity(vel) {}
  ~MovingObject() {}

  virtual void move(const double timestep) = 0;

protected:
  Vector2D velocity;
  
};

// ---------------------  Robot : MovingObject  ---------------------

struct stat_t
{
  stat_t(int s, int g, int pos, double p, double  t, double tp) :
    sequence_nr(s), game_nr(g), position(pos), points(p), time_survived(t), total_points(tp) {}
  int sequence_nr;
  int game_nr;
  int position;
  double points;
  double time_survived;
  double total_points;
};

enum rotation_mode_t { NORMAL_ROT, SWEEP_LEFT, SWEEP_RIGHT, ROTATE_TO_RIGHT, ROTATE_TO_LEFT };

struct rotation_t
{
  rotation_t() {}
  rotation_t(const double p, const double v, const double l, const double r, const rotation_mode_t m) :
    pos(p), vel(v), left(l), right(r), mode(m) {}
  void set(const double p, const double v, const double l, const double r, const rotation_mode_t m) 
    { pos=p; vel=v; left=l; right=r; mode=m; }
  double pos;
  double vel;
  double left;
  double right;
  rotation_mode_t mode;
};

class Robot : public virtual MovingObject, public virtual Circle
{
public:
  Robot(const String& filename);   
  ~Robot();
  
  void move(const double timestep);  
  void change_velocity(const double timestep);  
  void update_radar_and_cannon(const double timestep);  
  void update_rotation(rotation_t& angle, const double timestep);
  friend void bounce_on_wall(class Robot& robot, const class Wall& wall, const Vector2D& normal);
  friend void bounce_on_robot(Robot& robot1, Robot& robot2, const Vector2D& normal);
  void change_energy(const double energy_diff);
  void check_name_uniqueness();
  void get_messages();
  void send_message(enum message_to_robot_type ...);
  void set_initial_values(const Vector2D& pos, double angle);
  void set_stats(int robots_killed_same_time);
  void start_process();
  bool is_process_running();
  void send_signal();
  void end_process();
  void delete_pipes();
  void kill_process_forcefully();
  void live();
  void die();
 
  object_type get_object_type() { return ROBOT; }
  String get_robot_name() { return robot_name; }
  bool is_alive() { return alive; }
  double get_energy() { return energy; }
  pid_t get_pid() { return pid; }
  GList* get_statistics() { return statistics; }
  ofstream* get_outstreamp() { return outstreamp; }  
  int get_position_this_game() { return position_this_game; }
  double get_total_points() { return total_points; }
  void add_points(double pts) { points_this_game += pts; total_points += pts; }
  int get_last_position();
  rotation_t get_robot_angle() { return robot_angle; }
  void display_energy();
  void display_place();
  void display_last();
  void display_score();
  void draw_radar_and_cannon();

private:
  message_from_robot_type name2msg_from_robot_type(char*);
  void move(const double timestep, int iterstep);

  bool alive;
  bool process_running;

  bool send_usr_signal;

  double energy; 
  double extra_air_resistance;
  double break_percent;

  rotation_t robot_angle;
  rotation_t cannon_angle;
  rotation_t radar_angle;

  double acceleration;
  double shot_energy;

  Vector2D last_drawn_robot_center;
  double last_drawn_radar_angle;
  double last_drawn_cannon_angle;

  GList* statistics;

  int robot_name_uniqueness_number;
  String plain_robot_name;
  String robot_name;

  String robot_filename;
  //String robot_dir;
  double total_points;
  double points_this_game;
  int position_this_game;

  ifstream* instreamp;
  ofstream* outstreamp;
  int pipes[2];
  pid_t pid;    
};

// ---------------------  Shot : MovingObject  ---------------------

class Shot : public virtual MovingObject, public virtual Circle
{
public:
  Shot(const Vector2D& c, const double r, 
       const Vector2D& velocity, const double energy); 
  ~Shot() {}

  void move(const double timestep);
  void die();
  bool is_alive() { return alive; }
  double get_energy() { return energy; }

  object_type get_object_type() { return SHOT; }

private:
  bool alive;
  double energy;
};

#endif __MOVING_OBJECT__