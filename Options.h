
#ifndef __OPTIONS__
#define __OPTIONS__

#include <gtk/gtk.h>
#include "String.h"
#include "Various.h"

enum option_double_t
{
  OPTION_GRAV_CONST,
  OPTION_AIR_RESISTANCE,
  OPTION_ROLL_FRICTION,
  OPTION_SLIDE_FRICTION,
  OPTION_MAX_ACCELERATION,
  OPTION_MIN_ACCELERATION,

  OPTION_ROBOT_RADIUS,
  OPTION_ROBOT_MASS,
  OPTION_ROBOT_BOUNCE_COEFF,
  OPTION_ROBOT_HARDNESS,
  OPTION_ROBOT_PROTECTION,

  OPTION_ROBOT_START_ENERGY,
  OPTION_ROBOT_MAX_ENERGY,

  OPTION_ROBOT_FRONT_PROTECTION,
  OPTION_ROBOT_FRONT_HARDNESS,
  OPTION_ROBOT_FRONT_BOUNCE_COEFF,
  OPTION_ROBOT_COS_FRONTSIZE,

  OPTION_ROBOT_MAX_ROTATE,
  OPTION_ROBOT_CANNON_MAX_ROTATE,
  OPTION_ROBOT_RADAR_MAX_ROTATE,

  OPTION_SHOT_RADIUS,
  OPTION_SHOT_SPEED,
  OPTION_SHOOTING_PENALTY,
  OPTION_SHOT_MIN_ENERGY,
  OPTION_SHOT_MAX_ENERGY,
  OPTION_SHOT_ENERGY_INCREASE_SPEED,

  OPTION_COOKIE_MAX_ENERGY,
  OPTION_COOKIE_MIN_ENERGY,
  OPTION_COOKIE_FREQUENCY,
  OPTION_COOKIE_RADIUS,

  OPTION_MINE_MAX_ENERGY,
  OPTION_MINE_MIN_ENERGY,
  OPTION_MINE_FREQUENCY,
  OPTION_MINE_RADIUS,

  OPTION_MAX_TIMESTEP,
  OPTION_TIMEOUT,

  LAST_DOUBLE_OPTION
};

enum option_long_t
{
  OPTION_BACKGROUND_COLOUR,
  OPTION_FOREGROUND_COLOUR,
  OPTION_COOKIE_COLOUR,
  OPTION_MINE_COLOUR,

  LAST_LONG_OPTION
};

enum option_string_t
{
  LAST_STRING_OPTION
};

enum option_bool_t
{
  LAST_BOOL_OPTION
};

template<class T>
struct option_info_t
{
  option_info_t() {}
  option_info_t(const entry_datatype_t d, const T v, const double mn, const double mx,
                const int l, const String& s, GtkWidget * e ) :
    datatype(d), value(v), min_value(mn), max_value(mx), max_letters_in_entry(l), label(s), entry(e) {}
  entry_datatype_t datatype;
  T value;
  double min_value;
  double max_value;
  int max_letters_in_entry;
  String label;
  GtkWidget * entry;
};

class Options
{
public:
  Options();
  ~Options();
  //static void set_options();

private: // options
  double grav_const;
  double air_resistance;
  double roll_friction;
  double slide_friction;
  double max_acceleration;
  double min_acceleration;

  double robot_radius;
  double robot_mass;
  double robot_bounce_coeff;
  double robot_hardness;
  double robot_protection;

  double robot_start_energy;
  double robot_max_energy;

  double robot_front_protection;
  double robot_front_hardness;
  double robot_front_bounce_coeff;
  double robot_cos_frontsize;

  double robot_max_rotate;
  double robot_cannon_max_rotate;
  double robot_radar_max_rotate;

  double shot_radius;
  double shot_speed;
  double shooting_penalty;
  double shot_min_energy;
  double shot_max_energy;
  double shot_energy_increase_speed;

  long background_colour;
  long foreground_colour;

  double cookie_max_energy;
  double cookie_min_energy;
  double cookie_frequency;
  double cookie_radius;
  long cookie_colour;

  double mine_max_energy;
  double mine_min_energy;
  double mine_frequency;
  double mine_radius;
  long mine_colour;
  
  double max_timestep;
  double timeout;

public: // get_options
  double get_grav_const() { return grav_const; }
  double get_air_resistance() { return air_resistance; }
  double get_roll_friction() { return roll_friction; }
  double get_slide_friction() { return slide_friction; }
  double get_max_acceleration() { return max_acceleration; }
  double get_min_acceleration() { return min_acceleration; }

  double get_robot_radius() { return robot_radius; }
  double get_robot_mass() { return robot_mass; }
  double get_robot_bounce_coeff() { return robot_bounce_coeff; } 
  double get_robot_hardness() { return robot_hardness; }
  double get_robot_protection() { return robot_protection; }

  double get_robot_start_energy() { return robot_start_energy; }
  double get_robot_max_energy() { return robot_max_energy; }

  double get_robot_front_protection() { return robot_front_protection; }
  double get_robot_front_hardness() { return robot_front_hardness; }
  double get_robot_front_bounce_coeff() { return robot_front_bounce_coeff; }
  double get_robot_cos_frontsize() { return robot_cos_frontsize; }

  double get_robot_max_rotate() { return robot_max_rotate; } 
  double get_robot_cannon_max_rotate() { return robot_cannon_max_rotate; }
  double get_robot_radar_max_rotate() { return robot_radar_max_rotate; }

  double get_shot_radius() { return shot_radius; }
  double get_shot_speed() { return shot_speed; }
  double get_shooting_penalty() { return shooting_penalty; }
  double get_shot_min_energy() { return shot_min_energy; }
  double get_shot_max_energy() { return shot_max_energy; }
  double get_shot_energy_increase_speed() { return shot_energy_increase_speed; }

  long get_background_colour() { return background_colour; }
  long get_foreground_colour() { return foreground_colour; }

  double get_cookie_max_energy() { return cookie_max_energy; }
  double get_cookie_min_energy() { return cookie_min_energy; }
  double get_cookie_frequency() { return cookie_frequency; }
  double get_cookie_radius() { return cookie_radius; }
  long get_cookie_colour() { return cookie_colour; }
  
  double get_mine_max_energy() { return mine_max_energy; }
  double get_mine_min_energy() { return mine_min_energy; }
  double get_mine_frequency() { return mine_frequency; }
  double get_mine_radius() { return mine_radius; }
  long get_mine_colour() { return mine_colour; }

  double get_max_timestep() { return max_timestep; }
  double get_timeout() { return timeout; }

  double get_option_d( int option );
  long get_option_l( int option );
  String get_option_s( int option );
  bool get_option_b( int option );

  set_option( double v );
  set_option( long v );
  set_option( String v );
  set_option( bool v );

private:  // other private variables and functions

  option_info_t<double> * all_double_options[LAST_DOUBLE_OPTION];
  option_info_t<long> * all_long_options[LAST_LONG_OPTION];

  // No char or bool options yet
  //  option_info_t<String> * all_char_options[LAST_STRING_OPTION];
  //  option_info_t<bool> * all_bool_options[LAST_BOOL_OPTION];

  bool options_window_up;
  GtkWidget * options_window;

public:  // other public variables and functions
  void broadcast_opts();
  void setup_options_window();
  void close_options_window();
  void get_options_from_rtbrc();
  bool get_options_window_up() { return options_window_up; }
};
#endif __OPTIONS__
