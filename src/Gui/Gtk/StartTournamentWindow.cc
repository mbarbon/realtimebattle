/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony

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
# if HAVE_SYS_TYPES_H
#  include <sys/types.h>
# endif
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

#include <iostream>
#include <string>

#include "IntlDefs.h"
#include "StartTournamentWindow.h"
#include "GuiVarious.h"
#include "Dialog.h"
#include "OptionHandler.h"
#include "Gui.h"

//From src/
#include "String.h"
#include "Various.h"
#include "ArenaController.h"

//From src/Network/
#include "Packets.h"
#include "TournamentAgreementPackets.h"

const string tmp_tournament_file( "/tmp.tour" );

StartTournamentWindow::StartTournamentWindow( const int default_width,
                                              const int default_height,
                                              const int default_x_pos,
                                              const int default_y_pos )
{
  tournament_started_flag = false;
  filesel = NULL;

  //The window widget

  window_p = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  gtk_widget_set_name( window_p, "RTB StartTournament" );

  gtk_window_set_title( GTK_WINDOW( window_p ),
			_("Negociations - Match 1") );

  gtk_container_border_width( GTK_CONTAINER( window_p ), 12 );
  
  if( default_width != -1 && default_height != -1 )
    gtk_window_set_default_size( GTK_WINDOW( window_p ),
				 default_width, default_height );

  if( default_x_pos != -1 && default_y_pos != -1 )
    gtk_widget_set_uposition( window_p, default_x_pos, default_y_pos );

  gtk_signal_connect( GTK_OBJECT( window_p ), "delete_event",
                      (GtkSignalFunc) StartTournamentWindow::delete_event_occured,
                      (gpointer) this );

  // Lists for clists

  list<string> robotdirs;
  list<string> arenadirs;

  read_dirs_from_system( robotdirs, arenadirs );

  // Main box

  GtkWidget* vbox = gtk_vbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_p ), vbox );
  gtk_widget_show( vbox );

  //Create the box for the arena
  GtkWidget* hbox = gtk_hbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( vbox ), hbox );
  gtk_widget_show( hbox );

  //The list of known arenas
  arenas_in_directory_clist = gtk_clist_new_with_titles( 1, new char*( _("Arenas in Directory") ) ) ;

  add_clist( arenas_in_directory_clist, hbox, false );

  //TODO : A preview of the arena (partial parse + show as bitmap ?)
  GtkWidget* arena_pv = gtk_button_new_with_label( _("Arena Preview") );
  gtk_container_add( GTK_CONTAINER( hbox ), arena_pv);
  gtk_widget_show( arena_pv );

  //Informations for the arena (partial parse -> type, max team, max robots, time, ...)
  GtkWidget* arena_info = gtk_button_new_with_label( _("Arena Info") );
  gtk_container_add( GTK_CONTAINER( hbox ), arena_info);
  gtk_widget_show( arena_info );

  // Make a horizontal ruler between robots and arenas
  GtkWidget* hsep = gtk_hseparator_new();
  gtk_box_pack_start( GTK_BOX( vbox ), hsep, FALSE, FALSE, 0 );
  gtk_widget_show( hsep );

  //The HBox for the robots
  hbox = gtk_hbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( vbox ), hbox );
  gtk_widget_show( hbox );
  
  //The list of robots in the tournament
  robots_in_tournament_clist = gtk_clist_new_with_titles( 1, new char*( _("Robots in Tournament") ) );
  gtk_clist_set_selection_mode( GTK_CLIST( robots_in_tournament_clist ),
                                GTK_SELECTION_MULTIPLE );

  add_clist( robots_in_tournament_clist, hbox, true );

  //buttons between both robot lists
  char* button_labels[] = { _(" Remove "), _(" Select All "), _(" Unselect All "), 
			    _(" Next\nTeam "), _(" No Team "), _(" Previous\nTeam "), 
			    _(" Add "),    _(" Select All "), _(" Unselect All ") };
  
  GtkWidget* vbox2 = NULL;

  for( int i=START_TORUNAMENT_REMOVE;
       i<= START_TORUNAMENT_UNSELECT_ALL_DIRECTORY; i++ )
    {
      if( i == START_TORUNAMENT_REMOVE ||
	  i == START_TORUNAMENT_NEXT_TEAM ||
	  i == START_TORUNAMENT_ADD )
	{
	  vbox2 = gtk_vbox_new( FALSE, 10 );
	  gtk_box_pack_start( GTK_BOX( hbox ), vbox2, FALSE, FALSE, 0 );
	  gtk_widget_show( vbox2 );
	}
      GtkWidget* button = gtk_button_new_with_label( button_labels[i] );
      
      if( i == START_TORUNAMENT_THE_TEAM )  //Store button, because we need to change its label/color
	{
	  the_team_button = button;
	}

      struct select_buttons_t* s_button;
      
      s_button = new select_buttons_t( i, this );
      gtk_signal_connect( GTK_OBJECT( button ), "clicked",
			  (GtkSignalFunc) StartTournamentWindow::button_selected,
			  (gpointer) s_button );
      gtk_box_pack_start( GTK_BOX( vbox2 ), button, TRUE, FALSE, 0 );
      gtk_widget_show( button );
    }
  
  robots_in_directory_clist = gtk_clist_new_with_titles( 1, new char*( _("Robots in directory") ) );

  add_clist( robots_in_directory_clist, hbox, true );


  //Fill the directory lists
  
  for( int j = 0; j < 2; j++ )
    {
      list<start_tournament_info_t>* dir_list;
      GtkWidget* dir_clist;
      bool robot;

      list<string>::const_iterator li;
      list<string>::const_iterator li_end;

      if( j == 0 )  //filling the list for the robots
        {
          dir_list  = &selected_robot_directory;
          dir_clist  = robots_in_directory_clist;
          robot = true;

          li = robotdirs.begin();
          li_end = robotdirs.end();
        }
      else          //filling the list of arenas
        {
          dir_list  = &selected_arena_directory;
          dir_clist  = arenas_in_directory_clist;
          robot = false;

          li = arenadirs.begin();
          li_end = arenadirs.end();
        }

      for( ; li != li_end ; li++ )
        {
          DIR* dir;
          if( NULL != (dir = opendir((*li).c_str())))
            {
	      
              struct dirent * entry;
              while (NULL != ( entry = readdir( dir ) ) )
                {
                  string full_file_name = (*li) + entry->d_name;
                  if( ( robot && check_if_filename_is_robot( full_file_name ) ) ||
                      ( !robot && check_if_filename_is_arena( full_file_name ) ) )
                    {
                      char* lst[] = { entry->d_name };

                      int row = gtk_clist_append( GTK_CLIST( dir_clist ), lst );
#if GTK_MAJOR_VERSION != 1 || GTK_MINOR_VERSION > 1
                      gtk_clist_set_foreground( GTK_CLIST( dir_clist ), row, 
                                                gui_p->get_fg_gdk_colour_p());
                      gtk_clist_set_background( GTK_CLIST( dir_clist ), row,
						gui_p->get_bg_gdk_colour_p());
#endif
                      dir_list->push_back( start_tournament_info_t
                                           ( row, false, entry->d_name,
                                             (*li).c_str() ) );
                    }
                }
              closedir( dir );
            }
        }
    }

  // A Check button to use the same set of robots all the time
  GtkWidget* check_button = gtk_check_button_new_with_label( _("use this robot set for all matches") );
  gtk_signal_connect( GTK_OBJECT( check_button ), "toggled",
		      (GtkSignalFunc) StartTournamentWindow::switch_robot_set_const,
		      (gpointer) this );

  gtk_box_pack_start( GTK_BOX( vbox ), check_button, FALSE, FALSE, 0 );
  gtk_widget_show( check_button );

  //Separator
  hsep = gtk_hseparator_new();
  gtk_box_pack_start( GTK_BOX( vbox ), hsep, FALSE, FALSE, 0 );
  gtk_widget_show( hsep );
  
  // Choose Number of games per sequence, Number of robots per sequence
  // and Number of sequences
  // NOTE : useless now that arena definition is more complet

  char * label_titles[] = { _("Games per sequence"), _("Robots per sequence"),
                            _("Number of sequences") };

  GtkWidget *hbox2 = gtk_hbox_new (FALSE, 10);
  gtk_box_pack_start (GTK_BOX (vbox), hbox2, FALSE, FALSE, 0);
  gtk_widget_show (hbox2);

  GtkWidget * internal_hbox = gtk_hbox_new( FALSE, 10 );
  gtk_box_pack_start (GTK_BOX (hbox2), internal_hbox, TRUE, FALSE, 0);
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

      switch(i)
        {
        case 0:
        case 2:
          gtk_entry_set_text( GTK_ENTRY( entries[i] ), "1" );
          break;
        case 1:
          gtk_entry_set_text( GTK_ENTRY( entries[i] ), "2" );
          break;
        }

      // TODO: use another entry_handler!
