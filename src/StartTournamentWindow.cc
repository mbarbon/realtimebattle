/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-1999  Erik Ouchterlony and Ragnar Ouchterlony

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#include "ArenaController.h"
#include "ArenaRealTime.h"
#include "StartTournamentWindow.h"
#include "String.h"
#include "Various.h"
#include "List.h"

StartTournamentWindow::StartTournamentWindow( const int default_width,
                                              const int default_height,
                                              const int default_x_pos,
                                              const int default_y_pos )
{
  // The window widget

  window_p = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  gtk_widget_set_name( window_p, "RTB StartTournament" );

  gtk_window_set_title( GTK_WINDOW( window_p ),
                        "Start New Tournament" );

  gtk_container_border_width( GTK_CONTAINER( window_p ), 12 );

  if( default_width != -1 && default_height != -1 )
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
    gtk_window_set_default_size( GTK_WINDOW( window_p ),
                                 default_width, default_height );
#else
  gtk_widget_set_usize( window_p, default_width, default_height );
#endif
  if( default_x_pos != -1 && default_y_pos != -1 )
    gtk_widget_set_uposition( window_p, default_x_pos, default_y_pos );

  gtk_signal_connect( GTK_OBJECT( window_p ), "delete_event",
                      (GtkSignalFunc) StartTournamentWindow::delete_event_occured,
                      (gpointer) this );

  // Lists for clists

  List<String> robotdirs;
  List<String> arenadirs;

  read_dirs_from_system( robotdirs, arenadirs );

  // Main box

  GtkWidget* vbox = gtk_vbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_p ), vbox );
  gtk_widget_show( vbox );

  // Clists and buttons for selecting robots

  for( int j = 0; j < 2; j++ )
    {
      List<start_tournament_info_t>* dir_list;
      GtkWidget* tour_clist;
      GtkWidget* dir_clist;
      char** tour_title;
      char** dir_title;
      bool robot;

      if( j == 0 )
        {
          dir_list  = &selected_robot_directory;
          tour_clist = robots_in_tournament_clist;
          dir_clist  = robots_in_directory_clist;
          tour_title = new char*( "Robots in Tournament" );
          dir_title  = new char*( "Robots in Directory" );
          robot = true;
        }
      else
        {
          dir_list  = &selected_arena_directory;
          tour_clist = arenas_in_tournament_clist;
          dir_clist  = arenas_in_directory_clist;
          tour_title = new char*( "Arenas in Tournament" );
          dir_title  = new char*( "Arenas in Directory" );
          robot = false;
        }

      GtkWidget* hbox = gtk_hbox_new( FALSE, 10 );
      gtk_container_add( GTK_CONTAINER( vbox ), hbox );
      gtk_widget_show( hbox );

      tour_clist = gtk_clist_new_with_titles( 1, tour_title );

      add_clist( tour_clist, hbox );

      char* button_labels[] = { " Remove ", " Select All ",
                                " Unselect All ", " Add ",
                                " Select All ", " Unselect All " };

      GtkWidget* vbox2 = NULL;

      for( int i=START_TORUNAMENT_REMOVE;
           i<= START_TORUNAMENT_UNSELECT_ALL_DIRECTORY; i++ )
        {
          if( i == START_TORUNAMENT_REMOVE ||
              i == START_TORUNAMENT_ADD )
            {
              vbox2 = gtk_vbox_new( FALSE, 10 );
              gtk_box_pack_start( GTK_BOX( hbox ), vbox2, FALSE, FALSE, 0 );
              gtk_widget_show( vbox2 );
            }
          GtkWidget* button = gtk_button_new_with_label( button_labels[i] );
          struct select_buttons_t* s_button;
          
          s_button = new select_buttons_t( robot, i, this );
          gtk_signal_connect( GTK_OBJECT( button ), "clicked",
                              (GtkSignalFunc) StartTournamentWindow::button_selected,
                              (gpointer) s_button );
          gtk_box_pack_start( GTK_BOX( vbox2 ), button, TRUE, FALSE, 0 );
          gtk_widget_show( button );
        }

      dir_clist = gtk_clist_new_with_titles( 1, dir_title );

      add_clist( dir_clist, hbox );

      ListIterator<String> li;

      if( robot )
        robotdirs.first(li);
      else
        arenadirs.first(li);

      for( ; li.ok() ; li++ )
        {
          String* current_dir = li();
          DIR* dir;
          if( NULL != (dir = opendir(current_dir->chars())))
            {
              struct dirent * entry;
              while (NULL != ( entry = readdir( dir ) ) )
                {
                  String full_file_name = *current_dir + entry->d_name;
                  if( ( robot && check_if_filename_is_robot( full_file_name ) ) ||
                      ( !robot && check_if_filename_is_arena( full_file_name ) ) )
                    {
                      char* lst[] = { "" };

                      int row = gtk_clist_append( GTK_CLIST( dir_clist ), lst );
                      gtk_clist_set_foreground( GTK_CLIST( dir_clist ), row, 
                                                the_gui.get_fg_gdk_colour_p());
                      gtk_clist_set_background( GTK_CLIST( dir_clist ), row, 
                                                the_gui.get_bg_gdk_colour_p());

                      gtk_clist_set_text( GTK_CLIST( dir_clist ),
                                          row, 0, entry->d_name);

                      start_tournament_info_t* info;
                      info = new start_tournament_info_t
                        ( row, false,
                          entry->d_name,
                          current_dir->chars() );
                      dir_list->insert_last( info );
                    }
                }
              closedir( dir );
            }
        }

      if( j == 0 )
        {
          robots_in_tournament_clist = tour_clist;
          robots_in_directory_clist = dir_clist;
        }
      else
        {
          arenas_in_tournament_clist = tour_clist;
          arenas_in_directory_clist = dir_clist;
          robot = false;
        }
    }

  // Choose Number of games per sequence, Number of robots per sequence
  // and Number of sequences

  char * label_titles[] = { "Games per sequence", "Robots per sequence",
                            "Number of sequences" };

  GtkWidget * hbox2 = gtk_hbox_new (FALSE, 10);
  gtk_box_pack_start (GTK_BOX (vbox), hbox2, FALSE, FALSE, 0);
  gtk_widget_show (hbox2);

  GtkWidget * internal_hbox = gtk_hbox_new( FALSE, 10 );
  gtk_box_pack_start (GTK_BOX (hbox2), internal_hbox, FALSE, FALSE, 0);
  gtk_widget_show (internal_hbox);

  GtkWidget * description_table = gtk_table_new( 3, 1, TRUE );
  GtkWidget * entry_table = gtk_table_new( 3, 1, TRUE );
  GtkWidget * button_table = gtk_table_new( 3, 12, TRUE );
  gtk_box_pack_start (GTK_BOX (internal_hbox), description_table,
                      FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (internal_hbox), entry_table,
                      FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (internal_hbox), button_table,
                      FALSE, FALSE, 0);

  for( int i=0;i<3;i++ )
    {
      GtkWidget * internal_hbox = gtk_hbox_new( FALSE, 10 );
      gtk_table_attach_defaults( GTK_TABLE( description_table ),
                                 internal_hbox, 0, 1, i, i + 1 );
      gtk_widget_show (internal_hbox);

      GtkWidget * label = gtk_label_new(label_titles[i]);
      gtk_box_pack_start (GTK_BOX (internal_hbox), label, FALSE, FALSE, 0);
      gtk_widget_show(label);

      entries[i] = gtk_entry_new_with_max_length(4);
      if(i !=1 )
        gtk_entry_set_text( GTK_ENTRY( entries[i] ), "1");
      else
        gtk_entry_set_text( GTK_ENTRY( entries[i] ), "2");

      entry_t * info = new entry_t( ENTRY_INT, false );

      gtk_signal_connect(GTK_OBJECT(entries[i]), "changed",
                         (GtkSignalFunc) entry_handler, info);
      gtk_table_attach_defaults( GTK_TABLE( entry_table ),
                                 entries[i], 0, 1, i, i + 1 );
      gtk_widget_set_usize(entries[i], 36,18);
      gtk_widget_show(entries[i]);

      GtkWidget * button;
      int add = 0;
      if( i == 1 ) add = 3;

      button = gtk_button_new_with_label (" Min ");

      min_max_full_buttons_t* mmf_button;
      mmf_button = new min_max_full_buttons_t( i, MMF_MIN, this );
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          (GtkSignalFunc) StartTournamentWindow::set_entry,
                          (gpointer) mmf_button );
      gtk_table_attach_defaults( GTK_TABLE( button_table ), button, 0, 3 + add, i, i + 1 );
      gtk_widget_show (button);

      if( i == 0 )
        {
          button = gtk_button_new_with_label (" All Arenas ");
          mmf_button = new min_max_full_buttons_t( i, MMF_ALL_ARENAS,
                                                   this );
          gtk_signal_connect (GTK_OBJECT (button), "clicked",
                              (GtkSignalFunc) StartTournamentWindow::set_entry,
                              (gpointer) mmf_button );
          gtk_table_attach_defaults( GTK_TABLE( button_table ), button, 3, 9, i, i + 1 );
          gtk_widget_show (button);
        }

      if( i == 2 )
        {
          button = gtk_button_new_with_label (" Full Round ");
          mmf_button = new min_max_full_buttons_t( i, MMF_FULL_ROUND,
                                                   this );
          gtk_signal_connect( GTK_OBJECT( button ), "clicked",
                              (GtkSignalFunc) StartTournamentWindow::set_entry,
                              (gpointer) mmf_button );
          gtk_table_attach_defaults( GTK_TABLE( button_table ), button, 3, 9, i, i + 1 );
          gtk_widget_show( button );
        }

      button = gtk_button_new_with_label (" Max ");
      mmf_button = new min_max_full_buttons_t( i, MMF_MAX, this );
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          (GtkSignalFunc) StartTournamentWindow::set_entry,
                          (gpointer) mmf_button );
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

  GtkWidget* vbox2 = gtk_vbox_new( FALSE, 10 );
  gtk_box_pack_end( GTK_BOX(hbox2), vbox2, FALSE, FALSE, 0);
  gtk_widget_show (vbox2);

  {
    char * button_labels[] = { " Start ", " Cancel " };
    for(int i=0;i<2;i++)
      {
        GtkWidget* button = gtk_button_new_with_label( button_labels[i] );
        if(i==0)
          gtk_signal_connect( GTK_OBJECT( button ), "clicked",
                              (GtkSignalFunc) StartTournamentWindow::start,
                              (gpointer) this );          
        if(i==1)
          gtk_signal_connect( GTK_OBJECT( button ), "clicked",
                              (GtkSignalFunc) StartTournamentWindow::cancel_new_tournament,
                              (gpointer) this );
        gtk_box_pack_start( GTK_BOX(vbox2), button, TRUE, FALSE, 0);
        gtk_widget_show( button );
      }
  }  

  gtk_widget_show( window_p );
}

