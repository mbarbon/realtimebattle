#include <glib.h>
#include <gdk/gdk.h>
#include <fstream.h>
#include <stdlib.h>
#include "Vector2D.h"
#include "messagetypes.h"
#include "gui.h"

#ifndef __ARENA__
#define __ARENA__

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

static const double infinity = 1.0e10;

enum touch_type { NO_ACTION, BOUNCE, TRANSFORM };
enum object_type { NOOBJECT, ROBOT, SHOT, WALL, COOKIE, MINE, EXPLOSION };
static const number_of_object_types = 6;



// --------------------------  Arena --------------------------
class Arena 
{
public:
  Arena();
  ~Arena();

  double get_shortest_distance(const Vector2D& pos, const Vector2D& vel, 
                               const double size, object_type& closest_shape, 
                               void* colliding_object );

  bool space_available(const Vector2D& pos, const double margin);

  gint timeout_function();
  void start_tournament(char**robotfilenamelist, 
                        char** arenafilenamelist, // NULL terminated lists
                        int robots_p_game, 
                        int games_p_sequence);   

  Vector2D get_random_position();
  GList** get_object_lists() { return object_lists; }
  Gui* get_the_gui() { return the_gui; }
  double get_max_acceleration() { return max_acceleration; }
  double get_shot_radius() { return shot_radius; }
  double get_shot_speed() { return shot_speed; }
  double get_robot_radius() { return robot_radius; }

  void add_to_list(class ArenaObject&);
  void remove_from_list(class ArenaObject&);
  void add_to_solid_object_list(class Shape&);
  void remove_from_solid_object_list(class Shape&);

  enum state_t { NOT_STARTED, STARTING_ROBOTS, GAME_IN_PROGRESS, 
                 SHUTTING_DOWN_ROBOTS, FINISHED, EXITING };
  
private:
  gdouble timestep;
  gdouble total_time;
  GTimer* timer;
  
  gdouble next_check_time;

  void parse_file(istream&);
  void check_initialization();

  void update();
  void update_timer();
  void update_robots();
  void move_shots();
  void update_explosions();
  
  
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
  GList* solid_objects;
  GList* arena_filenames;               // list of GStrings
  
  int games_remaining_in_sequence;
  int games_per_sequence;
  int sequences_remaining;
  int current_arena_nr;
  int robots_left;
  int robots_per_game;
  double robot_radius;
  double shot_radius;
  double shot_speed;
  state_t state;
  
  double air_resistance;
  double roll_friction;
  double slide_friction;
  double max_acceleration;
  Vector2D boundary[2];   // {top-left, bottom-right}

  Gui* the_gui;
};

// ---------------------  ArenaObject ---------------------

class ArenaObject
{
public:
  ArenaObject() {the_arena = NULL;}
  virtual ~ArenaObject() {}
  virtual object_type get_object_type() = 0;
  //  class Error;
protected:
  Arena* the_arena;
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
  Shape() {touch_action = BOUNCE;bounce_coeff = 0.5;}
  virtual ~Shape() {}

  virtual double get_distance(const Vector2D& pos, const Vector2D& vel, 
                              const double size) = 0;
  virtual bool within_distance(const Vector2D& pos, const double size) = 0;
  virtual Vector2D get_normal(const Vector2D& pos, const Vector2D& vel, 
                              const double size) = 0;
  virtual void draw_shape(Gui& the_gui) = 0;
  //virtual void get_args(istream&) = 0;

  void set_colour(const long);
  GdkColor get_colour() { return colour; }

protected:
  touch_type touch_action;
  double bounce_coeff;
  GdkColor colour;
};


// ---------------------  Line : Shape ---------------------

class Line : public virtual Shape
{
public:
  Line();
  Line(const Vector2D& sp, const Vector2D& d, const double len, const double th);
  ~Line() {}

  double get_distance(const Vector2D& pos, const Vector2D& vel, const double size);
  bool within_distance(const Vector2D& pos, const double size);
  Vector2D get_normal(const Vector2D& pos, const Vector2D& vel, const double size);
  void draw_shape(Gui& the_gui);

  Vector2D get_start_point() { return start_point; }
  Vector2D get_direction() { return direction; }
  double get_length() { return length; }
  double get_thickness() { return thickness; }

protected:
  Vector2D start_point;
  Vector2D direction;
  double length;
  double thickness;
};