//        entry_t * info = new entry_t( ENTRY_INT, false );

//        gtk_signal_connect(GTK_OBJECT(entries[i]), "changed",
//                           (GtkSignalFunc) entry_handler, info);
      gtk_table_attach_defaults( GTK_TABLE( entry_table ),
                                 entries[i], 0, 1, i, i + 1 );
      gtk_widget_set_usize(entries[i], 36,18);
      gtk_widget_show(entries[i]);

      GtkWidget * button;
      int add = 0;
      if( i == 1 ) add = 3;

      button = gtk_button_new_with_label (_(" Min "));

      min_max_full_buttons_t* mmf_button;
      mmf_button = new min_max_full_buttons_t( i, MMF_MIN, this );
      gtk_signal_connect (GTK_OBJECT (button), "clicked",
                          (GtkSignalFunc) StartTournamentWindow::set_entry,
                          (gpointer) mmf_button );
      gtk_table_attach_defaults( GTK_TABLE( button_table ), button, 0, 3 + add, i, i + 1 );
      gtk_widget_show (button);

      if( i == 0 )
        {
          button = gtk_button_new_with_label (_(" All Arenas "));
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
          button = gtk_button_new_with_label (_(" Full Round "));
          mmf_button = new min_max_full_buttons_t( i, MMF_FULL_ROUND,
                                                   this );
          gtk_signal_connect( GTK_OBJECT( button ), "clicked",
                              (GtkSignalFunc) StartTournamentWindow::set_entry,
                              (gpointer) mmf_button );
          gtk_table_attach_defaults( GTK_TABLE( button_table ), button, 3, 9, i, i + 1 );
          gtk_widget_show( button );
        }

      button = gtk_button_new_with_label (_(" Max "));
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

  GtkWidget* vsep = gtk_vseparator_new();
  gtk_box_pack_start( GTK_BOX( hbox2 ), vsep, FALSE, FALSE, 0 );
  gtk_widget_show( vsep );

  vbox2 = gtk_vbox_new( FALSE, 10 );
  gtk_box_pack_start( GTK_BOX(hbox2), vbox2, TRUE, FALSE, 0);
  gtk_widget_show (vbox2);

  {
    char * button_labels[] = { _(" Next Match "), _(" Remove "), _(" Prev Match ") };
    for(int i=0;i<3;i++)
      {
        GtkWidget* button = gtk_button_new_with_label( button_labels[i] );
	struct select_buttons_t* selected_button = new select_buttons_t(i, this);

	gtk_signal_connect( GTK_OBJECT( button ), "clicked",
			    (GtkSignalFunc) StartTournamentWindow::match_selection,
			    (gpointer) selected_button );          

        gtk_box_pack_start( GTK_BOX(vbox2), button, TRUE, FALSE, 0);
        gtk_widget_show( button );
      }
  }  
  vbox2 = gtk_vbox_new( FALSE, 10 );
  gtk_box_pack_start( GTK_BOX(hbox2), vbox2, TRUE, FALSE, 0);
  gtk_widget_show (vbox2);

  {
    char * button_labels[] = { _(" Load tournament "), _(" Save tournament ") };
    for(int i=0;i<2;i++)
      {
        GtkWidget* button = gtk_button_new_with_label( button_labels[i] );
        if(i==0)
          gtk_signal_connect( GTK_OBJECT( button ), "clicked",
                              (GtkSignalFunc) StartTournamentWindow::load_tournament_selected,
                              (gpointer) this );          
        if(i==1)
          gtk_signal_connect( GTK_OBJECT( button ), "clicked",
                              (GtkSignalFunc) StartTournamentWindow::save_tournament_selected,
                              (gpointer) this );
        gtk_box_pack_start( GTK_BOX(vbox2), button, TRUE, FALSE, 0);
        gtk_widget_show( button );
      }
  }  

  vbox2 = gtk_vbox_new( FALSE, 10 );
  gtk_box_pack_end( GTK_BOX(hbox2), vbox2, TRUE, FALSE, 0);
  gtk_widget_show (vbox2);

  {
    char * button_labels[] = { _(" Start "), _(" Cancel ") };
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
  
  load_tournament_file( the_opts.get_s( OPTION_TMP_RTB_DIR ) +
                        tmp_tournament_file, false );

  gtk_widget_show( window_p );
  
  //Create a first match for the tournament
  match_info_t m;
  my_tournament_info.matches.push_back( m );
  my_tournament_info.current_match = my_tournament_info.matches.begin();

}

