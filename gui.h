#include <gtk/gtk.h>

#ifndef __GUI__
#define __GUI__

void delete_event( GtkWidget * widget, GdkEvent * event, gpointer data );

class Gui
{
public:
  Gui( int robot_number, char * robot_name_list[], int arena_width, int arena_height );
  ~Gui() {}

  void quit_event( GtkWidget * widget, GdkEvent * event );

  void print_to_message_output( char * from_robot, char * text, GdkColor colour);
  void draw_objects();

private:
  void draw_circle( int center, int radius, GdkColor colour, bool filled );
  void draw_line( int x1, int y1, int x2, int y2, GdkColor colour );
  void draw_rectangle( int x1, int y1, int x2, int y2, GdkColor colour, bool filled );

  void setup_control_window( int robot_number, char * robot_name_list[] );
  void setup_arena_window( void );

  int width;
  int height;

  GtkWidget * message_output;
  GtkWidget * drawing_area;
  GdkColor blue_colour;
  GdkColor background_colour;
};

#endif