// ---------------------  Circle : Shape ---------------------

class Circle : public virtual Shape 
{
public:
  Circle();
  Circle(const Vector2D& c, const double r); 
  ~Circle() {}

  double get_distance(const Vector2D& pos, const Vector2D& dir, const double size);
  bool within_distance(const Vector2D& pos, const double size);
  Vector2D get_normal(const Vector2D& pos, const Vector2D& dir, const double size);
  void draw_shape(Gui& the_gui);
  
  double get_radius() { return radius; }
  Vector2D get_center() { return center; }
  //void add_to_center(const Vector2D& diff) { center += diff; }
  
protected:
  Vector2D center;
  double radius;
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
  WallCircle(const Vector2D& c, const double r) : Circle(c, r) {}
  ~WallCircle() {}
};

class WallLine : public virtual Wall, public virtual Line
{
public:
  WallLine(const Vector2D& sp, const Vector2D& d, const double len, 
           const double th) : Line(sp, d, len, th) {}
  ~WallLine() {}
};


// ---------------------  Extras : ArenaObject  ---------------------

class Robot;

class Extras : public virtual ArenaObject
{
public:
  //Extras(const Vector2D& c, const double r); 
  //~Extras() {}
  
  virtual int touch_action(class Robot&) = 0;
};

// ---------------------  Cookie : Extras  ---------------------

class Cookie : public virtual Extras, public virtual Circle
{
public:
  Cookie(const Vector2D& c, const double r, const double e); 
  ~Cookie() {}
  
  int touch_action(class Robot&);
  object_type get_object_type() { return COOKIE; }
private:
  double energy;
};

// ---------------------  Mine : Extras  ---------------------

class Mine : public virtual Extras, public virtual Circle
{
public:
  Mine(const Vector2D& c, const double r, const double e); 
  ~Mine() {}

  int touch_action(class Robot&);
  object_type get_object_type() { return MINE; }
private:
};


// ----------------  MovingObject : ArenaObject  ---------------------

class MovingObject : public virtual ArenaObject
{
public:
  MovingObject::MovingObject() {}
  MovingObject(const Vector2D& vel) : velocity(vel) {}
  ~MovingObject() {}

  virtual void move(const double timestep) = 0;

protected:
  Vector2D velocity;
  
};

// ---------------------  Robot : MovingObject  ---------------------

class Robot : public virtual MovingObject, public virtual Circle
{
public:
  Robot(char* filename, Arena* ap);
  //Robot(const Vector2D& c, const double r, char* filename, Arena* ap); 
  ~Robot();
  
  void move(const double timestep);  
  void update_radar_and_cannon(const double timestep);  
  void bounce_on_wall(Wall* colliding_object);
  void bounce_on_robot(Robot* colliding_object);
  void change_energy(const double energy_diff);
  void get_messages();
  void send_message(enum message_to_robot_type ...);
  void set_initial_position_and_direction(const Vector2D& pos, double angle, double size);
  void start_process();
  bool is_process_running();
  void end_process();
  void kill_process_forcefully();
  void live();
  void die();
 
  object_type get_object_type() { return ROBOT; }
  char* get_robotname();
  bool is_alive() { return alive; }

private:
  message_from_robot_type name2msg_from_robot_type(char*);
  bool alive;
  bool process_running;
  double extra_air_resistance;
  double energy;
  double radar_angle;
  double radar_speed;
  double cannon_angle;
  double cannon_speed;
  double robot_angle;
  double robot_angle_speed;
  double acceleration;

  GString robot_name;
  GString robot_filename;
  GString robot_dir;
  int points;

  ifstream* instreamp;
  ofstream* outstreamp;
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

  object_type get_object_type() { return SHOT; }

private:
  bool alive;
  double energy;
};

class Error
{
public:
  Error() {}
  Error(char* strp, char* funcp);
  Error(char* strp, char* str2p, char* funcp);
  Error(char* strp, char ch, char* funcp);
  void print_message();
  
  friend class Arena;
  friend class ArenaObject;
private:

  GString* error_stringp;
  GString* error_string2p;
  GString* function_stringp;
  gchar c;
};

#endif __ARENA__
