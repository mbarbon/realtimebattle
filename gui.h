#include <gtk/gtk.h>
#include <math.h>
#include "Vector2D.h"
#ifndef __ARENA__
#include "Arena.h"
#endif

#ifndef __GUI__
#define __GUI__

#define ZOOMFACTOR 20

void statistics_button_callback(GtkWidget *widget, gpointer guip);
void delete_event( GtkWidget * widget, gpointer guip );

class Gui
{
public:
  Gui(class Arena * arenap);
  ~Gui() {}

  void setup_control_window();
  void setup_score_window();
  void setup_message_window();
  void setup_arena_window( Vector2D bound[] );
  void setup_statistics_window();

  void close_control_window();
  void close_score_window();
  void close_message_window();
  void close_arena_window();
  void close_statistics_window();

  void quit_event();

  void print_to_message_output( char * from_robot, char * text, GdkColor colour);
  void draw_objects();

  int change_to_pixels_x(double input);
  int change_to_pixels_y(double input);

  void draw_circle( Vector2D center, double radius, GdkColor colour, bool filled );
  void draw_line( Vector2D start, Vector2D end, GdkColor colour );
  void draw_line( Vector2D start, Vector2D end, double length, double thickness, GdkColor colour );
  void draw_rectangle( Vector2D start, Vector2D end, GdkColor colour, bool filled );

  GdkColor get_background_color() { return background_colour; }
  bool get_statistics_up() { return statistics_up; }

private:
  void clear_area();

  bool statistics_up;

  GtkWidget * message_output;
  GtkWidget * drawing_area;
  GtkWidget * da_scrolled_window;

  GtkWidget * control_window;
  GtkWidget * score_window;
  GtkWidget * message_window;
  GtkWidget * arena_window;
  GtkWidget * statistics_window;

  GdkColormap * colormap;
  GdkColor background_colour;

  Vector2D boundary[2];

  class Arena * the_arena;
};

#endif
