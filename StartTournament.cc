#include <strstream.h>
#include <iostream.h>
#include <iomanip.h>
#include <dirent.h>
#include <sys/stat.h>
#include <math.h>
#include <string.h>
#include "gui.h"
#include "Arena.h"
#include "String.h"

void
start_tournament_min_callback(GtkWidget *widget, gpointer data)
{
  String name((char *) data);
  if( name == String("Games per sequence") )
    the_gui.set_entry(0, MMF_MIN);
  if( name == String("Robots per sequence") )
    the_gui.set_entry(1, MMF_MIN);
  if( name == String("Number of sequences") )
    the_gui.set_entry(2, MMF_MIN);
}

void
start_tournament_max_callback(GtkWidget *widget, gpointer data)
{
  String name((char *) data);
  if( name == String("Games per sequence") )
    the_gui.set_entry(0, MMF_MAX);
  if( name == String("Robots per sequence") )
    the_gui.set_entry(1, MMF_MAX);
  if( name == String("Number of sequences") )
    the_gui.set_entry(2, MMF_MAX);
}

void
start_tournament_full_round_callback(GtkWidget *widget, gpointer data)
{
  the_gui.set_entry(2,MMF_FULL_ROUND);
}

void
start_tournament_button_callback(GtkWidget *widget, gpointer data)
{
  if(the_gui.get_start_tournament_up() == false)
    the_gui.setup_start_tournament_window();
  else
    the_gui.close_start_tournament_window();
}

void
start_tournament_start_callback(GtkWidget *widget, gpointer data)
{
  if(the_arena.get_state() == Arena::NOT_STARTED || the_arena.get_state() == Arena::FINISHED)
    the_gui.start_new_tournament();
}

void
start_tournament_select_robots_buttons_callback(GtkWidget *widget, gpointer button_number_p)
{
  switch(*(int *)button_number_p)
    {
    case START_TORUNAMENT_REMOVE:
      the_gui.start_tournament_remove_all_selected( true );
      break;
    case START_TORUNAMENT_SELECT_ALL_TOURNAMENT:
      the_gui.start_tournament_change_all_selection( true,false,true );
      break;
    case START_TORUNAMENT_UNSELECT_ALL_TOURNAMENT:
      the_gui.start_tournament_change_all_selection( true,false,false );
      break;
    case START_TORUNAMENT_ADD:
      the_gui.start_tournament_add_all_selected( true );
      break;
    case START_TORUNAMENT_SELECT_ALL_DIRECTORY:
      the_gui.start_tournament_change_all_selection( true,true,true );
      break;
    case START_TORUNAMENT_UNSELECT_ALL_DIRECTORY:
      the_gui.start_tournament_change_all_selection( true,true,false );
      break;
    }
}

void
start_tournament_select_arenas_buttons_callback(GtkWidget *widget, gpointer button_number_p)
{
  switch(*(int *)button_number_p)
    {
    case START_TORUNAMENT_REMOVE:
      the_gui.start_tournament_remove_all_selected( false );
      break;
    case START_TORUNAMENT_SELECT_ALL_TOURNAMENT:
      the_gui.start_tournament_change_all_selection( false,false,true );
      break;
    case START_TORUNAMENT_UNSELECT_ALL_TOURNAMENT:
      the_gui.start_tournament_change_all_selection( false,false,false );
      break;
    case START_TORUNAMENT_ADD:
      the_gui.start_tournament_add_all_selected( false );
      break;
    case START_TORUNAMENT_SELECT_ALL_DIRECTORY:
      the_gui.start_tournament_change_all_selection( false,true,true );
      break;
    case START_TORUNAMENT_UNSELECT_ALL_DIRECTORY:
      the_gui.start_tournament_change_all_selection( false,true,false );
      break;
    }
}

void
start_tournament_selection_made( GtkWidget * clist, gint row, gint column,
                                 GdkEventButton *event, gpointer data )
{
  if( event != NULL )
    the_gui.start_tournament_change_one_selection( row, clist );
}