StartTournamentWindow::~StartTournamentWindow()
{
  gtk_widget_destroy( window_p );
}

//////////////////////////////
// Handle a packet coming from the network
//////////////////////////////
int
StartTournamentWindow::handle_packet(TournamentCommitChangePacket * p)
{//If we come here, it means that the packet has been accepted
  //TODO : Not so much work actually
  /*
  cout<<"Handling packet at the good level !!! \n";
  gtk_clist_freeze(GTK_CLIST(robots_in_tournament_clist));
  gtk_clist_clear(GTK_CLIST(robots_in_tournament_clist));

  char * list[] = { "" };
  
  int row = gtk_clist_append( GTK_CLIST( robots_in_tournament_clist ), list );
#if GTK_MAJOR_VERSION != 1 || GTK_MINOR_VERSION > 1
  gtk_clist_set_foreground( GTK_CLIST( robots_in_tournament_clist ), row,
			    gui_p->get_fg_gdk_colour_p() );
  gtk_clist_set_background( GTK_CLIST( robots_in_tournament_clist ), row,
			    gui_p->get_bg_gdk_colour_p() );
#endif

  char* cstr = copy_to_c_string( (string)"*" + p->value() );
  gtk_clist_set_text( GTK_CLIST( robots_in_tournament_clist ), row, 0, cstr );
  delete [] cstr;

  gtk_clist_thaw(GTK_CLIST(robots_in_tournament_clist));
  */
  return 0;
}

void
StartTournamentWindow::add_clist( GtkWidget* clist, GtkWidget* box, bool robot)
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

  if( robot )
    {
      gtk_clist_set_selection_mode( GTK_CLIST( clist ), GTK_SELECTION_MULTIPLE );
      gtk_signal_connect( GTK_OBJECT( clist ), "select_row",
			  (GtkSignalFunc) StartTournamentWindow::selection_made, this );
      gtk_signal_connect( GTK_OBJECT( clist ), "unselect_row",
			  (GtkSignalFunc) StartTournamentWindow::selection_made, this );
    }
  else
    {
      gtk_clist_set_selection_mode( GTK_CLIST( clist ), GTK_SELECTION_SINGLE );
      gtk_signal_connect( GTK_OBJECT( clist ), "select_row",
			  (GtkSignalFunc) StartTournamentWindow::arena_selection, this );
    }

  gtk_clist_set_column_width( GTK_CLIST( clist ), 0, 90 );
  gtk_clist_set_column_justification( GTK_CLIST( clist ),
                                      0, GTK_JUSTIFY_LEFT);
  gtk_clist_column_titles_passive( GTK_CLIST( clist ) );

