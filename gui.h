#include <gtk/gtk.h>
#include <math.h>
#include "Vector2D.h"

#ifndef __GUI__
#define __GUI__

#define ZOOMFACTOR 20

void delete_event( GtkWidget * widget, GdkEvent * event, gpointer data );

class Gui
{
public:
  Gui();
  ~Gui() {}

  void setup_control_window();
  void setup_score_window( void * the_arenap );
  void setup_message_window();
  void setup_arena_window( Vector2D bound[] );

  void close_control_window();
  void close_score_window();
  void close_message_window();
  void close_arena_window();
  void quit_event( GtkWidget * widget, GdkEvent * event );

  void print_to_message_output( char * from_robot, char * text, GdkColor colour);
  void draw_objects( void * the_arenap );

  int change_to_pixels_x(double input);
  int change_to_pixels_y(double input);

  void draw_circle( Vector2D center, double radius, GdkColor colour, bool filled );
  void draw_line( Vector2D start, Vector2D end, GdkColor colour );
  void draw_line( Vector2D start, Vector2D end, double length, double thickness, GdkColor colour );
  void draw_rectangle( Vector2D start, Vector2D end, GdkColor colour, bool filled );

private:
  void clear_area();

  GtkWidget * message_output;
  GtkWidget * drawing_area;
  GtkWidget * da_scrolled_window;

  GtkWidget * control_window;
  GtkWidget * score_window;
  GtkWidget * message_window;
  GtkWidget * arena_window;

  GdkColormap *colormap;
  GdkColor blue_colour;
  GdkColor background_colour;

  Vector2D boundary[2];
};

#endif