StartTournamentWindow::~StartTournamentWindow()
{
  gtk_widget_destroy( window_p );
}

void
StartTournamentWindow::add_clist( GtkWidget* clist, GtkWidget* box )
{
  int all_clists_width=150;
  int all_clists_height= 150;

#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  GtkObject* hadj = gtk_adjustment_new( 0.0, 0.0, 100.0, 1.0, 1.0, 1.0 );
  GtkObject* vadj = gtk_adjustment_new( 0.0, 0.0, 100.0, 1.0, 1.0, 1.0 );
  GtkWidget* scrolled_win =
    gtk_scrolled_window_new( GTK_ADJUSTMENT ( hadj ),
                             GTK_ADJUSTMENT ( vadj ) );
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrolled_win ),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC );
  gtk_box_pack_start( GTK_BOX( box ), scrolled_win, TRUE, TRUE, 5 );
  gtk_widget_show( scrolled_win );
#endif

  gtk_clist_set_selection_mode( GTK_CLIST( clist ),
                                GTK_SELECTION_MULTIPLE );
  gtk_clist_set_column_width( GTK_CLIST( clist ), 0, 90 );
  gtk_clist_set_column_justification( GTK_CLIST( clist ),
                                      0, GTK_JUSTIFY_LEFT);
  gtk_signal_connect( GTK_OBJECT( clist ), "select_row",
                      (GtkSignalFunc) StartTournamentWindow::selection_made, this );
  gtk_signal_connect( GTK_OBJECT( clist ), "unselect_row",
                      (GtkSignalFunc) StartTournamentWindow::selection_made, this );