#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  gtk_clist_set_shadow_type( GTK_CLIST( clist ),
                             GTK_SHADOW_IN );
  gtk_widget_set_usize( clist, all_clists_width,
                        all_clists_height );
  gtk_container_add( GTK_CONTAINER( scrolled_win ),
                     clist );

  GtkStyle* clist_style = gtk_rc_get_style(window_p);
  if( clist_style == NULL )
    clist_style = gtk_style_new();
  else
    clist_style = gtk_style_copy(clist_style);
  clist_style->base[GTK_STATE_NORMAL] = *(gui_p->get_bg_gdk_colour_p());
  clist_style->base[GTK_STATE_ACTIVE] = make_gdk_colour( 0xffffff );
  clist_style->bg[GTK_STATE_SELECTED] = make_gdk_colour( 0xf0d2b4 );
  clist_style->fg[GTK_STATE_SELECTED] = *(gui_p->get_fg_gdk_colour_p());
  gtk_widget_set_style( clist, clist_style );
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
  gui_p->close_starttournamentwindow();
}

void
StartTournamentWindow::cancel_new_tournament( GtkWidget* widget,
                                              class StartTournamentWindow* stw_p )
{
  gui_p->close_starttournamentwindow();
}

void
StartTournamentWindow::load_tournament_selected( GtkWidget* widget,
                                                 class StartTournamentWindow* stw_p )
{
  if( stw_p->get_filesel() != NULL )
    return;

  GtkWidget* fs = gtk_file_selection_new( _("Choose a tournament file to load") );
  gtk_signal_connect( GTK_OBJECT( fs ), "destroy",
                      (GtkSignalFunc) StartTournamentWindow::destroy_filesel,
                      (gpointer) stw_p );
  gtk_signal_connect
    ( GTK_OBJECT( GTK_FILE_SELECTION( fs )->cancel_button ), "clicked",
      (GtkSignalFunc) StartTournamentWindow::destroy_filesel,
      (gpointer) stw_p );
  gtk_signal_connect
    ( GTK_OBJECT( GTK_FILE_SELECTION( fs )->ok_button ), "clicked",
      (GtkSignalFunc) StartTournamentWindow::load_file_selected,
      (gpointer) stw_p );
  gtk_widget_show( fs );
  stw_p->set_filesel( fs );
}

void
StartTournamentWindow::load_file_selected( GtkWidget* widget,
                                           class StartTournamentWindow* stw_p )
{
  stw_p->load_tournament_file
    ( string( gtk_file_selection_get_filename
              ( GTK_FILE_SELECTION( stw_p->get_filesel() ) ) ),
      true );
  destroy_filesel( stw_p->get_filesel(), stw_p );
}

void
StartTournamentWindow::save_tournament_selected( GtkWidget* widget,
                                                 class StartTournamentWindow* stw_p )
{
  if( stw_p->get_filesel() != NULL )
    return;

  GtkWidget* fs = gtk_file_selection_new( _("Choose file to save tournament into") );
  gtk_signal_connect( GTK_OBJECT( fs ), "destroy",
                      (GtkSignalFunc) StartTournamentWindow::destroy_filesel,
                      (gpointer) stw_p );
  gtk_signal_connect
    ( GTK_OBJECT( GTK_FILE_SELECTION( fs )->cancel_button ), "clicked",
      (GtkSignalFunc) StartTournamentWindow::destroy_filesel,
      (gpointer) stw_p );
  gtk_signal_connect
    ( GTK_OBJECT( GTK_FILE_SELECTION( fs )->ok_button ), "clicked",
      (GtkSignalFunc) StartTournamentWindow::save_file_selected,
      (gpointer) stw_p );
  gtk_widget_show( fs );
  stw_p->set_filesel( fs );
}

void
StartTournamentWindow::save_file_selected( GtkWidget* widget,
                                           class StartTournamentWindow* stw_p )
{
  stw_p->save_tournament_file
    ( string( gtk_file_selection_get_filename
             ( GTK_FILE_SELECTION( stw_p->get_filesel() ) ) ),
      true, true );
  destroy_filesel( stw_p->get_filesel(), stw_p );
}

void
StartTournamentWindow::destroy_filesel( GtkWidget* widget,
                                        class StartTournamentWindow* stw_p )
{
  gtk_widget_destroy( stw_p->get_filesel() );
  stw_p->set_filesel( NULL );
}

void
StartTournamentWindow::load_tournament_file( const string& full_filename,
                                             bool display_fail_message )
{
//    if(!parse_tournament_file( full_filename,
//                               (StartTournamentFunction)
//                               StartTournamentWindow::new_tournament_from_tournament_file,
//                               this, false ) && display_fail_message )
//      {
//        string error_msg( _("Error in specified tournament file.") );
//        list<string> button_list;
//        button_list.push_back( string( _(" Ok ") ) );
//        string info_text = (string)_("Tournament could not be loaded.") + string("\n")
//          + error_msg;
//        Dialog( info_text, button_list, 
//                (DialogFunction) StartTournamentWindow::dummy_result );
//      }
}

void
StartTournamentWindow::new_tournament_from_tournament_file
( list<start_tournament_info_t>& robotfilename_list, 
  list<start_tournament_info_t>& arenafilename_list, 
  const int robots_p_game, const int games_p_sequence, const int n_o_sequences,
  StartTournamentWindow* stw_p )
{
  stw_p->new_tournament( robotfilename_list, arenafilename_list, robots_p_game,
                         games_p_sequence, n_o_sequences );
}

void 
StartTournamentWindow::
switch_robot_set_const   ( GtkWidget* widget,
			   class StartTournamentWindow* stw_p )
{
  stw_p->get_tournament_info()->keep_same_robot_set = 
    GTK_TOGGLE_BUTTON(widget)->active;
}

