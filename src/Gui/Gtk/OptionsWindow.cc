/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2000  Erik Ouchterlony and Ragnar Ouchterlony

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
# include <config.h>
#endif

#include <gtk/gtk.h>
#include <fstream.h>
#include <assert.h>
#include <map>
#include <vector>

#include "OptionsWindow.h"
#include "IntlDefs.h"
#include "ArenaWindow.h"
#include "ControlWindow.h"
#include "MessageWindow.h"
#include "ScoreWindow.h"
#include "StatisticsWindow.h"
#include "OptionHandler.h"
#include "Option.h"
#include "GuiVarious.h"
#include "Gui.h"
#include "String.h"
#include "FileSelector.h"

OptionsWindow::OptionsWindow( const int default_width,
                              const int default_height,
                              const int default_x_pos,
                              const int default_y_pos )
{
  // The window widget

  window_p = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  gtk_widget_set_name( window_p, "RTB Options" );

  gtk_window_set_title( GTK_WINDOW( window_p ), _("Options") );

  gtk_container_border_width( GTK_CONTAINER( window_p ), 12 );

  if( default_width != -1 && default_height != -1 )
    gtk_widget_set_usize( window_p, default_width, default_height );
  if( default_x_pos != -1 && default_y_pos != -1 )
    gtk_widget_set_uposition( window_p, default_x_pos, default_y_pos );

  gtk_signal_connect( GTK_OBJECT( window_p ), "delete_event",
                      (GtkSignalFunc) OptionsWindow::delete_event_occured,
                      (gpointer) this );

  // Main box

  GtkWidget* vbox = gtk_vbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_p ), vbox );
  gtk_widget_show( vbox );

  // The notebook with all options

  GtkWidget* notebook = gtk_notebook_new();
  gtk_notebook_set_tab_pos( GTK_NOTEBOOK( notebook ), GTK_POS_LEFT );
  gtk_box_pack_start( GTK_BOX( vbox ), notebook, TRUE, TRUE, 0 );
  gtk_widget_show( notebook );

  vector<page_t> page_list;
  const vector<string> group_names =
    the_gui.get_main_opts()->get_group_names();

  int number_of_options[group_names.size()];
  // TODO: Use all options (not just main_opts)
  const map<string,Option*>& all_options = the_gui.get_main_opts()->get_options();
  map<string,Option*>::const_iterator mci;
  for( mci = all_options.begin(); mci != all_options.end(); mci++ )
    {
      assert( (mci->second->get_group() >= 0) &&
              ((unsigned)mci->second->get_group() < group_names.size()) );
      number_of_options[mci->second->get_group()]++;
    }

  int page_nr = -1;
  vector<string>::const_iterator group_ci;
  for( group_ci = group_names.begin(); group_ci != group_names.end(); group_ci++ )
    {
      page_nr++;
      // Page Boxes

      GtkWidget* page_vbox = gtk_vbox_new( FALSE, 10 );
      gtk_container_border_width( GTK_CONTAINER( page_vbox ), 10 );
      gtk_widget_show( page_vbox );

      GtkWidget* page_hbox = gtk_hbox_new( FALSE, 10 );
      gtk_box_pack_start( GTK_BOX( page_vbox ), page_hbox,
                          FALSE, FALSE, 0 );
      gtk_widget_show( page_hbox );

      // Page tables

      GtkWidget* description_table = gtk_table_new( number_of_options[page_nr], 1, TRUE );
      GtkWidget* entry_table = gtk_table_new( number_of_options[page_nr], 1, TRUE );
      GtkWidget* button_table = gtk_table_new( number_of_options[page_nr], 3, TRUE );
      gtk_box_pack_start( GTK_BOX( page_hbox ), description_table, FALSE, TRUE, 0 );
      gtk_box_pack_start( GTK_BOX( page_hbox ), entry_table, TRUE, TRUE, 0 );
      gtk_box_pack_start( GTK_BOX( page_hbox ), button_table, FALSE, TRUE, 0 );

      gtk_table_set_row_spacings( GTK_TABLE( description_table ) , 10 );
      gtk_table_set_col_spacings( GTK_TABLE( description_table ) , 10 );
      gtk_table_set_row_spacings( GTK_TABLE( entry_table ) , 10 );
      gtk_table_set_col_spacings( GTK_TABLE( entry_table ) , 10 );
      gtk_table_set_row_spacings( GTK_TABLE( button_table ) , 10 );
      gtk_table_set_col_spacings( GTK_TABLE( button_table ) , 10 );
      gtk_widget_show( description_table );
      gtk_widget_show( entry_table );
      gtk_widget_show( button_table );

#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
      // TODO: a better way to recognize Window size page
      if( *group_ci == _("Window sizes") )
        {
          GtkWidget* grab_button = gtk_button_new_with_label
            ( _(" Grab sizes and positions from present windows ") );
          gtk_signal_connect( GTK_OBJECT( grab_button ), "clicked",
                              (GtkSignalFunc) OptionsWindow::grab_windows, this );
          gtk_box_pack_start( GTK_BOX( page_vbox ), grab_button, FALSE, FALSE, 0 );
          gtk_widget_show( grab_button );
        }
#endif

      GtkWidget* label = gtk_label_new( (*group_ci).c_str() );
      gtk_widget_show( label );
      gtk_notebook_append_page( GTK_NOTEBOOK( notebook ),
                                page_vbox, label );
      page_list.push_back( page_t( *group_ci, description_table,
                                   entry_table, button_table,
                                   number_of_options[page_nr] ) );
    }

  struct button_t { string label; bool used; GtkSignalFunc func; };

  list_of_options_and_entries.clear();
  for( mci = all_options.begin(); mci != all_options.end(); mci++ )
    {
      const int current_group = mci->second->get_group();
      const int current_row = page_list[current_group].current_row;
      // TODO: all entries should be stored to make sure we get all values.
      GtkWidget* entry = gtk_entry_new();

      bool use_minmax = true;
      if( mci->second->get_string_min().empty() ||
          mci->second->get_string_max().empty() )
        use_minmax = false;

      struct button_t buttons[] = {
        { (string)_(" Min "), use_minmax,
          (GtkSignalFunc) OptionsWindow::min_callback },
        { (string)_(" Def "), use_minmax,
          (GtkSignalFunc) OptionsWindow::def_callback },
        { (string)_(" Max "), use_minmax,
          (GtkSignalFunc) OptionsWindow::max_callback } };

      GtkWidget* label = gtk_label_new( mci->second->get_description().c_str() );
      gtk_table_attach_defaults
        ( GTK_TABLE( page_list[current_group].description_table ),
          label, 0, 1, current_row, current_row + 1 );
      gtk_widget_show( label );

      gtk_signal_connect( GTK_OBJECT( entry ), "changed",
                          (GtkSignalFunc) OptionsWindow::entry_handler,
                          (gpointer) mci->second );
      gtk_entry_set_text( GTK_ENTRY( entry ),
                          mci->second->get_string_val().c_str() );
      gtk_widget_set_usize( entry, 112, 22 );
      gtk_table_attach_defaults( GTK_TABLE( page_list[current_group].entry_table ),
                                 entry, 0, 1, current_row, current_row + 1 );
      gtk_widget_show( entry );

      GtkWidget* button_w = NULL;
      for( int i=0; i<3; i++ )
        if( buttons[i].used )
          {
            button_w = gtk_button_new_with_label( buttons[i].label.c_str() );
            gtk_signal_connect( GTK_OBJECT( button_w ), "clicked",
                                buttons[i].func, (gpointer) mci->second );
            gtk_table_attach_defaults
              ( GTK_TABLE( page_list[current_group].button_table ), button_w,
                i, i+1, current_row, current_row + 1 );
            gtk_widget_show( button_w );
          }
      list_of_options_and_entries[ mci->second ] = GTK_ENTRY( entry );
      page_list[current_group].current_row++;
    }

  // Lower buttons

  GtkWidget* hbox = gtk_hbox_new( FALSE, 10 );
  gtk_box_pack_start( GTK_BOX( vbox ), hbox, FALSE, FALSE, 0 );
  gtk_widget_show( hbox );

  struct button_t buttons[] = {
    { spaced_string( _("Default") ), true,
      (GtkSignalFunc) OptionsWindow::default_opts },
    { spaced_string( _("Load options") ), true, (GtkSignalFunc) OptionsWindow::load },
    { spaced_string( _("Save options") ), true, (GtkSignalFunc) OptionsWindow::save },
    { spaced_string( _("Save as default") ), true,
      (GtkSignalFunc) OptionsWindow::save_def },
    { spaced_string( _("Apply") ), true, (GtkSignalFunc) OptionsWindow::apply },
    { spaced_string( _("Ok") ), true, (GtkSignalFunc) OptionsWindow::ok },
    { spaced_string( _("Cancel") ), true, (GtkSignalFunc) OptionsWindow::cancel } };

  for( int i=0; i<7; i++ )
    {
      GtkWidget* button_w =
        gtk_button_new_with_label( buttons[i].label.c_str() );
      gtk_signal_connect( GTK_OBJECT( button_w ), "clicked",
                          buttons[i].func, (gpointer) this );
      gtk_box_pack_start( GTK_BOX( hbox ), button_w, TRUE,TRUE, 0 );
      gtk_widget_show( button_w );
    }

  gtk_widget_show( window_p );
  filesel = new FileSelector<OptionsWindow>( this );
}