void
Gui::start_new_tournament()
{
  int min_value,max_value;
  int robot_number = 0;
  int value[3];

  GList * gl;
  for(gl = g_list_next( selected_items_in_robot_tournament ); gl != NULL ; gl = g_list_next(gl))
    robot_number++;

  for(int i = 0 ; i < 3; i++)
    {
      String text = gtk_entry_get_text(GTK_ENTRY(start_tournament_entries[i]));
      if(i != 1)
        max_value = 10000;
      else
        {
          max_value = min(120,robot_number);
        }
      if(i != 1)
        min_value = 1;
      else
        min_value = 2;

      value[i] = str2int(text);
      if( value[i] > max_value )
        value[i] = max_value;
      if( value[i] < min_value )
        value[i] = min_value;
    } 
  if(robot_number > 1 && g_list_next( selected_items_in_arena_tournament ) != NULL )
    {
      the_arena.start_tournament( selected_items_in_robot_tournament , selected_items_in_arena_tournament, value[1], value[0], value[2]);  
      close_start_tournament_window();
    }
}

start_tournament_glist_info_t *
Gui::start_tournament_find_row_in_clist(const int row, GList * list)
{
  GList * gl;
  for(gl = g_list_next(list); gl !=NULL; gl = g_list_next(gl))
    {
      start_tournament_glist_info_t * info_p = (start_tournament_glist_info_t *)gl->data;
      if( info_p->row == row )
        return info_p;
    }
  return NULL;
}

void
Gui::start_tournament_change_one_selection(const int row, const GtkWidget * clist)
{
  GList * list;

  if(clist == robots_in_tournament_clist)
    list = selected_items_in_robot_tournament;
  else if(clist == robots_in_directory_clist)
    list = selected_items_in_robot_directory;
  else if(clist == arenas_in_tournament_clist)
    list = selected_items_in_arena_tournament;
  else if(clist == arenas_in_directory_clist)
    list = selected_items_in_arena_directory;

  start_tournament_glist_info_t * info_p;
  info_p = start_tournament_find_row_in_clist( row, list );
  if( info_p->selected )
    info_p->selected = false;
  else 
    info_p->selected = true;
}

void
Gui::start_tournament_change_all_selection(bool robots, bool dir, bool all)
{
  GList * gl;
  GList * list;
  GtkWidget * clist;


  if( robots && dir )
    {
      list = selected_items_in_robot_directory;
      clist = robots_in_directory_clist;
    }
  else if(robots && !dir)
    {
      list = selected_items_in_robot_tournament;
      clist = robots_in_tournament_clist;
    }
  else if(!robots && dir)
    {
      list = selected_items_in_arena_directory;
      clist = arenas_in_directory_clist;
    }
  else if(!robots && !dir)
    {
      list = selected_items_in_arena_tournament;
      clist = arenas_in_tournament_clist;
    }
  for(gl = g_list_next(list); gl != NULL; gl = g_list_next(gl))
    {
      start_tournament_glist_info_t * info_p = (start_tournament_glist_info_t *)gl->data;
      if( all )
        {
          info_p->selected = true;
          gtk_clist_select_row(GTK_CLIST(clist), info_p->row, 0);
        }
      else
        {
          info_p->selected = false;
          gtk_clist_unselect_row(GTK_CLIST(clist), info_p->row, 0);
        }
    }
}

void
Gui::start_tournament_add_all_selected( bool robots )
{
  GList * gl, * gl_tourn;
  GtkWidget * clist_tourn;

  if(robots)
    {
      gl = g_list_next(selected_items_in_robot_directory);
      gl_tourn = selected_items_in_robot_tournament;
      clist_tourn = robots_in_tournament_clist;
    }
  else
    {
      gl = g_list_next(selected_items_in_arena_directory);
      gl_tourn = selected_items_in_arena_tournament;
      clist_tourn = arenas_in_tournament_clist;
    }

  for( ; gl != NULL ; gl = g_list_next(gl))
    {
      start_tournament_glist_info_t * info_dir_p = (start_tournament_glist_info_t *)gl->data;
      if( info_dir_p->selected )
        {
          char * list[] = { "" };
          
          int row = gtk_clist_append(GTK_CLIST(clist_tourn), list);
          gtk_clist_set_foreground(GTK_CLIST(clist_tourn), row, the_arena.get_foreground_colour_p());
          gtk_clist_set_background(GTK_CLIST(clist_tourn), row, the_arena.get_background_colour_p());      

          gtk_clist_set_text(GTK_CLIST(clist_tourn), row, 0, info_dir_p->filename.non_const_chars());
      
          start_tournament_glist_info_t * info_tourn_p;
          String full_filename;
          if(robots)
            full_filename = robotdir + info_dir_p->filename;
          else
            full_filename = arenadir + info_dir_p->filename;

          info_tourn_p = new start_tournament_glist_info_t(row,false,full_filename.chars());
          g_list_append(gl_tourn, info_tourn_p);                        
        }
    }
}