void
StartTournamentWindow::
new_tournament( list<start_tournament_info_t>& robotfilename_list, 
                list<start_tournament_info_t>& arenafilename_list, 
                const int robots_p_game, 
                const int games_p_sequence, 
                const int n_o_sequences )
{
  /*
  for( int i = 0; i<2; i++ )
    {
      GtkWidget* tour_clist;
      list<start_tournament_info_t>* tour_list;
      bool robot;
      if( i == 0 )
        {
          tour_list = &selected_robot_tournament;
          tour_clist = robots_in_tournament_clist;
          robot = true;
        }
      else
        {
          tour_list = &selected_arena_tournament;
          tour_clist = arenas_in_tournament_clist;
          robot = false;
        }

      gtk_clist_clear( GTK_CLIST( tour_clist ) );
      tour_list->clear();

      list<start_tournament_info_t>::iterator li;
      list<start_tournament_info_t>::iterator li_end;
      if( robot )
        {
          li = robotfilename_list.begin();
          li_end = robotfilename_list.end();
        }
      else
        {
          li = arenafilename_list.begin();
          li_end = arenafilename_list.end();
        }

      for( ; li != li_end; li++ )
        {
          //          start_tournament_info_t* info = li();

          char* lst[] = { "" };

          int row = gtk_clist_append( GTK_CLIST( tour_clist ), lst );
#if GTK_MAJOR_VERSION != 1 || GTK_MINOR_VERSION < 1
          gtk_clist_set_foreground( GTK_CLIST( tour_clist ), row, 
                                    gui_p->get_fg_gdk_colour_p());
          gtk_clist_set_background( GTK_CLIST( tour_clist ), row, 
                                    gui_p->get_bg_gdk_colour_p());
#endif
      
          string fname = (*li).filename;
          string::size_type charpos = 0;
          if( (charpos = fname.rfind('/')) != string::npos )
            fname = fname.substr( charpos+1, string::npos );
          else
            gui_p->error(true, "Incomplete arena file path" + fname,
                  "StartTournamentWindow::new_tournament");
      
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
          gtk_clist_set_text( GTK_CLIST( tour_clist ),
                              row, 0, fname.c_str() );
#else // TODO: Should we really do this test?
          char* cstr = copy_to_c_string( fname );
          gtk_clist_set_text( GTK_CLIST( tour_clist ), row, 0, cstr );
          delete [] cstr;
#endif
          (*li).selected = false;
          (*li).row = row;
          tour_list->push_back( start_tournament_info_t( (*li) ) );
        }
    }

  const char* str = int2chars(robots_p_game);
  gtk_entry_set_text( GTK_ENTRY( entries[1] ), str );
  delete [] str;
  str = int2chars(games_p_sequence);
  gtk_entry_set_text( GTK_ENTRY( entries[0] ), str );
  delete [] str;
  str = int2chars(n_o_sequences);
  gtk_entry_set_text( GTK_ENTRY( entries[2] ), str );
  delete [] str;
  */
}

void
StartTournamentWindow::save_tournament_file( const string& full_filename,
                                             bool display_file_fail_message,
                                             bool display_tour_fail_message )
{
  /*
  int value[3];
  int robot_number = get_selected_robot_tournament()->size();

  for( int i = 0; i < 3; i++ )
    {
      int min_value;
      int max_value;

      if(i != 1)
        max_value = 10000;
      else
        {
          max_value = min( the_opts.get_l( OPTION_MAX_ROBOTS_ALLOWED ),
                           (long int)robot_number );
        }
      if(i != 1)
        min_value = 1;
      else
        min_value = 2;

      value[i] = string2int( gtk_entry_get_text( GTK_ENTRY( get_entries()[i] ) ) );

      value[i] = min( max_value, value[i] );
      value[i] = max( min_value, value[i] );
    }

  ofstream file(full_filename.c_str(), ios::out);
  if( robot_number > 1 && file &&
      !( selected_arena_tournament.empty() ) )
    {
      file << "Robots: " << endl;
      list<start_tournament_info_t>::const_iterator li;
      for( li = selected_robot_tournament.begin();
           li != selected_robot_tournament.end(); li++ )
        file << (*li).filename << " " << endl;

      file << "Arenas: " << endl;
      for( li = selected_arena_tournament.begin();
           li != selected_arena_tournament.end(); li++ )
        file << (*li).filename << " " << endl;

      file << "Robots/Sequence: " << value[1] << endl;
      file << "Games/Sequence: " << value[0] << endl;
      file << "Sequences: " << value[2] << endl;
    }
  else if( display_file_fail_message || display_tour_fail_message )
    {
      string error_msg( "" );
      if( display_tour_fail_message )
        {
          if( robot_number <= 1 )
            error_msg += string("\n") + _("There are too few robots in the tournament.");
          if( selected_arena_tournament.empty() )
            error_msg += string("\n") + _("There are no arenas in the tournament.");
        }
      if( display_file_fail_message && !file )
        error_msg += string("\n") + _("Could not open file.");

      if( error_msg != "" )
        {
          list<string> button_list;
          button_list.push_back( string( _(" Ok ") ) );
          string info_text = (string)_("Tournament could not be saved.") + error_msg;
          Dialog( info_text, button_list, 
                  (DialogFunction) StartTournamentWindow::dummy_result );
        }
    }
  */
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
            int number_of_robots = mmf_p->stw_p->get_selected_robot_tournament()->size();
            if( number_of_robots > the_opts.get_l( OPTION_MAX_ROBOTS_ALLOWED ) )
              number_of_robots = the_opts.get_l( OPTION_MAX_ROBOTS_ALLOWED );
            const char* str = int2chars(number_of_robots);
            gtk_entry_set_text
              ( GTK_ENTRY( mmf_p->stw_p->get_entries()[mmf_p->entry] ), str );
            delete [] str;
          }
      }
      break;
    case MMF_FULL_ROUND:
      {
        int number_of_robots = mmf_p->stw_p->
          get_selected_robot_tournament()->size();

        int robots_per_sequence = 
          string2int( gtk_entry_get_text
                   ( GTK_ENTRY( mmf_p->stw_p->get_entries()[1] ) ) );

        if( number_of_robots < robots_per_sequence )
          robots_per_sequence = number_of_robots;
        if( robots_per_sequence > the_opts.get_l( OPTION_MAX_ROBOTS_ALLOWED ) )
          robots_per_sequence = the_opts.get_l( OPTION_MAX_ROBOTS_ALLOWED );
        if( robots_per_sequence < 2 )
          robots_per_sequence = 2;
        const char* str = int2chars( min( 9999, binomial( number_of_robots,
                                                          robots_per_sequence ) ) );
        gtk_entry_set_text
          ( GTK_ENTRY( mmf_p->stw_p->get_entries()[mmf_p->entry] ), str );
        delete [] str;
      }
      break;
    case MMF_ALL_ARENAS:
      {
        int number_of_arenas = 1; //TODO : remove this lines ? 
	  //mmf_p->stw_p->get_selected_arena_tournament()->size();

        if( number_of_arenas > the_opts.get_l( OPTION_MAX_ROBOTS_ALLOWED ) )
          number_of_arenas = the_opts.get_l( OPTION_MAX_ROBOTS_ALLOWED );
        if( number_of_arenas < 1 )
          number_of_arenas = 1;
        const char* str = int2chars( number_of_arenas );
        gtk_entry_set_text
          ( GTK_ENTRY( mmf_p->stw_p->get_entries()[mmf_p->entry] ), str );
        delete [] str;
      }
      break;
    }
}