OptionsWindow::~OptionsWindow()
{
  delete filesel;
  gtk_widget_destroy( window_p );
}

void
OptionsWindow::process_all_options( process_option_func func )
{
  map<const Option*,GtkEntry*>::iterator mi;
  for( mi = list_of_options_and_entries.begin();
       mi != list_of_options_and_entries.end(); mi++ )
    (this->*func)( mi->first, mi->second );
}

void
OptionsWindow::set_option( const Option* option_p, GtkEntry* entry )
{
  string new_val( gtk_entry_get_text( entry ) );
  string old_val( option_p->get_string_val() );
  if( old_val != new_val )
    ; // TODO: Set the option in some way. e.g. send a OptionChangeRequest
}

void
OptionsWindow::set_all_options()
{
  if( !(( the_gui.get_game_mode() == COMPETITION_MODE &&
          ( the_gui.get_state() == NO_STATE ||
            the_gui.get_state() == NOT_STARTED ||
            the_gui.get_state() == FINISHED ) ) ||
        ( the_gui.get_game_mode() != COMPETITION_MODE )) )
    return;

  process_all_options( &OptionsWindow::set_option );

  the_gui.set_colours();
}

void
OptionsWindow::set_entry_to_value( const Option* option_p, GtkEntry* entry )
{
  string prev_text( gtk_entry_get_text( entry ) );
  string new_text( option_p->get_string_val() );
  if( prev_text != new_text )
    gtk_entry_set_text( entry, new_text.c_str() );
}