void
Gui::start_tournament_remove_all_selected( bool robots )
{
  GList * gl, * list;
  GtkWidget * clist_tourn;

  if(robots)
    {
      list = selected_items_in_robot_tournament;
      clist_tourn = robots_in_tournament_clist;
    }
  else
    {
      list = selected_items_in_arena_tournament;
      clist_tourn = arenas_in_tournament_clist;
    }

  for( gl = g_list_next(list); gl != NULL ; )
    {
      start_tournament_glist_info_t * info_p = (start_tournament_glist_info_t *)gl->data;
      gl = g_list_next(gl);
      if( info_p->selected )
        {
          gtk_clist_remove(GTK_CLIST(clist_tourn), info_p->row);
          g_list_remove(list, info_p);
          GList * gl2;
          for( gl2 = g_list_next(list); gl2 != NULL; gl2 = g_list_next(gl2))
            {
              start_tournament_glist_info_t * info2_p = (start_tournament_glist_info_t *)gl2->data;
              if(info2_p->row > info_p->row)
                info2_p->row = info2_p->row - 1;
            }
          delete info_p;
        }
    }
}

void
Gui::set_entry(const int entry, const enum min_max_full_t mmf)
{
  switch( mmf )
    {
    case MMF_MIN:
      {
        if(entry != 1)
          gtk_entry_set_text( GTK_ENTRY( start_tournament_entries[entry] ), "1" );
        else
          gtk_entry_set_text( GTK_ENTRY( start_tournament_entries[entry] ), "2" );
        break;
      }
    case MMF_MAX:
      {
        if(entry != 1)
          gtk_entry_set_text( GTK_ENTRY( start_tournament_entries[entry] ), "9999" );
        else
          {
            GList * gl;
            int number_of_robots = 0;
            for(gl = g_list_next(selected_items_in_robot_tournament); gl !=NULL; gl = g_list_next(gl))
              number_of_robots++;
            if( number_of_robots > 120 )
              number_of_robots = 120;
            gtk_entry_set_text( GTK_ENTRY( start_tournament_entries[entry] ), String(number_of_robots).chars() );
          }
        break;
      }
    case MMF_FULL_ROUND:
      {
        GList * gl;
        int number_of_robots = 0;
        for(gl = g_list_next(selected_items_in_robot_tournament); gl !=NULL; gl = g_list_next(gl))
          number_of_robots++;
        int robots_per_sequence = str2int(gtk_entry_get_text(GTK_ENTRY(start_tournament_entries[1])));
        if( number_of_robots < robots_per_sequence )
          robots_per_sequence = number_of_robots;
        if( robots_per_sequence > 120 )
          robots_per_sequence = 120;
        if( robots_per_sequence < 2 )
          robots_per_sequence = 2;
        gtk_entry_set_text( GTK_ENTRY( start_tournament_entries[entry] ),
                            String(min(9999,binomial(number_of_robots,robots_per_sequence))).chars() );
      break;
      }
    }
}