#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  gtk_clist_set_shadow_type( GTK_CLIST( clist ),
                             GTK_SHADOW_IN );
  gtk_widget_set_usize( clist, all_clists_width,
                        all_clists_height );
  gtk_container_add( GTK_CONTAINER( scrolled_win ),
                     clist );
#else
  gtk_clist_set_border( GTK_CLIST( clist ),
                        GTK_SHADOW_IN );
  gtk_clist_set_policy( GTK_CLIST( clist ),
                        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
  gtk_widget_set_usize( clist,
                        all_clists_width + 20, all_clists_height + 20 );
  gtk_box_pack_start( GTK_BOX( box ), clist,
                      TRUE, TRUE, 0 );
#endif
  gtk_widget_show( clist );
}

void
StartTournamentWindow::delete_event_occured( GtkWidget* widget,
                                             GdkEvent* event,
                                             class StartTournamentWindow* stw_p )
{
  the_gui.close_starttournamentwindow();
}

void
StartTournamentWindow::cancel_new_tournament( GtkWidget* widget,
                                              class StartTournamentWindow* stw_p )
{
  the_gui.close_starttournamentwindow();
}

void
StartTournamentWindow::set_entry( GtkWidget* widget,
                                  struct min_max_full_buttons_t* mmf_p )
{
  switch( mmf_p->type )
    {
    case MMF_MIN:
      {
        if( mmf_p->entry != 1 )
          gtk_entry_set_text( GTK_ENTRY( mmf_p->stw_p->get_entries()[mmf_p->entry] ), "1" );
        else
          gtk_entry_set_text( GTK_ENTRY( mmf_p->stw_p->get_entries()[mmf_p->entry] ), "2" );
      }
      break;
    case MMF_MAX:
      {
        if( mmf_p->entry != 1 )
          gtk_entry_set_text( GTK_ENTRY( mmf_p->stw_p->get_entries()[mmf_p->entry] ), "9999" );
        else
          {
            int number_of_robots = mmf_p->stw_p->
              get_selected_robot_tournament()->number_of_elements();
            if( number_of_robots > 120 )
              number_of_robots = 120;
            gtk_entry_set_text
              ( GTK_ENTRY( mmf_p->stw_p->get_entries()[mmf_p->entry] ),
                String(number_of_robots).chars() );
          }
      }
      break;
    case MMF_FULL_ROUND:
      {
        int number_of_robots = mmf_p->stw_p->
          get_selected_robot_tournament()->number_of_elements();

        int robots_per_sequence = 
          str2int( gtk_entry_get_text
                   ( GTK_ENTRY( mmf_p->stw_p->get_entries()[1] ) ) );

        if( number_of_robots < robots_per_sequence )
          robots_per_sequence = number_of_robots;
        if( robots_per_sequence > 120 )
          robots_per_sequence = 120;
        if( robots_per_sequence < 2 )
          robots_per_sequence = 2;
        gtk_entry_set_text
          ( GTK_ENTRY( mmf_p->stw_p->get_entries()[mmf_p->entry] ),
            String( min ( 9999, binomial( number_of_robots,
                                          robots_per_sequence) ) ).chars() );
      }
      break;
    case MMF_ALL_ARENAS:
      {
        int number_of_arenas = mmf_p->stw_p->
          get_selected_arena_tournament()->number_of_elements();

        if( number_of_arenas > 120 )
          number_of_arenas = 120;
        if( number_of_arenas < 1 )
          number_of_arenas = 1;
        gtk_entry_set_text
          ( GTK_ENTRY( mmf_p->stw_p->get_entries()[mmf_p->entry] ),
            String(number_of_arenas).chars() );
      }
      break;
    }
}

