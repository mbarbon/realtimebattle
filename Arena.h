#include <glib.h>
#include <fstream.h>
#include "Vector2D.h"
#include <stdlib.h>

#ifndef __ARENA__
#define __ARENA__

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

static const double infinity = 1.0e10;

enum touch_type { NO_ACTION, BOUNCE, TRANSFORM };
enum object_type { ROBOT, SHOT, WALL, COOKIE, MINE, EXPLOSION };
static const number_of_object_types = 6;

// ---------------------  ArenaObject ---------------------

class ArenaObject
{
public:
  virtual ~ArenaObject() {}
  virtual object_type get_object_type() = 0;
  //  class Error;
};

// ---------------------  Explosion : ArenaObject  -------------------

class Explosion : public ArenaObject
{
public:
  Explosion(const Vector2D& c, const double r, const double energy); 
  ~Explosion() {}

  object_type get_object_type() { return EXPLOSION; }
private:

};

// ---------------------  SolidObject ---------------------

// class SolidObject : public ArenaObject 
// {
// public:

// };

// ---------------------  Shape ---------------------

class Shape : public ArenaObject
{
public:
  virtual ~Shape() {}
  virtual double get_distance(const Vector2D& pos, const Vector2D& dir, const double size) = 0;
  //virtual Vector2D get_normal(const Vector2D& pos, const Vector2D& dir, const double size) = 0;
  //virtual void get_args(istream&) = 0;

private:
  touch_type touch_action;
  double bounce_coeff;
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
  object_type get_object_type() { return WALL; }

private:
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
  object_type get_object_type() { return WALL; }
  
private:
  Vector2D center;
  double radius;
  //  double radiussqr;
};

// ---------------------  Extras : ArenaObject  ---------------------

class Robot;

class Extras : public ArenaObject //, public Circle 
{
public:
  //Extras(const Vector2D& c, const double r); 
  //~Extras() {}
  
  virtual int touch_action(const class Robot&) = 0;
private:
  Circle my_shape;

};

// ---------------------  Cookie : Extras  ---------------------

class Cookie : public Extras
{
public:
  Cookie(const Vector2D& c, const double r, const double energy); 
  ~Cookie() {}
  
  int touch_action(const class Robot&);
  object_type get_object_type() { return COOKIE; }
private:

};

// ---------------------  Mine : Extras  ---------------------

class Mine : public Extras
{
public:
  Mine(const Vector2D& c, const double r, const double energy); 
  ~Mine() {}

  int touch_action(const class Robot&);
  object_type get_object_type() { return MINE; }
private:
};


// ----------------  MovingObject : ArenaObject  ---------------------

class MovingObject : public ArenaObject//, public Circle 
{
public:
  //MovingObject(const Vector2D& c, const double r); 
  //~MovingObject() {}

  virtual void move(const double timestep) = 0;

protected:
  Circle my_shape;
  Vector2D velocity;
  
};

// ---------------------  Robot : MovingObject  ---------------------

class Robot : private MovingObject
{
public:
  Robot(char* filename);
  Robot(const Vector2D& c, const double r, char* filename); 
  ~Robot();
  
  void move(const double& timestep);  
  void check_radar(double distance, object_type obj_type);
  char* get_message(char* readbuffer);
  void send_message(const char* message);

  object_type get_object_type() { return ROBOT; }
  char* get_robotname();

private:
  double extra_air_resistance;
  GString robot_name;
  GString robot_filename;
  GString robot_dir;
  istream instream;
  ostream outstream;
  pid_t robot_pid;  
};

// ---------------------  Shot : MovingObject  ---------------------

class Shot : private MovingObject
{
public:
  Shot(const Vector2D& c, const double r, const Vector2D& velocity, const double energy); 
  ~Shot() {}

  void move(const double timestep);
  object_type get_object_type() { return SHOT; }
private:

};

// --------------------------  Arena --------------------------


class Arena 
{
public:
  Arena(char* filename);
  ~Arena();

  double get_shortest_distance(const Vector2D& pos, const Vector2D& dir, const double size, int closest_shape);
  void update_robots();
  void move_shots();
  void update_explosions();
  void add_to_list(const class ArenaObject&);
  void remove_from_list(const class ArenaObject&);
  void add_to_solid_object_list(const class ArenaObject&);
  void remove_from_solid_object_list(const class ArenaObject&);
  void set_initial_position_and_direction(const Vector2D&, const Vector2D&);
  
private:
  void parse_file(istream&);

  GList object_lists[number_of_object_types];

  GList SolidsObjects;

  Shape** shapes;       // To be removed!
  int number_of_shapes; // -- "" ---

  double air_resistance;
  double roll_friction;
  double slide_friction;
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

