#include <glib.h>
#include <gdk/gdk.h>
#include <fstream.h>
#include <stdlib.h>
#include "Vector2D.h"
#include "messagetypes.h"

#ifndef __ARENA__
#define __ARENA__

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

static const double infinity = 1.0e10;

enum touch_type { NO_ACTION, BOUNCE, TRANSFORM };
enum object_type { ROBOT, SHOT, WALL, COOKIE, MINE, EXPLOSION };
static const number_of_object_types = 6;



// --------------------------  Arena --------------------------
class Arena 
{
public:
  Arena();
  ~Arena();



  double get_shortest_distance(const Vector2D& pos, const Vector2D& dir, 
                               const double size, class Shape* closest_shape);

  gint timeout_function();
  
  void start_game(int arenanr);
  void end_game();
  void start_sequence(int first_arena);
  void end_sequence();
  void start_tournament(char**robotfilenamelist, char** arenafilenamelist, // NULL terminated lists
                        int robots_per_game); 
  void end_tournament();

  enum state_t { NOT_STARTED, STARTING_ROBOTS, GAME_IN_PROGRESS, SHUTTING_DOWN_ROBOTS, FINISHED, EXITING };
  
private:
  double timestep;
  double total_time;
  GTimer timer;
  
  double next_check_time;

  void parse_file(istream&);
  void check_initialization();

  void update();
  void start_robot(char* filename);
  void update_robots();
  void move_shots();
  void update_explosions();

  void add_to_list(class ArenaObject&);
  void remove_from_list(class ArenaObject&);
  void add_to_solid_object_list(class Shape&);
  void remove_from_solid_object_list(class Shape&);

  GList object_lists[number_of_object_types];
  
  GList all_robots_in_tournament;
  GList all_robots_in_sequence;
  GList solid_objects;
  GList arena_filenames;               // list of GStrings
  
  int games_remaining_in_sequence;
  int sequences_remaining;
  int robots_left;
  int robots_per_game;
  state_t state;
  
  double air_resistance;
  double roll_friction;
  double slide_friction;
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

class Explosion : private ArenaObject
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

  virtual double get_distance(const Vector2D& pos, const Vector2D& dir, 
                              const double size) = 0;
  //virtual Vector2D get_normal(const Vector2D& pos, const Vector2D& dir, const double size) = 0;
  //virtual void get_args(istream&) = 0;

protected:
  touch_type touch_action;
  double bounce_coeff;
  GdkColor colour;
};


// ---------------------  Line : Shape ---------------------

class Line : public Shape
{
public:
  Line();
  Line(const Vector2D& sp, const Vector2D& d, const double len, const double th);
  ~Line() {}

  double get_distance(const Vector2D& pos, const Vector2D& dir, const double size);
  Vector2D get_normal(const Vector2D& pos, const Vector2D& dir, const double size);

protected:
  Vector2D start_point;
  Vector2D direction;
  double length;
  double thickness;
};

// ---------------------  Circle : Shape ---------------------

class Circle : public Shape 
{
public:
  Circle();
  Circle(const Vector2D& c, const double r); 
  ~Circle() {}

  double get_distance(const Vector2D& pos, const Vector2D& dir, const double size);
  Vector2D get_normal(const Vector2D& pos, const Vector2D& dir, const double size);
  
  // double get_radius() { return radius; }
  //Vector2D get_center() { return center; }
  //void add_to_center(const Vector2D& diff) { center += diff; }
  
protected:
  Vector2D center;
  double radius;
  //  double radiussqr;
};

// ---------------------  Wall : ArenaObject ---------------------

class Wall : protected ArenaObject 
{
public:
  object_type get_object_type() { return WALL; }
};

class WallCircle : public Wall, private Circle
{
public:
  WallCircle(const Vector2D& c, const double r); 
  ~WallCircle() {}
};

class WallLine : public Wall, private Line
{
public:
  WallLine(const Vector2D& sp, const Vector2D& d, const double len, const double th);
  ~WallLine() {}
};


// ---------------------  Extras : ArenaObject  ---------------------

class Robot;

class Extras : protected ArenaObject
{
public:
  //Extras(const Vector2D& c, const double r); 
  //~Extras() {}
  
  virtual int touch_action(class Robot&) = 0;
protected:
  Circle my_shape;

};

// ---------------------  Cookie : Extras  ---------------------

class Cookie : private Extras, private Circle
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

class Mine : private Extras, private Circle
{
public:
  Mine(const Vector2D& c, const double r, const double e); 
  ~Mine() {}

  int touch_action(class Robot&);
  object_type get_object_type() { return MINE; }
private:
};


// ----------------  MovingObject : ArenaObject  ---------------------

class MovingObject : protected ArenaObject
{
public:
  //MovingObject(const Vector2D& c, const double r); 
  //~MovingObject() {}

  virtual void move(const double timestep) = 0;

protected:
  //Circle my_shape;
  Vector2D velocity;
  
};

// ---------------------  Robot : MovingObject  ---------------------

class Robot : private MovingObject, private Circle
{
public:
  Robot(char* filename);
  Robot(const Vector2D& c, const double r, char* filename); 
  ~Robot();
  
  void move(const double timestep);  
  void update_radar_and_cannon(const double timestep);  
  void change_energy(const double energy_diff);
  void get_messages();
  void send_message(enum message_to_robot_type ...);
  void set_initial_position_and_direction(const Vector2D&, const Vector2D&);
  void start_process();
  int check_process_started();
  void end_process();
  int check_procees_finished();

  void set_colour( int red, int green, int blue );
  GdkColor get_colour() { return colour; }
  
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

  GdkColor colour;
  GString robot_name;
  GString robot_filename;
  GString robot_dir;
  int points;

  istream instream;
  ostream outstream;
  pid_t robot_pid;    
};

// ---------------------  Shot : MovingObject  ---------------------

class Shot : private MovingObject, private Circle
{
public:
  Shot(const Vector2D& c, const double r, 
       const Vector2D& velocity, const double energy); 
  ~Shot() {}

  void move(const double timestep);
  object_type get_object_type() { return SHOT; }
private:

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

