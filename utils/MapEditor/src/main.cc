#include <gtk/gtk.h>
#include "MainWindow.h"



int main( int   argc,
	  char *argv[] )
{

  gtk_init (&argc, &argv);

  GtkWidget* window = init_main_window();
  
  gtk_widget_show(window);

  message("Ready...\n");

  gtk_main ();
  
  return(0);
}

