
#ifndef __ARENA__
#define __ARENA__

#include <glib.h>
#include <gdk/gdk.h>
#include <fstream.h>
#include <stdlib.h>
#include "Vector2D.h"
#include "messagetypes.h"
#include "Options.h"
#include "gui.h"
#include "Various.h"

static const double infinity = 1.0e10;
static const double eps = 1.0e-2;

extern class Options the_opts;
extern class Arena the_arena;
extern class Gui the_gui;


// --------------------------  Arena --------------------------
class Arena 
{
public:
  enum state_t { NOT_STARTED, STARTING_ROBOTS, GAME_IN_PROGRESS, 
                 SHUTTING_DOWN_ROBOTS, FINISHED, EXITING };
  

  Arena();
  ~Arena();

  void clear();

  double get_shortest_distance(const Vector2D& pos, const Vector2D& vel, 
                               const double size, object_type& closest_shape, 
                               void*& colliding_object );

  bool space_available(const Vector2D& pos, const double margin);

  gint timeout_function();
  void start_tournament(const GList* robotfilenamelist, 
                        const GList* arenafilenamelist, // NULL terminated lists
                        int robots_p_game, 
                        int games_p_sequence,
                        int number_of_sequences);   

  void broadcast(enum message_to_robot_type ...);
  void set_colours();
  long find_free_colour(const long home_colour, const long away_colour, const class Robot*);
  void quit_ordered();
  void delete_lists(bool kill_robots, bool del_seq_list, bool del_tourn_list);

  Vector2D get_random_position();

  GList** get_object_lists() { return object_lists; }
  GList* get_all_robots_in_sequence() { return all_robots_in_sequence; }
  GList* get_all_robots_in_tournament() { return all_robots_in_tournament; }
  int get_sequence_nr() { return sequence_nr; }
  int get_games_per_sequence() { return games_per_sequence; }
  int get_games_remaining_in_sequence() { return games_remaining_in_sequence; }
  int get_sequences_remaining() { return sequences_remaining; }
  int get_robots_per_game() { return robots_per_game; }

  int get_robots_left() { return robots_left; }
  double get_total_time() { return (double)total_time; }
  double get_shooting_penalty() { return the_opts.get_shooting_penalty() / max(1.0, ((double)robots_left)/10.0); }
  GdkColor* get_background_colour_p() { return &background_colour; }
  GdkColor* get_foreground_colour_p() { return &foreground_colour; }
  state_t get_state() { return state; }
  Vector2D * get_boundary() { return boundary; }
  
private:
  void parse_file(istream&);
  void check_initialization();

  void update();
  void update_timer();
  void reset_timer();
  void update_robots();
  void move_shots();
  void update_explosions();

  void add_cookie();
  void add_mine();

  bool is_colour_allowed(const long colour, const double min_dist, const class Robot*);
  double colour_dist(const long col1, const GdkColor& col2);

  void start_game();
  void end_game();
  void start_sequence();
  void start_sequence_follow_up();
  void end_sequence();
  void end_sequence_follow_up();

  void end_tournament();

  GList* object_lists[number_of_object_types];
  
  GList* all_robots_in_tournament;
  GList* all_robots_in_sequence;
  GList* exclusion_points;
  GList* arena_filenames;               // list of GStrings

  int** robots_in_sequence;
  
  gdouble timestep;
  gdouble total_time;
  gdouble current_timer;
  gdouble timescale;

  GTimer* timer;
  
  gdouble next_check_time;

  int sequence_nr;
  int games_remaining_in_sequence;
  int games_per_sequence;
  int sequences_remaining;

  int number_of_arenas;
  int current_arena_nr;

  int number_of_robots;
  int robots_left;
  int robots_per_game;

  GdkColor background_colour;
  GdkColor foreground_colour;

  Vector2D boundary[2];   // {top-left, bottom-right}
  state_t state;
};

// ---------------------  ArenaObject ---------------------

class ArenaObject
{
public:
  ArenaObject() {}
  virtual ~ArenaObject() {}
  virtual object_type get_object_type() = 0;
  //  class Error;
protected:
};

// ---------------------  Explosion : ArenaObject  -------------------

class Explosion : public virtual ArenaObject
{
public:
  Explosion(const Vector2D& c, const double r, const double energy); 
  ~Explosion() {}

  object_type get_object_type() { return EXPLOSION; }
private:

};

//---------------------  Shape ---------------------

class Shape
{
public:
  Shape() {colour = make_gdk_colour( 0x000000 ); }
  Shape(long col) { colour = make_gdk_colour( col ); }

  virtual ~Shape() {}

  virtual double get_distance(const Vector2D& pos, const Vector2D& vel, 
                              const double size) = 0;
  virtual bool within_distance(const Vector2D& pos, const double size) = 0;
  virtual Vector2D get_normal(const Vector2D& pos) = 0;
  virtual void draw_shape(bool erase) = 0;
  //virtual void get_args(istream&) = 0;

  GdkColor get_colour() { return colour; }

  friend void bounce_on_wall(class Robot& robot, const Shape& wall, const Vector2D& normal);

protected:
  double bounce_coeff;
  double hardness_coeff;
  double mass;
  GdkColor colour;
};


// ---------------------  Line : Shape ---------------------

class Line : public virtual Shape
{
public:
  Line();
  Line(const Vector2D& sp, const Vector2D& d, const double len, const double th);
  Line(const Vector2D& sp, const Vector2D& d, const double len, const double th, 
       const double b_c, const double hardn);
  ~Line() {}