void
StartTournamentWindow::start( GtkWidget* widget,
                              class StartTournamentWindow* stw_p )
{
  if( the_arena_controller.is_started() )
    if( the_arena.get_state() != NOT_STARTED && the_arena.get_state() != FINISHED )
      return;

  int value[3];
  int robot_number = stw_p->get_selected_robot_tournament()->number_of_elements();

  for( int i = 0; i < 3; i++ )
    {
      int min_value;
      int max_value;

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

      value[i] = str2int( gtk_entry_get_text( GTK_ENTRY( stw_p->get_entries()[i] ) ) );

      value[i] = min( max_value, value[i] );
      value[i] = max( min_value, value[i] );
    }
  if( robot_number > 1 &&
      !( stw_p->get_selected_arena_tournament()->is_empty() ) )
    {
      the_arena_controller.start_realtime_arena();
      realtime_arena.start_tournament( *( stw_p->get_selected_robot_tournament() ),
                                       *( stw_p->get_selected_arena_tournament() ),
                                       value[1], value[0], value[2] );

      the_gui.close_starttournamentwindow();
    }
}

void
StartTournamentWindow::button_selected( GtkWidget* widget,
                                        struct select_buttons_t* button )
{
  switch( button->button_nr )
    {
    case START_TORUNAMENT_REMOVE:
      button->stw_p->remove_all_selected( button->robot );
      break;
    case START_TORUNAMENT_SELECT_ALL_TOURNAMENT:
      button->stw_p->change_all_selection( button->robot,false,true );
      break;
    case START_TORUNAMENT_UNSELECT_ALL_TOURNAMENT:
      button->stw_p->change_all_selection( button->robot,false,false );
      break;
    case START_TORUNAMENT_ADD:
      button->stw_p->add_all_selected( button->robot );
      break;
    case START_TORUNAMENT_SELECT_ALL_DIRECTORY:
      button->stw_p->change_all_selection( button->robot,true,true );
      break;
    case START_TORUNAMENT_UNSELECT_ALL_DIRECTORY:
      button->stw_p->change_all_selection( button->robot,true,false );
      break;
    }
}