void
StartTournamentWindow::start( GtkWidget* widget,
                              class StartTournamentWindow* stw_p )
{
  if( stw_p->get_tournament_started_flag() == true )
    return;

  if( the_arena_controller.is_started() )
    //if( the_arena.get_state() != NOT_STARTED && the_arena.get_state() != FINISHED )
    return;

  //  int value[3];
  int robot_number = stw_p->get_selected_robot_tournament()->size();

  /*
    for( int i = 0; i < 3; i++ )
    {
    int min_value;
    int max_value;
    
    //if(i != 1)  //TODO : make this option available
    max_value = 10000;
    //else
    // {
    //  max_value = min( the_opts.get_l( OPTION_MAX_ROBOTS_ALLOWED ),
    //                   (long int) robot_number );
    //	     }
    if(i != 1)
    min_value = 1;
    else
    min_value = 2;
    
    value[i] = string2int( gtk_entry_get_text( GTK_ENTRY( stw_p->get_entries()[i] ) ) );
    
    value[i] = min( max_value, value[i] );
    value[i] = max( min_value, value[i] );
    }
  
    vector<string> _robots, _arenas;
    for(list<start_tournament_info_t>::iterator li =  ( stw_p->get_selected_robot_tournament() )->begin();
    li !=  ( stw_p->get_selected_robot_tournament() )->end(); li ++)
    {
    _robots.push_back(li->directory + li->filename);
    cout<<"Robot : "<<li->directory + li->filename<<endl;
    }
    for(list<start_tournament_info_t>::iterator li =  ( stw_p->get_selected_arena_tournament() )->begin();
    li !=  ( stw_p->get_selected_arena_tournament() )->end(); li ++)
    {
    _arenas.push_back(li->directory + li->filename);
    cout<<"Arena : "<<li->directory + li->filename<<endl;
    }
  */

  //NOTE : why can't a robot play alone ?
  //TODO : check there is at least one robot and one arena for each match
  //if( robot_number > 1 &&
  //    !( stw_p->get_selected_arena_tournament()->size() ) )
  if( true )
    {
      cout<<"Everything seems correct to start the tournament\n";
      stw_p->set_tournament_started_flag( true );
      //the_arena_controller.start_realtime_arena();
      //        realtime_arena.start_tournament( *( stw_p->get_selected_robot_tournament() ),
      //                              *( stw_p->get_selected_arena_tournament() ),
      //                               value[1], value[0], value[2] );
      
      gui_p->apply_request( new StartTournamentRequest( *(stw_p->get_tournament_info()) ) );
						      

      // create the tmp rtb dir if it exists and save the current tournament there
      create_tmp_rtb_dir();
      stw_p->save_tournament_file( the_opts.get_s( OPTION_TMP_RTB_DIR ) +
				   tmp_tournament_file, false, false );
      
      // close down StartTournamentWindow
      gui_p->close_starttournamentwindow();
    }
  else
    {
      string error_msg( "" );
      if( robot_number <= 1 )
	error_msg += string("\n") + _("There must be at least two robots in the tournament.");
      //if( stw_p->selected_arena_tournament.size() )
      //error_msg += string("\n") + _("There are no arenas in the tournament.");
      
      if( error_msg != "" )
	{
	  list<string> button_list;
	  button_list.push_back( string( _(" Ok ") ) );
	  string info_text = _("Tournament could not be started.") + error_msg;
	  Dialog( info_text, button_list, 
		  (DialogFunction) StartTournamentWindow::dummy_result );
	}
    }
}

