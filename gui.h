#include <gtk/gtk.h>

void draw_objects ( void );
void insert_message (char * robot_name, char * message);
void callback (GtkWidget *widget, gpointer data);
void delete_event (GtkWidget *widget, GdkEvent *event, gpointer data);

void control_win( int antrobotar, char * robotnames[] );
void arena_win( void );