void
StartTournamentWindow::change_all_selection( const bool robots,
                                             const bool dir,
                                             const bool all )
{
  ListIterator<start_tournament_info_t> li;
  List<start_tournament_info_t>* info_list = NULL;
  GtkWidget * clist = NULL;


  if( robots && dir )
    {
      info_list = get_selected_robot_directory();
      clist = get_robots_in_directory_clist();
    }
  else if(robots && !dir)
    {
      info_list = get_selected_robot_tournament();
      clist = get_robots_in_tournament_clist();
    }
  else if(!robots && dir)
    {
      info_list = get_selected_arena_directory();
      clist = get_arenas_in_directory_clist();
    }
  else if(!robots && !dir)
    {
      info_list = get_selected_arena_tournament();
      clist = get_arenas_in_tournament_clist();
    }
  for( info_list->first(li); li.ok() ; li++ )
    {
      start_tournament_info_t* info_p = li();
      if( all )
        {
          info_p->selected = true;
          gtk_clist_select_row( GTK_CLIST( clist ), info_p->row, 0);
        }
      else
        {
          info_p->selected = false;
          gtk_clist_unselect_row( GTK_CLIST( clist ), info_p->row, 0);
        }
    }
}

void
StartTournamentWindow::add_all_selected( const bool robots )
{
  ListIterator<start_tournament_info_t> li;
  List<start_tournament_info_t>* info_dir_list;
  List<start_tournament_info_t>* info_tourn_list;
  GtkWidget * clist_tourn;

  if(robots)
    {
      info_dir_list = get_selected_robot_directory();
      info_tourn_list = get_selected_robot_tournament();
      clist_tourn = get_robots_in_tournament_clist();
    }
  else
    {
      info_dir_list = get_selected_arena_directory();
      info_tourn_list = get_selected_arena_tournament();
      clist_tourn = get_arenas_in_tournament_clist();
    }

  for( info_dir_list->first(li) ; li.ok() ; li++ )
    {
      start_tournament_info_t* info_dir_p = li();
      if( info_dir_p->selected )
        {
          char * list[] = { "" };
          
          int row = gtk_clist_append( GTK_CLIST( clist_tourn ), list );
          gtk_clist_set_foreground( GTK_CLIST( clist_tourn ), row,
                                    the_gui.get_fg_gdk_colour_p() );
          gtk_clist_set_background( GTK_CLIST( clist_tourn ), row,
                                    the_gui.get_bg_gdk_colour_p() );

          gtk_clist_set_text( GTK_CLIST( clist_tourn ), row, 0,
                              info_dir_p->filename.non_const_chars() );
      
          start_tournament_info_t* info_tourn_p;
          String full_filename;
          if(robots)
            full_filename = info_dir_p->directory + info_dir_p->filename;
          else
            full_filename = info_dir_p->directory + info_dir_p->filename;

          info_tourn_p = new start_tournament_info_t
            ( row, false, full_filename.chars(), info_dir_p->directory );
          info_tourn_list->insert_last( info_tourn_p );
        }
    }
}