void
StartTournamentWindow::button_selected( GtkWidget* widget,
                                        struct select_buttons_t* button )
{
  switch( button->button_nr )
    {
    case START_TORUNAMENT_REMOVE:
      button->stw_p->remove_all_selected( );
      break;
    case START_TORUNAMENT_SELECT_ALL_TOURNAMENT:
      button->stw_p->change_all_selection( false,true );
      break;
    case START_TORUNAMENT_UNSELECT_ALL_TOURNAMENT:
      button->stw_p->change_all_selection( false,false );
      break;
      //    case START_TORUNAMENT_NEXT_TEAM:
      //    button->stw_p->the_team_button->label = next_label
      //    case START_TORUNAMENT_PREV_TEAM:
      //    case START_TORUNAMENT_THE_TEAM : switch the color for the team ?

    case START_TORUNAMENT_ADD:
      button->stw_p->add_all_selected( );
      break;
    case START_TORUNAMENT_SELECT_ALL_DIRECTORY:
      button->stw_p->change_all_selection( true,true );
      break;
    case START_TORUNAMENT_UNSELECT_ALL_DIRECTORY:
      button->stw_p->change_all_selection( true,false );
      break;
    }
}

/* match_selection : move to next match to change its setting */ 
void
StartTournamentWindow::match_selection( GtkWidget* widget,
					struct select_buttons_t* button )
{
  tourn_info_t* my_t_info = button->stw_p->get_tournament_info();
  
  //TODO : when changing, set the arena to the first arena of the list if it is a new match


  if( button->button_nr == 0 )  //Move to next match
    {
      my_t_info->current_match++;

      if(my_t_info->current_match == my_t_info->matches.end())
	{
	  match_info_t m;
	  my_t_info->matches.push_back( m );
	  my_t_info->current_match --;
	}
    }
  else if( button->button_nr == 1 ) //Remove the match
    {
      cout<<"Remove this match\n"; 
      if( my_t_info->matches.size() == 1 ) //I need at least one match to run a tournament
	return;
      list<match_info_t>::iterator to_remove = my_t_info->current_match;
      if(my_t_info->current_match != my_t_info->matches.begin())
	{  //Place the current match on the previous match ...
	  my_t_info->current_match --;
	}
      else
	{  //... Except if we remove the first match
	  my_t_info->current_match ++;                    
	  if( my_t_info->keep_same_robot_set )    //And then we need to copy the robot set
	    my_t_info->current_match->robots = to_remove->robots;
	}
      my_t_info->matches.erase( to_remove );     //Now we can remove the object
    }
  else if( button->button_nr == 2 ) //Move to previous match
    {
      cout<<"Move to previous match\n";
      if( my_t_info->current_match != my_t_info->matches.begin() )
	{
	  my_t_info->current_match--;
	}
      else
	return;
    }
  else return;

  button->stw_p->update_clists();
}

void
StartTournamentWindow::change_all_selection( const bool dir,
                                             const bool all )
{
  if( dir )
    {
      list<start_tournament_info_t>::iterator li;
      list<start_tournament_info_t>* info_list = &selected_robot_directory;
      GtkWidget * clist = robots_in_directory_clist;

      gtk_clist_freeze( GTK_CLIST( clist ) );
      
      for( li = info_list->begin(); li != info_list->end(); li++ )
	{
	  (*li).selected = all;
	  if( all )
	    gtk_clist_select_row( GTK_CLIST( clist ), (*li).row, 0);
	  else
	    gtk_clist_unselect_row( GTK_CLIST( clist ), (*li).row, 0);
	}
      gtk_clist_thaw( GTK_CLIST( clist ) );
    }
  else
    {
      list<robot_info_t>::iterator li;
      list<robot_info_t>* info_list = &(my_tournament_info.current_match->robots);
      GtkWidget * clist = robots_in_tournament_clist;

      gtk_clist_freeze( GTK_CLIST( clist ) );

      for( li = info_list->begin(); li != info_list->end(); li++ )
	{
	  (*li).selected = all;
	  if( all )
	    gtk_clist_select_row( GTK_CLIST( clist ), (*li).row, 0);
	  else
	    gtk_clist_unselect_row( GTK_CLIST( clist ), (*li).row, 0);
	}
      gtk_clist_thaw( GTK_CLIST( clist ) );
    }
}

void
StartTournamentWindow::add_all_selected( )
{
  list<start_tournament_info_t>::const_iterator li;
  list<start_tournament_info_t>* info_dir_list = &selected_robot_directory;
  list<robot_info_t>* info_tourn_list = get_selected_robot_tournament();
  GtkWidget * clist_tourn = robots_in_tournament_clist;

  gtk_clist_freeze( GTK_CLIST( clist_tourn ) );
  for( li = info_dir_list->begin(); li != info_dir_list->end(); li++ )
    {
      if( (*li).selected )
        {
          char * list[] = { "" };
          
          int row = gtk_clist_append( GTK_CLIST( clist_tourn ), list );
#if GTK_MAJOR_VERSION != 1 || GTK_MINOR_VERSION > 1
          gtk_clist_set_foreground( GTK_CLIST( clist_tourn ), row,
                                    gui_p->get_fg_gdk_colour_p() );
          gtk_clist_set_background( GTK_CLIST( clist_tourn ), row,
                                    gui_p->get_bg_gdk_colour_p() );
#endif

          char* cstr = copy_to_c_string( (*li).filename );
          gtk_clist_set_text( GTK_CLIST( clist_tourn ), row, 0, cstr );
	  
	  gui_p->apply_request( new BroadCastTournamentChangeRequest("AddRobot", cstr) );

          delete [] cstr;
      
	  info_tourn_list->push_back( robot_info_t( (*li).filename, (*li).directory, row, false, 0 ) ); 
        }
    }
  gtk_clist_thaw( GTK_CLIST( clist_tourn ) );
}

