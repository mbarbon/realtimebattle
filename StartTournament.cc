#include <strstream.h>
#include <iostream.h>
#include <iomanip.h>
#include <dirent.h>
#include <sys/stat.h>
#include <math.h>
#include <string.h>
#include "gui.h"

void
start_tournament_button_callback(GtkWidget *widget, gpointer guip)
{
  if(((Gui *)guip)->get_start_tournament_up() == false)
    ((Gui *)guip)->setup_start_tournament_window();
  else
    ((Gui *)guip)->close_start_tournament_window();
}

void
Gui::setup_start_tournament_window()
{
  GtkWidget * vbox, * hbox;

  start_tournament_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (start_tournament_window), "RealTimeBattle Start New Tournament");
  gtk_signal_connect (GTK_OBJECT (start_tournament_window), "delete_event",
                      (GtkSignalFunc)gtk_widget_hide, GTK_OBJECT(start_tournament_window));
  gtk_container_border_width (GTK_CONTAINER (start_tournament_window), 12);

  vbox = gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (start_tournament_window), vbox);
  gtk_widget_show (vbox);

  hbox = gtk_hbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (vbox), hbox);
  gtk_widget_show (hbox);

  char * rob_tour_title[1] = { "Robots in Tournament" };
  robots_in_tournament_clist = gtk_clist_new_with_titles(1, rob_tour_title);
  gtk_clist_set_selection_mode (GTK_CLIST(robots_in_tournament_clist), GTK_SELECTION_MULTIPLE);
  gtk_clist_set_border(GTK_CLIST(robots_in_tournament_clist), GTK_SHADOW_IN);
  gtk_clist_set_column_width(GTK_CLIST(robots_in_tournament_clist), 0, 90);
  gtk_clist_set_column_justification(GTK_CLIST(robots_in_tournament_clist), 0, GTK_JUSTIFY_LEFT);
  gtk_clist_set_policy(GTK_CLIST(robots_in_tournament_clist), GTK_POLICY_AUTOMATIC,
                       GTK_POLICY_AUTOMATIC);
  gtk_widget_set_usize(robots_in_tournament_clist, 150, 150);
  gtk_box_pack_start (GTK_BOX (hbox), robots_in_tournament_clist, TRUE, TRUE, 0);
  gtk_widget_show( robots_in_tournament_clist );

  GtkWidget * middle_vbox = gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (hbox), middle_vbox);
  gtk_widget_show (middle_vbox);

  gtk_widget_show( start_tournament_window );

  const char * arrow_xpms[2][11] = {
    {"13 8 2 1",
     "       c None",
     "x      c #000000000000",
     "   xx        ",
     "  xx         ",
     " xx          ",
     "xxxxxxxxxxxxx",
     "xxxxxxxxxxxxx",
     " xx          ",
     "  xx         ",
     "   xx        "},

    {"13 8 2 1",
     "       c None",
     "x      c #000000000000",
     "        xx   ",
     "         xx  ",
     "          xx ",
     "xxxxxxxxxxxxx",
     "xxxxxxxxxxxxx",
     "          xx ",
     "         xx  ",
     "        xx   "}};

  for(int i=0;i<2;i++)
    {
      GdkPixmap * pixmap;
      GdkBitmap * bitmap_mask;
      pixmap = gdk_pixmap_create_from_xpm_d( start_tournament_window->window, &bitmap_mask,
                                             the_arena->get_background_colour_p(),
                                             (gchar **)arrow_xpms[i] );
      GtkWidget * pixmap_widget = gtk_pixmap_new( pixmap, bitmap_mask );
      gtk_widget_show( pixmap_widget );
      GtkWidget * button = gtk_button_new();
      gtk_container_add( GTK_CONTAINER(button), pixmap_widget );

      //  button_info_t * data;
      //  data = new button_info_t( this, (stat_button_t)(i + 3) );
      //  gtk_signal_connect (GTK_OBJECT (button), "clicked",
      //                      GTK_SIGNAL_FUNC (buttons_in_statistics_callback), (gpointer) data);
      //  gtk_widget_set_usize(button, 28,14);
      gtk_box_pack_start (GTK_BOX (middle_vbox), button, TRUE, FALSE, 0);
      gtk_widget_show (button);
    }

  char * rob_dir_title[1] = { "Robots in Directory" };
  robots_in_directory_clist = gtk_clist_new_with_titles(1, rob_dir_title);
  gtk_clist_set_selection_mode (GTK_CLIST(robots_in_directory_clist), GTK_SELECTION_MULTIPLE);
  gtk_clist_set_border(GTK_CLIST(robots_in_directory_clist), GTK_SHADOW_IN);
  gtk_clist_set_column_width(GTK_CLIST(robots_in_directory_clist), 0, 90);
  gtk_clist_set_column_justification(GTK_CLIST(robots_in_directory_clist), 0, GTK_JUSTIFY_LEFT);
  gtk_clist_set_policy(GTK_CLIST(robots_in_directory_clist), GTK_POLICY_AUTOMATIC,
                       GTK_POLICY_AUTOMATIC);
  gtk_widget_set_usize(robots_in_directory_clist, 150, 150);
  gtk_box_pack_start (GTK_BOX (hbox), robots_in_directory_clist, TRUE, TRUE, 0);
  gtk_widget_show( robots_in_directory_clist );

  DIR * robotdir;
  if( NULL != (robotdir = opendir(getenv("RTB_ROBOTDIR"))))
    {
      struct dirent * entry;
      while (NULL != ( entry = readdir( robotdir ) ) )
        {
          GString * full_file_name;
          full_file_name = g_string_new(getenv("RTB_ROBOTDIR"));
          g_string_append( full_file_name, entry->d_name );
          struct stat filestat;
          if( 0 == stat( full_file_name->str, &filestat ) )
            if( S_ISREG( filestat.st_mode) && (filestat.st_mode & S_IXOTH) ) // Check if file is a regular file that can be executed
              {     
                char * list[] = { "" };
          
                int row = gtk_clist_append(GTK_CLIST(robots_in_directory_clist), list);
                gtk_clist_set_foreground(GTK_CLIST(robots_in_directory_clist), row, the_arena->get_foreground_colour_p());
                gtk_clist_set_background(GTK_CLIST(robots_in_directory_clist), row, the_arena->get_background_colour_p());
          
                gtk_clist_set_text(GTK_CLIST(robots_in_directory_clist), row, 0, entry->d_name);
              }
        }
      closedir( robotdir );
    }

  start_tournament_up = true;
}

void
Gui::close_start_tournament_window()
{
  start_tournament_up = false;
  gtk_widget_destroy ( start_tournament_window );
}