void
StartTournamentWindow::remove_all_selected( const bool robots )
{
  ListIterator<start_tournament_info_t> li;
  ListIterator<start_tournament_info_t> li2;
  List<start_tournament_info_t>* info_dir_list;
  GtkWidget * clist_tourn;

  if(robots)
    {
      info_dir_list = get_selected_robot_tournament();
      clist_tourn = get_robots_in_tournament_clist();
    }
  else
    {
      info_dir_list = get_selected_arena_tournament();
      clist_tourn = get_arenas_in_tournament_clist();
    }

  for( info_dir_list->first(li) ; li.ok(); li++ )
    {
      start_tournament_info_t* info_p = li();

      if( info_p->selected )
        {
          gtk_clist_remove(GTK_CLIST(clist_tourn), info_p->row);
          
          for( info_dir_list->first(li2); li2.ok(); li2++ )
            {
              start_tournament_info_t* info2_p = li2();
              if(info2_p->row > info_p->row)
                info2_p->row = info2_p->row - 1;
            }
          info_dir_list->remove(li);
        }
    }
}

void
StartTournamentWindow::selection_made( GtkWidget* clist, gint row,
                                       gint column, GdkEventButton *event,
                                       class StartTournamentWindow* stw_p )
{
  if( event == NULL )
    return;

  List<start_tournament_info_t>* info_list = NULL;

  if(clist == stw_p->get_robots_in_tournament_clist() )
    info_list = stw_p->get_selected_robot_tournament();
  else if(clist == stw_p->get_robots_in_directory_clist() )
    info_list = stw_p->get_selected_robot_directory();
  else if(clist == stw_p->get_arenas_in_tournament_clist() )
    info_list = stw_p->get_selected_arena_tournament();
  else if(clist == stw_p->get_arenas_in_directory_clist() )
    info_list = stw_p->get_selected_arena_directory();

  start_tournament_info_t* info_p;
  info_p = stw_p->find_row_in_clist( row, info_list );
  if( info_p->selected )
    info_p->selected = false;
  else 
    info_p->selected = true;
}

start_tournament_info_t*
StartTournamentWindow::find_row_in_clist( const int row,
                                          List<start_tournament_info_t>* info_list )
{
  ListIterator<start_tournament_info_t> li;
  for( info_list->first(li); li.ok() ; li++ )
    {
      start_tournament_info_t* info_p = li();
      if( info_p->row == row )
        return info_p;
    }
  return NULL;
}