void
OptionsWindow::update_all_gtk_entries()
{
  process_all_options( &OptionsWindow::set_entry_to_value );
}

void
OptionsWindow::set_entry_to_default( const Option* option_p, GtkEntry* entry )
{
  string prev_text( gtk_entry_get_text( entry ) );
  string new_text( option_p->get_string_def() );
  if( prev_text != new_text )
    gtk_entry_set_text( entry, new_text.c_str() );
}

void
OptionsWindow::default_opts( GtkWidget* widget,
                             class OptionsWindow* optionswindow_p )
{
  optionswindow_p->process_all_options( &OptionsWindow::set_entry_to_default );
}

void
OptionsWindow::load( GtkWidget* widget, class OptionsWindow* optionswindow_p )
{
  optionswindow_p->get_filesel()->bring_up( _("Choose an options file to load"),
                                            &OptionsWindow::load_options );
}

void
OptionsWindow::save( GtkWidget* widget, class OptionsWindow* optionswindow_p )
{
  optionswindow_p->get_filesel()->bring_up( _("Choose an options file to save"),
                                            &OptionsWindow::save_options );
}

void
OptionsWindow::load_options( const string& filename )
{
  if( filename.empty() || filename[filename.length() - 1] == '/' )  
    return;  // no file is selected

  // Read all opts
  update_all_gtk_entries();
}

void
OptionsWindow::save_options( const string& filename )
{
  if( filename.empty() || filename[filename.length() - 1] == '/' )  
    return;  // no file is selected

  // Save all opts
}

void
OptionsWindow::save_def( GtkWidget* widget, class OptionsWindow* optionswindow_p )
{
//    the_opts.save_all_options_to_file("",true);
}

void
OptionsWindow::apply( GtkWidget* widget, class OptionsWindow* optionswindow_p )
{
  optionswindow_p->set_all_options();
}

void
OptionsWindow::ok( GtkWidget* widget, class OptionsWindow* optionswindow_p )
{
  optionswindow_p->set_all_options();
  the_gui.close_optionswindow();
}

void
OptionsWindow::cancel( GtkWidget* widget, class OptionsWindow* optionswindow_p )
{
  the_gui.close_optionswindow();
}

void
OptionsWindow::delete_event_occured( GtkWidget* widget, GdkEvent* event,
                                     class OptionsWindow* optionswindow_p )
{
  the_gui.close_optionswindow();
}

