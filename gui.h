#include <gtk/gtk.h>
#include <math.h>
#include "Vector2D.h"
#ifndef __ARENA__
#include "Arena.h"
#endif

#ifndef __GUI__
#define __GUI__

#define ZOOMFACTOR 20

enum zoom_t { NO_ZOOM, ZOOM_IN, ZOOM_OUT };

void statistics_button_callback(GtkWidget *widget, gpointer guip);
void delete_event( GtkWidget * widget, gpointer guip );
void no_zoom_callback(GtkWidget *widget, gpointer guip);
void zoom_in_callback(GtkWidget *widget, gpointer guip);
void zoom_out_callback(GtkWidget *widget, gpointer guip);
gint redraw_arena (GtkWidget *widget, GdkEventExpose *event, gpointer guip);

class Gui
{
public:
  Gui(class Arena * arenap);
  ~Gui() {}

  void setup_control_window();
  void setup_score_window();
  void setup_message_window();
  void setup_arena_window( const Vector2D bound[] );
  void setup_statistics_window();

  void close_control_window();
  void close_score_window();
  void close_message_window();
  void close_arena_window();
  void close_statistics_window();

  void quit_event();

  void print_to_message_output( char * from_robot, char * text, GdkColor& colour);
  void draw_objects();
  void draw_all_walls();

  double get_zoom();
  void change_zoom( const zoom_t type );
  int change_to_pixels_x(const double input);
  int change_to_pixels_y(const double input);

  void draw_circle( const Vector2D& center, const double radius, GdkColor& colour, const bool filled );
  void draw_line( const Vector2D& start, const Vector2D& end, GdkColor& colour );
  void draw_line( const Vector2D& start, const Vector2D& end, const double length, 
                  const double thickness, GdkColor& colour );
  void draw_rectangle( const Vector2D& start, const Vector2D& end, GdkColor& colour, const bool filled );

  int get_robot_nr( void * robotp, GList * robot_list );
  bool get_statistics_up() { return statistics_up; }
  GtkWidget * get_score_clist() { return score_clist; }
  Arena* get_the_arena() { return the_arena; }

private:
  void clear_area();

  bool statistics_up;

  GtkWidget * message_output;
  GtkWidget * drawing_area;
  GtkWidget * da_scrolled_window;

  GtkWidget * score_clist;

  GtkWidget * control_window;
  GtkWidget * score_window;
  GtkWidget * message_window;
  GtkWidget * arena_window;
  GtkWidget * statistics_window;

  GdkColormap * colormap;

  int zoomfactor;
  Vector2D boundary[2];

  class Arena * the_arena;
};

#endif
