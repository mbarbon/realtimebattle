#include <strstream.h>
#include <iostream.h>
#include <iomanip.h>
#include <dirent.h>
#include <sys/stat.h>
#include <math.h>
#include <string.h>
#include "gui.h"

void
start_tournament_button_callback(GtkWidget *widget, gpointer data)
{
  if(the_gui.get_start_tournament_up() == false)
    the_gui.setup_start_tournament_window();
  else
    the_gui.close_start_tournament_window();
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

  // Widgets for selecting robots

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

  GtkWidget * vbox2 = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start( GTK_BOX(hbox), vbox2, TRUE, TRUE, 0);
  gtk_widget_show (vbox2);

  {
    char * button_labels[] = { "Remove", "Select All", "Unselect All" };
    for(int i=0;i<3;i++)
      {
        GtkWidget * button = gtk_button_new_with_label( button_labels[i] );
        gtk_box_pack_start( GTK_BOX(vbox2), button, TRUE, FALSE, 0);
        gtk_widget_show( button );
      }
  }

  vbox2 = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start( GTK_BOX(hbox), vbox2, TRUE, TRUE, 0);
  gtk_widget_show (vbox2);

  {
    char * button_labels[] = { "Add", "Select All", "Unselect All" };
    for(int i=0;i<3;i++)
      {
        GtkWidget * button = gtk_button_new_with_label( button_labels[i] );
        gtk_box_pack_start( GTK_BOX(vbox2), button, TRUE, FALSE, 0);
        gtk_widget_show( button );
      }
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
  if( NULL != getenv("RTB_ROBOTDIR"))
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
                  gtk_clist_set_foreground(GTK_CLIST(robots_in_directory_clist), row, the_arena.get_foreground_colour_p());
                  gtk_clist_set_background(GTK_CLIST(robots_in_directory_clist), row, the_arena.get_background_colour_p());
          
                  gtk_clist_set_text(GTK_CLIST(robots_in_directory_clist), row, 0, entry->d_name);
                }
          }
        closedir( robotdir );
      }

  // Widgets for selecting arenas

  hbox = gtk_hbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (vbox), hbox);
  gtk_widget_show (hbox);

  char * arena_tour_title[1] = { "Arenas in Tournament" };
  arenas_in_tournament_clist = gtk_clist_new_with_titles(1, arena_tour_title);
  gtk_clist_set_selection_mode (GTK_CLIST(arenas_in_tournament_clist), GTK_SELECTION_MULTIPLE);
  gtk_clist_set_border(GTK_CLIST(arenas_in_tournament_clist), GTK_SHADOW_IN);
  gtk_clist_set_column_width(GTK_CLIST(arenas_in_tournament_clist), 0, 90);
  gtk_clist_set_column_justification(GTK_CLIST(arenas_in_tournament_clist), 0, GTK_JUSTIFY_LEFT);
  gtk_clist_set_policy(GTK_CLIST(arenas_in_tournament_clist), GTK_POLICY_AUTOMATIC,
                       GTK_POLICY_AUTOMATIC);
  gtk_widget_set_usize(arenas_in_tournament_clist, 150, 150);
  gtk_box_pack_start (GTK_BOX (hbox), arenas_in_tournament_clist, TRUE, TRUE, 0);
  gtk_widget_show( arenas_in_tournament_clist );

  vbox2 = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start( GTK_BOX(hbox), vbox2, TRUE, TRUE, 0);
  gtk_widget_show (vbox2);

  {
    char * button_labels[] = { "Remove", "Select All", "Unselect All" };
    for(int i=0;i<3;i++)
      {
        GtkWidget * button = gtk_button_new_with_label( button_labels[i] );
        gtk_box_pack_start( GTK_BOX(vbox2), button, TRUE, FALSE, 0);
        gtk_widget_show( button );
      }
  }

  vbox2 = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start( GTK_BOX(hbox), vbox2, TRUE, TRUE, 0);
  gtk_widget_show (vbox2);

  {
    char * button_labels[] = { "Add", "Select All", "Unselect All" };
    for(int i=0;i<3;i++)
      {
        GtkWidget * button = gtk_button_new_with_label( button_labels[i] );
        gtk_box_pack_start( GTK_BOX(vbox2), button, TRUE, FALSE, 0);
        gtk_widget_show( button );
      }
  }

  char * arena_dir_title[1] = { "Arenas in Directory" };
  arenas_in_directory_clist = gtk_clist_new_with_titles(1, arena_dir_title);
  gtk_clist_set_selection_mode (GTK_CLIST(arenas_in_directory_clist), GTK_SELECTION_MULTIPLE);
  gtk_clist_set_border(GTK_CLIST(arenas_in_directory_clist), GTK_SHADOW_IN);
  gtk_clist_set_column_width(GTK_CLIST(arenas_in_directory_clist), 0, 90);
  gtk_clist_set_column_justification(GTK_CLIST(arenas_in_directory_clist), 0, GTK_JUSTIFY_LEFT);
  gtk_clist_set_policy(GTK_CLIST(arenas_in_directory_clist), GTK_POLICY_AUTOMATIC,
                       GTK_POLICY_AUTOMATIC);
  gtk_widget_set_usize(arenas_in_directory_clist, 150, 150);
  gtk_box_pack_start (GTK_BOX (hbox), arenas_in_directory_clist, TRUE, TRUE, 0);
  gtk_widget_show( arenas_in_directory_clist );

  DIR * arenadir;
  if( NULL != getenv("RTB_ARENADIR"))
    if( NULL != (arenadir = opendir(getenv("RTB_ARENADIR"))))
      {
        struct dirent * entry;
        while (NULL != ( entry = readdir( arenadir ) ) )
          {
            GString * full_file_name;
            full_file_name = g_string_new(getenv("RTB_ARENADIR"));
            g_string_append( full_file_name, entry->d_name );
            struct stat filestat;
            if( 0 == stat( full_file_name->str, &filestat ) && full_file_name->len > 6 )
              // Check if file is a regular file that is readable and ends with .arena
              if( S_ISREG( filestat.st_mode) &&
                  (filestat.st_mode & S_IROTH)  &&
                  strcmp(".arena",&(full_file_name->str[full_file_name->len - 6])) == 0 )
                {     
                  char * list[] = { "" };
          
                  int row = gtk_clist_append(GTK_CLIST(arenas_in_directory_clist), list);
                  gtk_clist_set_foreground(GTK_CLIST(arenas_in_directory_clist), row, the_arena.get_foreground_colour_p());
                  gtk_clist_set_background(GTK_CLIST(arenas_in_directory_clist), row, the_arena.get_background_colour_p());
          
                  gtk_clist_set_text(GTK_CLIST(arenas_in_directory_clist), row, 0, entry->d_name);
                }
          }
        closedir( arenadir );
      }

  // Choose Number of games per sequence, Number of robots per sequence and Number of sequences

  char * label_titles[] = { "Games per sequence", "Robots per sequence", "Number of sequences" };

  for( int i=0;i<3;i++ )
    {
      hbox = gtk_hbox_new (FALSE, 5);
      gtk_container_add (GTK_CONTAINER (vbox), hbox);
      gtk_widget_show (hbox);

      GtkWidget * label = gtk_label_new(label_titles[i]);
      gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
      gtk_widget_show(label);

      GtkWidget * entry = gtk_entry_new_with_max_length(4);
      gtk_entry_set_text( GTK_ENTRY( entry ), "0");
      gtk_box_pack_start (GTK_BOX (hbox), entry, FALSE, FALSE, 0);
      gtk_widget_set_usize(entry, 36,18);
      gtk_widget_show(entry);
    }

  gtk_widget_show( start_tournament_window );
  start_tournament_up = true;
}

void
Gui::close_start_tournament_window()
{
  start_tournament_up = false;
  gtk_widget_destroy ( start_tournament_window );
}