void
OptionsWindow::grab_windows( GtkWidget* widget,
                             class OptionsWindow* optionswindow_p )
{
//  #if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
//    option_info_t<long>* long_opts = the_opts.get_all_long_options();
//    if( the_gui.is_arenawindow_up() )
//      {
//        ArenaWindow* aw_p = the_gui.get_arenawindow_p();
//        int width, height;
//        int xpos, ypos;
//        gdk_window_get_size( aw_p->get_window_p()->window,
//                             &width, &height );
//        gdk_window_get_root_origin( aw_p->get_window_p()->window,
//                                    &xpos, &ypos );

//        long_opts[OPTION_ARENA_WINDOW_SIZE_X].value = width;
//        long_opts[OPTION_ARENA_WINDOW_SIZE_Y].value = height;
//        long_opts[OPTION_ARENA_WINDOW_POS_X].value = xpos;
//        long_opts[OPTION_ARENA_WINDOW_POS_Y].value = ypos;
//        optionswindow_p->update_all_gtk_entries();
//      }
//    {
//      int xpos, ypos;
//      gdk_window_get_root_origin( the_controlwindow.get_window_p()->window,
//                                  &xpos, &ypos );

//      long_opts[OPTION_CONTROL_WINDOW_POS_X].value = xpos;
//      long_opts[OPTION_CONTROL_WINDOW_POS_Y].value = ypos;
//      optionswindow_p->update_all_gtk_entries();
//    }
//    if( the_gui.is_messagewindow_up() )
//      {
//        MessageWindow* mw_p = the_gui.get_messagewindow_p();
//        int width, height;
//        int xpos, ypos;
//        gdk_window_get_size( mw_p->get_window_p()->window,
//                             &width, &height );
//        gdk_window_get_root_origin( mw_p->get_window_p()->window,
//                                    &xpos, &ypos );

//        long_opts[OPTION_MESSAGE_WINDOW_SIZE_X].value = width;
//        long_opts[OPTION_MESSAGE_WINDOW_SIZE_Y].value = height;
//        long_opts[OPTION_MESSAGE_WINDOW_POS_X].value = xpos;
//        long_opts[OPTION_MESSAGE_WINDOW_POS_Y].value = ypos;
//        optionswindow_p->update_all_gtk_entries();
//      }
//    if( the_gui.is_scorewindow_up() )
//      {
//        ScoreWindow* sw_p = the_gui.get_scorewindow_p();
//        int width, height;
//        int xpos, ypos;
//        gdk_window_get_size( sw_p->get_window_p()->window,
//                             &width, &height );
//        gdk_window_get_root_origin( sw_p->get_window_p()->window,
//                                    &xpos, &ypos );

//        long_opts[OPTION_SCORE_WINDOW_SIZE_X].value = width;
//        long_opts[OPTION_SCORE_WINDOW_SIZE_Y].value = height;
//        long_opts[OPTION_SCORE_WINDOW_POS_X].value = xpos;
//        long_opts[OPTION_SCORE_WINDOW_POS_Y].value = ypos;
//        optionswindow_p->update_all_gtk_entries();
//      }
//    if( the_gui.is_statisticswindow_up() )
//      {
//        StatisticsWindow* sw_p = the_gui.get_statisticswindow_p();
//        int width, height;
//        gdk_window_get_size( sw_p->get_window_p()->window,
//                             &width, &height );

//        long_opts[OPTION_STATISTICS_WINDOW_SIZE_X].value = width;
//        long_opts[OPTION_STATISTICS_WINDOW_SIZE_Y].value = height;
//        optionswindow_p->update_all_gtk_entries();
//      }
//  #endif
}

void
OptionsWindow::min_callback( GtkWidget* widget, Option* option_p )
{
  gtk_entry_set_text( GTK_ENTRY( widget ), option_p->get_string_min().c_str() );
}

void
OptionsWindow::def_callback( GtkWidget* widget, Option* option_p )
{
  gtk_entry_set_text( GTK_ENTRY( widget ), option_p->get_string_def().c_str() );
}

void
OptionsWindow::max_callback( GtkWidget* widget, Option* option_p )
{
  gtk_entry_set_text( GTK_ENTRY( widget ), option_p->get_string_max().c_str() );
}

void
OptionsWindow::entry_handler( GtkWidget* widget, Option* option_p )
{
  // TODO: Cursor positions are very strange when entering not allowed characters!
  string entry_text = gtk_entry_get_text(GTK_ENTRY(widget));
  string old_entry_text = entry_text;
  option_p->make_correct_string_val( entry_text );
  if( old_entry_text != entry_text )
    gtk_entry_set_text( GTK_ENTRY( widget ), entry_text.c_str() );
}