void
StartTournamentWindow::remove_all_selected( )
{ //TODO : use gtk_clist_clear if all item are selected !!!
  list<robot_info_t>::iterator li;
  list<robot_info_t>::iterator li2;
  list<robot_info_t>* info_dir_list = get_selected_robot_tournament();
  GtkWidget * clist_tourn = robots_in_tournament_clist;

  gtk_clist_freeze( GTK_CLIST( clist_tourn ) );
  li = info_dir_list->begin(); 
  while(li != info_dir_list->end())
    {
      if( (*li).selected )
	{
	  gtk_clist_remove(GTK_CLIST(clist_tourn), (*li).row);
	  
	  for( li2 = li; li2 != info_dir_list->end(); li2++ )  //Push the element up in the list
            li2->row = li2->row - 1;
	  
	  li2 = li++;                  //Place the cursor on the next element
	  info_dir_list->erase(li2);   //And remove what is to remove
	}
      else
	li ++;         //Jump to the next element
    }
  gtk_clist_thaw( GTK_CLIST( clist_tourn ) );
}

void
StartTournamentWindow::arena_selection( GtkWidget* clist, gint row,
					gint column, GdkEventButton *event,
					class StartTournamentWindow* stw_p)
{
  if( event == NULL )
    return;

  cout<<"Get the right directory\n";
  list<start_tournament_info_t>* info_list 
    = stw_p->get_selected_arena_directory();

  cout<<"Find the right information\n";
  start_tournament_info_t* info_p = stw_p->find_row_in_clist( row, info_list );

  cout<<"Change the value of the selection\n";
  info_p->selected = !( info_p->selected );

  cout<<"Update the tournament_info"<<info_p->filename<<"\n";
  stw_p->get_tournament_info()->current_match->match_arena.filename = info_p->filename;
  cout<<"Done\n";
}

void
StartTournamentWindow::selection_made( GtkWidget* clist, gint row,
                                       gint column, GdkEventButton *event,
                                       class StartTournamentWindow* stw_p )
{
  if( event == NULL )
    return;

  if(clist == stw_p->get_robots_in_tournament_clist() )
    {
      list<robot_info_t>* info_list = stw_p->get_selected_robot_tournament();
      list<robot_info_t>::iterator li;
      for( li = info_list->begin(); li != info_list->end(); li++ )
	{
	  if( (*li).row == row )
	    {
	      li->selected = !(li->selected);
	      break;
	    }
	}
    }
  else if(clist == stw_p->get_robots_in_directory_clist() )
    {
      list<start_tournament_info_t>* info_list = stw_p->get_selected_robot_directory();
      start_tournament_info_t* info_p = stw_p->find_row_in_clist( row, info_list );
      info_p->selected = !(info_p->selected);
    }
}

start_tournament_info_t*
StartTournamentWindow::find_row_in_clist( const int row,
                                          list<start_tournament_info_t>* info_list )
{
  list<start_tournament_info_t>::iterator li;
  for( li = info_list->begin(); li != info_list->end(); li++ )
    {
      if( (*li).row == row )
        return &(*li);
    }
  return NULL;
}

void
StartTournamentWindow::update_clists()
{
  //Update the window when current_match change

  //TODO : update the arena directory/preview/info

  change_all_selection( true, false );  //unselect everything in the robot directory

  //Find the match number
  int match_nr = 1;
  for(list<match_info_t>::iterator m_li = my_tournament_info.matches.begin();
      m_li != my_tournament_info.matches.end() && m_li != my_tournament_info.current_match;
      m_li ++)
    {
      match_nr ++;
    }

  //Set the name of the window
  char win_name[64];
  sprintf( win_name, "Negociations - Match %d", match_nr );
  gtk_window_set_title( GTK_WINDOW( window_p ),
			_(win_name) );

  //Update robot selection if necessary
  list<robot_info_t>* robots = get_selected_robot_tournament();
  GtkWidget * clist_tourn = get_robots_in_tournament_clist();

  //NOTE : maybe we can use this function to program those 2 :
  //       ie : remove_all_selected just remove the robots from the list and then call 
  //            this function to remove it from the screen

  gtk_clist_freeze( GTK_CLIST( clist_tourn ) );
      
  gtk_clist_clear( GTK_CLIST( clist_tourn ) );

  for( list<robot_info_t>::iterator r_li = robots->begin();
	   r_li != robots->end(); r_li ++ )
    {
      char * list[] = { "" };

      r_li->selected = false;  //Remove the selection if it has been selected previously...

      int row = gtk_clist_append( GTK_CLIST( clist_tourn ), list );
#if GTK_MAJOR_VERSION != 1 || GTK_MINOR_VERSION > 1
      gtk_clist_set_foreground( GTK_CLIST( clist_tourn ), row,
				gui_p->get_fg_gdk_colour_p() );
      gtk_clist_set_background( GTK_CLIST( clist_tourn ), row,
				gui_p->get_bg_gdk_colour_p() );
#endif
      string name = r_li->filename;
      if( r_li->directory.substr(0, 8) == "NetRobot" ) //This is a robot to run through the net
	name += "(*)";
      
      char* cstr = copy_to_c_string( name );
      gtk_clist_set_text( GTK_CLIST( clist_tourn ), row, 0, cstr );
      delete [] cstr;
    }
  gtk_clist_thaw( GTK_CLIST( clist_tourn ) );
}

