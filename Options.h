
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

  OPTION_ROBOT_FRONT_BOUNCE_COEFF,
  OPTION_ROBOT_FRONT_HARDNESS,
  OPTION_ROBOT_FRONT_PROTECTION,

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

  OPTION_ARENA_SCALE,

  OPTION_MAX_TIMESTEP,
  OPTION_TIMEOUT,

  LAST_DOUBLE_OPTION
};

enum option_long_t
{
  OPTION_ROBOT_ENERGY_LEVELS,

  OPTION_BACKGROUND_COLOUR,
  OPTION_FOREGROUND_COLOUR,
  OPTION_COOKIE_COLOUR,
  OPTION_MINE_COLOUR,

  LAST_LONG_OPTION
};

enum option_string_t
{
  OPTION_STATISTICS_SAVE_FILE,
  OPTION_OPTIONS_SAVE_FILE,
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
  option_info_t(const entry_datatype_t d, const T v, const T mn, const T mx,
                const int l, const String& s, GtkWidget * e ) :
    datatype(d), value(v), default_value(v), min_value(mn), max_value(mx), max_letters_in_entry(l), label(s), entry(e) {}
  entry_datatype_t datatype;
  T value;
  T default_value;
  T min_value;
  T max_value;
  int max_letters_in_entry;
  String label;
  GtkWidget * entry;
};

void options_window_requested(GtkWidget *widget, gpointer data);
void apply_options_requested(GtkWidget *widget, gpointer data);
void dump_options_requested(GtkWidget *widget, gpointer data);

void double_options_min_callback( GtkWidget * widget, option_info_t<double> * option );
void double_options_max_callback( GtkWidget * widget, option_info_t<double> * option );
void double_options_def_callback( GtkWidget * widget, option_info_t<double> * option );
void long_options_min_callback( GtkWidget * widget, option_info_t<long> * option );
void long_options_max_callback( GtkWidget * widget, option_info_t<long> * option );
void long_options_def_callback( GtkWidget * widget, option_info_t<long> * option );
void string_options_def_callback( GtkWidget * widget, option_info_t<String> * option );

class Options
{
public:
  Options();
  ~Options() {}
  //static void set_options();

  inline double get_d( option_double_t  option ) { return all_double_options[option].value; }
  inline long   get_l( option_long_t    option ) { return all_long_options[option].value; }
  inline const String get_s( option_string_t  option ) { return all_string_options[option].value.chars(); }
  //  inline bool get_b( option_bool_t option ) { return all_bool_options[option].value; }

  void broadcast_opts();
  void setup_options_window();
  void close_options_window();
  void set_all_options_from_gui();
  void save_all_options_to_file();
  void get_options_from_rtbrc();
  bool get_options_window_up() { return options_window_up; }

private:

  option_info_t<double> all_double_options[LAST_DOUBLE_OPTION];
  option_info_t<long> all_long_options[LAST_LONG_OPTION];

  option_info_t<String> all_string_options[LAST_STRING_OPTION];
  //  option_info_t<bool> all_bool_options[LAST_BOOL_OPTION];

  bool options_window_up;
  GtkWidget * options_window;
};
#endif __OPTIONS__