void
Gui::setup_start_tournament_window()
{
  GtkWidget * vbox, * hbox;

  selected_items_in_robot_tournament = g_list_alloc();
  selected_items_in_robot_directory = g_list_alloc();
  selected_items_in_arena_tournament = g_list_alloc();
  selected_items_in_arena_directory = g_list_alloc();

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
  gtk_signal_connect(GTK_OBJECT(robots_in_tournament_clist), "select_row",
                     GTK_SIGNAL_FUNC(start_tournament_selection_made), NULL);
  gtk_signal_connect(GTK_OBJECT(robots_in_tournament_clist), "unselect_row",
                     GTK_SIGNAL_FUNC(start_tournament_selection_made), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), robots_in_tournament_clist, TRUE, TRUE, 0);
  gtk_widget_show( robots_in_tournament_clist );

  GtkWidget * vbox2 = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start( GTK_BOX(hbox), vbox2, TRUE, TRUE, 0);
  gtk_widget_show (vbox2);

  {
    char * button_labels[] = { "Remove", "Select All", "Unselect All" };
    for(int i=START_TORUNAMENT_REMOVE;i<=START_TORUNAMENT_UNSELECT_ALL_TOURNAMENT;i++)
      {
        GtkWidget * button = gtk_button_new_with_label( button_labels[i] );
        int * button_number_p;
        button_number_p = new int(i);
        gtk_signal_connect (GTK_OBJECT (button), "clicked",
                            GTK_SIGNAL_FUNC (start_tournament_select_robots_buttons_callback), (gpointer) button_number_p );
        gtk_box_pack_start( GTK_BOX(vbox2), button, TRUE, FALSE, 0);
        gtk_widget_show( button );
      }
  }

  vbox2 = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start( GTK_BOX(hbox), vbox2, TRUE, TRUE, 0);
  gtk_widget_show (vbox2);

  {
    char * button_labels[] = { "Add", "Select All", "Unselect All" };
    for(int i=START_TORUNAMENT_ADD;i<=START_TORUNAMENT_UNSELECT_ALL_DIRECTORY;i++)
      {
        GtkWidget * button = gtk_button_new_with_label( button_labels[i-START_TORUNAMENT_ADD] );
        int * button_number_p;
        button_number_p = new int(i);
        gtk_signal_connect (GTK_OBJECT (button), "clicked",
                            GTK_SIGNAL_FUNC (start_tournament_select_robots_buttons_callback), (gpointer) button_number_p );
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
  gtk_signal_connect(GTK_OBJECT(robots_in_directory_clist), "select_row",
                     GTK_SIGNAL_FUNC(start_tournament_selection_made), NULL);
  gtk_signal_connect(GTK_OBJECT(robots_in_directory_clist), "unselect_row",
                     GTK_SIGNAL_FUNC(start_tournament_selection_made), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), robots_in_directory_clist, TRUE, TRUE, 0);
  gtk_widget_show( robots_in_directory_clist );

  DIR * rdir;
  if( NULL != (rdir = opendir(robotdir.chars())))
    {
      struct dirent * entry;
      while (NULL != ( entry = readdir( rdir ) ) )
        {
          String full_file_name = robotdir + entry->d_name;
          struct stat filestat;
          if( 0 == stat( full_file_name.chars(), &filestat ) )
            if( S_ISREG( filestat.st_mode) && (filestat.st_mode & S_IXOTH) ) // Check if file is a regular file that can be executed
              {     
                char * list[] = { "" };
          
                int row = gtk_clist_append(GTK_CLIST(robots_in_directory_clist), list);
                gtk_clist_set_foreground(GTK_CLIST(robots_in_directory_clist), row, the_arena.get_foreground_colour_p());
                gtk_clist_set_background(GTK_CLIST(robots_in_directory_clist), row, the_arena.get_background_colour_p());
          
                gtk_clist_set_text(GTK_CLIST(robots_in_directory_clist), row, 0, entry->d_name);

                start_tournament_glist_info_t * info;
                info = new start_tournament_glist_info_t(row,false,entry->d_name);
                g_list_append(selected_items_in_robot_directory, info);                  
              }
        }
      closedir( rdir );
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
  gtk_signal_connect(GTK_OBJECT(arenas_in_tournament_clist), "select_row",
                     GTK_SIGNAL_FUNC(start_tournament_selection_made), NULL);
  gtk_signal_connect(GTK_OBJECT(arenas_in_tournament_clist), "unselect_row",
                     GTK_SIGNAL_FUNC(start_tournament_selection_made), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), arenas_in_tournament_clist, TRUE, TRUE, 0);
  gtk_widget_show( arenas_in_tournament_clist );

  vbox2 = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start( GTK_BOX(hbox), vbox2, TRUE, TRUE, 0);
  gtk_widget_show (vbox2);

  {
    char * button_labels[] = { "Remove", "Select All", "Unselect All" };
    for(int i=START_TORUNAMENT_REMOVE;i<=START_TORUNAMENT_UNSELECT_ALL_TOURNAMENT;i++)
      {
        GtkWidget * button = gtk_button_new_with_label( button_labels[i] );
        int * button_number_p;
        button_number_p = new int(i);
        gtk_signal_connect (GTK_OBJECT (button), "clicked",
                            GTK_SIGNAL_FUNC (start_tournament_select_arenas_buttons_callback), (gpointer) button_number_p );
        gtk_box_pack_start( GTK_BOX(vbox2), button, TRUE, FALSE, 0);
        gtk_widget_show( button );
      }
  }

  vbox2 = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start( GTK_BOX(hbox), vbox2, TRUE, TRUE, 0);
  gtk_widget_show (vbox2);

  {
    char * button_labels[] = { "Add", "Select All", "Unselect All" };
    for(int i=START_TORUNAMENT_ADD;i<=START_TORUNAMENT_UNSELECT_ALL_DIRECTORY;i++)
      {
        GtkWidget * button = gtk_button_new_with_label( button_labels[i-START_TORUNAMENT_ADD] );
        int * button_number_p;
        button_number_p = new int(i);
        gtk_signal_connect (GTK_OBJECT (button), "clicked",
                            GTK_SIGNAL_FUNC (start_tournament_select_arenas_buttons_callback), (gpointer) button_number_p );
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
  gtk_signal_connect(GTK_OBJECT(arenas_in_directory_clist), "select_row",
                     GTK_SIGNAL_FUNC(start_tournament_selection_made), NULL);
  gtk_signal_connect(GTK_OBJECT(arenas_in_directory_clist), "unselect_row",
                     GTK_SIGNAL_FUNC(start_tournament_selection_made), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), arenas_in_directory_clist, TRUE, TRUE, 0);
  gtk_widget_show( arenas_in_directory_clist );

  DIR * adir;
  if( NULL != (adir = opendir(arenadir.chars())))
    {
      struct dirent * entry;
      while (NULL != ( entry = readdir( adir ) ) )
        {
          String full_file_name = arenadir + entry->d_name;
          struct stat filestat;
          if( 0 == stat( full_file_name.chars(), &filestat ) && full_file_name.get_length() > 6 )
            // Check if file is a regular file that is readable and ends with .arena
            if( S_ISREG( filestat.st_mode) &&
                (filestat.st_mode & S_IROTH)  &&
                String(".arena") == get_segment(full_file_name, -6, -1) )
              {     
                char * list[] = { "" };
          
                int row = gtk_clist_append(GTK_CLIST(arenas_in_directory_clist), list);
                gtk_clist_set_foreground(GTK_CLIST(arenas_in_directory_clist), row, the_arena.get_foreground_colour_p());
                gtk_clist_set_background(GTK_CLIST(arenas_in_directory_clist), row, the_arena.get_background_colour_p());
          
                gtk_clist_set_text(GTK_CLIST(arenas_in_directory_clist), row, 0, entry->d_name);
                start_tournament_glist_info_t * info;
                info = new start_tournament_glist_info_t(row,false,entry->d_name);
                g_list_append(selected_items_in_arena_directory, info);
              }
        }
      closedir( adir );
    }

  // Choose Number of games per sequence, Number of robots per sequence and Number of sequences

  char * label_titles[] = { "Games per sequence", "Robots per sequence", "Number of sequences" };

  GtkWidget * hbox2 = gtk_hbox_new (FALSE, 100);
  gtk_box_pack_start (GTK_BOX (vbox), hbox2, FALSE, FALSE, 0);
  gtk_widget_show (hbox2);

  GtkWidget * internal_hbox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (hbox2), internal_hbox, FALSE, FALSE, 0);
  gtk_widget_show (internal_hbox);

  GtkWidget * description_table = gtk_table_new( 3, 1, TRUE );
  GtkWidget * entry_table = gtk_table_new( 3, 1, TRUE );
  GtkWidget * button_table = gtk_table_new( 3, 12, TRUE );
  gtk_box_pack_start (GTK_BOX (internal_hbox), description_table, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (internal_hbox), entry_table, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (internal_hbox), button_table, FALSE, FALSE, 0);

  for( int i=0;i<3;i++ )
    {
      GtkWidget * internal_hbox = gtk_hbox_new (FALSE, 5);
      gtk_table_attach_defaults( GTK_TABLE( description_table ), internal_hbox, 0, 1, i, i + 1 );
      gtk_widget_show (internal_hbox);

      GtkWidget * label = gtk_label_new(label_titles[i]);
      gtk_box_pack_start (GTK_BOX (internal_hbox), label, FALSE, FALSE, 0);
      gtk_widget_show(label);

      start_tournament_entries[i] = gtk_entry_new_with_max_length(4);
      if(i !=1 )
        gtk_entry_set_text( GTK_ENTRY( start_tournament_entries[i] ), "1");
      else
        gtk_entry_set_text( GTK_ENTRY( start_tournament_entries[i] ), "2");

      entry_t * info = new entry_t( ENTRY_INT, false );

      gtk_signal_connect(GTK_OBJECT(start_tournament_entries[i]), "changed",
                         GTK_SIGNAL_FUNC(entry_handler), info);
      gtk_table_attach_defaults( GTK_TABLE( entry_table ), start_tournament_entries[i], 0, 1, i, i + 1 );
      gtk_widget_set_usize(start_tournament_entries[i], 36,18);
      gtk_widget_show(start_tournament_entries[i]);

      GtkWidget * button;
      int add = 0;
      if( i != 2 ) add = 3;

      button = gtk_button_new_with_label ("Min");
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          GTK_SIGNAL_FUNC (start_tournament_min_callback), (gpointer) label_titles[i] );
      gtk_table_attach_defaults( GTK_TABLE( button_table ), button, 0, 3 + add, i, i + 1 );
      gtk_widget_show (button);

      if( i == 2 )
        {
          button = gtk_button_new_with_label ("Full Round");
          gtk_signal_connect (GTK_OBJECT (button), "clicked",
                              GTK_SIGNAL_FUNC (start_tournament_full_round_callback), (gpointer) NULL );
          gtk_table_attach_defaults( GTK_TABLE( button_table ), button, 3, 9, i, i + 1 );
          gtk_widget_show (button);
        }

      button = gtk_button_new_with_label ("Max");
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          GTK_SIGNAL_FUNC (start_tournament_max_callback), (gpointer) label_titles[i] );
      gtk_table_attach_defaults( GTK_TABLE( button_table ), button, 9 - add, 12, i, i + 1 );
      gtk_widget_show (button);
    }

  gtk_table_set_row_spacings( GTK_TABLE( description_table ) , 5 );
  gtk_table_set_col_spacings( GTK_TABLE( description_table ) , 5 );
  gtk_table_set_row_spacings( GTK_TABLE( entry_table ) , 5 );
  gtk_table_set_col_spacings( GTK_TABLE( entry_table ) , 5 );
  gtk_table_set_row_spacings( GTK_TABLE( button_table ) , 5 );
  gtk_table_set_col_spacings( GTK_TABLE( button_table ) , 5 );
  gtk_widget_show( description_table );
  gtk_widget_show( entry_table );
  gtk_widget_show( button_table );

  vbox2 = gtk_vbox_new (FALSE, 5);
  gtk_box_pack_start( GTK_BOX(hbox2), vbox2, TRUE, TRUE, 0);
  gtk_widget_show (vbox2);

  {
    char * button_labels[] = { "Start", "Cancel" };
    for(int i=0;i<2;i++)
      {
        GtkWidget * button = gtk_button_new_with_label( button_labels[i] );
        if(i==0)
          gtk_signal_connect (GTK_OBJECT (button), "clicked",
                              GTK_SIGNAL_FUNC (start_tournament_start_callback), (gpointer) NULL );          
        if(i==1)
          gtk_signal_connect (GTK_OBJECT (button), "clicked",
                              GTK_SIGNAL_FUNC (start_tournament_button_callback), (gpointer) NULL );
        gtk_box_pack_start( GTK_BOX(vbox2), button, TRUE, FALSE, 0);
        gtk_widget_show( button );
      }
  }  

  gtk_widget_show( start_tournament_window );
  start_tournament_up = true;
}

void
Gui::close_start_tournament_window()
{
  start_tournament_up = false;
  gtk_widget_destroy ( start_tournament_window );

  g_list_free(selected_items_in_robot_tournament);
  g_list_free(selected_items_in_robot_directory);
  g_list_free(selected_items_in_arena_tournament);
  g_list_free(selected_items_in_arena_directory);
}
