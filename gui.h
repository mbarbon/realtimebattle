#include <gtk/gtk.h>

//void draw_objects ( void );
//void insert_message (char * robot_name, char * message);
//void callback (GtkWidget *widget, gpointer data);
//void delete_event (GtkWidget *widget, GdkEvent *event, gpointer data);

//void control_win( int antrobotar, char * robotnames[] );
//void arena_win( void );

class Gui
{
public:
  Gui( int robot_number, char * robot_name_list[], int arena_width, int arena_height );
  ~Gui() {}

  void add_to_message_output( char * from_robot, char * output_text );

private:
  void delete_event( GtkWidget * widget, GdkEvent * event, gpointer data );

  void setup_control_window( int robot_number, char * robot_name_list[] );
  void setup_arena_window( void );

  int width;
  int height;

  GtkWidget * message_output;
  GtkWidget * drawing_area;
  GdkColor colour;
  GdkColor da_bkg_colour;
};