  double get_distance(const Vector2D& pos, const Vector2D& vel, const double size);
  bool within_distance(const Vector2D& pos, const double size);
  Vector2D get_normal(const Vector2D& pos);
  void draw_shape(bool erase);

  Vector2D get_start_point() { return start_point; }
  Vector2D get_direction() { return direction; }
  double get_length() { return length; }
  double get_thickness() { return thickness; }

protected:
  Vector2D start_point;
  Vector2D direction;
  double length;
  double thickness;

  Vector2D last_drawn_start_point;
  Vector2D last_drawn_direction;
  double last_drawn_length;
  double last_drawn_thickness;
};

// ---------------------  Circle : Shape ---------------------

class Circle : public virtual Shape 
{
public:
  Circle();
  Circle(const Vector2D& c, const double r); 
  Circle(const Vector2D& c, const double r, const double b_c, const double hardn);
  ~Circle() {}

  double get_distance(const Vector2D& pos, const Vector2D& dir, const double size);
  bool within_distance(const Vector2D& pos, const double size);
  Vector2D get_normal(const Vector2D& pos);
  void draw_shape(bool erase);
  
  double get_radius() { return radius; }
  Vector2D get_center() { return center; }
  //void add_to_center(const Vector2D& diff) { center += diff; }
  
protected:
  Vector2D center;
  double radius;

  Vector2D last_drawn_center;
  double last_drawn_radius;
  //  double radiussqr;
};

// ---------------------  InnerCircle : Shape ---------------------

class InnerCircle : public virtual Shape 
{
public:
  InnerCircle();
  InnerCircle(const Vector2D& c, const double r); 
  InnerCircle(const Vector2D& c, const double r, const double b_c, const double hardn);
  ~InnerCircle() {}

  double get_distance(const Vector2D& pos, const Vector2D& dir, const double size);
  bool within_distance(const Vector2D& pos, const double size);
  Vector2D get_normal(const Vector2D& pos);
  void draw_shape(bool erase);
  
  double get_radius() { return radius; }
  Vector2D get_center() { return center; }
  //void add_to_center(const Vector2D& diff) { center += diff; }
  
protected:
  Vector2D center;
  double radius;

  Vector2D last_drawn_center;
  double last_drawn_radius;
  //  double radiussqr;
};

// ---------------------  Wall : ArenaObject ---------------------

class Wall : public virtual ArenaObject 
{
public:
  object_type get_object_type() { return WALL; }
};

class WallCircle : public virtual Wall, public virtual Circle
{
public:
  WallCircle(const Vector2D& c, const double r, 
             const double b_c, const double hardn) : Circle(c, r, b_c, hardn) {}
  ~WallCircle() {}
};

class WallInnerCircle : public virtual Wall, public virtual InnerCircle
{
public:
  WallInnerCircle(const Vector2D& c, const double r, 
             const double b_c, const double hardn) : InnerCircle(c, r, b_c, hardn) {}
  ~WallInnerCircle() {}
};

class WallLine : public virtual Wall, public virtual Line
{
public:
  WallLine(const Vector2D& sp, const Vector2D& d, const double len, 
           const double th, const double b_c, const double hardn) 
    : Line(sp, d, len, th, b_c, hardn) {}
  ~WallLine() {}
};


// ---------------------  Extras : ArenaObject  ---------------------

class Extras : public virtual ArenaObject
{
public:
  //Extras(const Vector2D& c, const double r); 
  //~Extras() {}
  
};

// ---------------------  Cookie : Extras  ---------------------

class Cookie : public virtual Extras, public virtual Circle
{
public:
  Cookie(const Vector2D& c, const double r, const double e); 
  ~Cookie() {}
  
  object_type get_object_type() { return COOKIE; }
  bool is_alive() { return alive; }
  void die();
  double get_energy() { return energy; }

private:
  double energy;
  bool alive;
};

// ---------------------  Mine : Extras  ---------------------

class Mine : public virtual Extras, public virtual Circle
{
public:
  Mine(const Vector2D& c, const double r, const double e); 
  ~Mine() {}

  object_type get_object_type() { return MINE; }
  bool is_alive() { return alive; }
  void die();
  double get_energy() { return energy; }

private:
  double energy;
  bool alive;
};


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
  rotation_t(double p, double v, double l, double r, rotation_mode_t m) :
    pos(p), vel(v), left(l), right(r), mode(m) {}
  void set(double p, double v, double l, double r, rotation_mode_t m) 
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
  Robot(char* filename);
  //Robot(const Vector2D& c, const double r, char* filename); 
  ~Robot();
  
  void move(const double timestep);  
  void change_velocity(const double timestep);  
  void update_radar_and_cannon(const double timestep);  
  void update_rotation(rotation_t& angle, const double timestep);
  friend void bounce_on_wall(class Robot& robot, const Shape& wall, const Vector2D& normal);
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
  char* get_robotname() { return robot_name.str; }
  bool is_alive() { return alive; }
  double get_energy() { return energy; }
  pid_t get_pid() { return pid; }
  GList* get_statistics() { return statistics; }
  int get_position_this_game() { return position_this_game; }
  double get_total_points();
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

  double protection_coeff;

  GList* statistics;

  int robot_name_uniqueness_number;
  GString plain_robot_name;
  GString robot_name;

  GString robot_filename;
  GString robot_dir;
  int points;
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

#endif __ARENA__
