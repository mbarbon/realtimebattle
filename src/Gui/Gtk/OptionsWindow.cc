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
#include "GuiVarious.h"
#include "Gui.h"
#include "String.h"


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

      struct button_t { string label; bool used; GtkSignalFunc func; };

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
      page_list[current_group].current_row++;
    }

//        for( int opt=0; opt<LAST_LONG_OPTION; opt++ )
//          if( long_opts[opt].page == i )
//            {
//              row++;

//              bool sign = false;
//              if( long_opts[opt].min_value < 0.0 )
//                sign = true;

//              string entry_text;
//              if( long_opts[opt].datatype == ENTRY_INT )
//                entry_text = lint2string( long_opts[opt].value );
//              else if( long_opts[opt].datatype == ENTRY_HEX )
//                entry_text = hex2string( long_opts[opt].value );

//              entry_t* info = new entry_t( long_opts[opt].datatype, sign );

//  //              long_opts[opt].entry =
//  //                gtk_entry_new_with_max_length
//  //                ( long_opts[opt].max_letters_in_entry );

//              struct button_t buttons[] = {
//                { (string)_(" Min "), true, (GtkSignalFunc) OptionsWindow::long_min,
//                  (gpointer) &long_opts[opt] },
//                { (string)_(" Def "), true, (GtkSignalFunc) OptionsWindow::long_def,
//                  (gpointer) &long_opts[opt] },
//                { (string)_(" Max "), true, (GtkSignalFunc) OptionsWindow::long_max,
//                  (gpointer) &long_opts[opt] } };

//  //              add_option_to_notebook( description_table,
//  //                                      entry_table, button_table,
//  //                                      row, long_opts[opt].translated_label,
//  //                                      long_opts[opt].entry,
//  //                                      entry_text, info, buttons );

//            }
//        for( int opt=0; opt<LAST_STRING_OPTION; opt++ )
//          if( string_opts[opt].page == i )
//            {
//              row++;

//              entry_t* info = new entry_t( ENTRY_CHAR, false );

//  //              string_opts[opt].entry =
//  //                gtk_entry_new_with_max_length
//  //                ( string_opts[opt].max_letters_in_entry );

//              struct button_t buttons[] = {
//                { (string)"" , false, (GtkSignalFunc) NULL, (gpointer) NULL },
//                { (string)_(" Def "), true, (GtkSignalFunc) OptionsWindow::string_def,
//                  (gpointer) &string_opts[opt] },
//                { (string)"" , false, (GtkSignalFunc) NULL, (gpointer) NULL } };

//  //              add_option_to_notebook( description_table,
//  //                                      entry_table, button_table,
//  //                                      row, string_opts[opt].translated_label,
//  //                                      string_opts[opt].entry,
//  //                                      string_opts[opt].value,
//  //                                      info, buttons );

//            }

//      }

//    // Lower buttons

//    GtkWidget* hbox = gtk_hbox_new( FALSE, 10 );
//    gtk_box_pack_start( GTK_BOX( vbox ), hbox, FALSE, FALSE, 0 );
//    gtk_widget_show( hbox );

//    struct button_t buttons[] = {
//      { (string)_(" Default "), true, (GtkSignalFunc) OptionsWindow::default_opts,
//        (gpointer) this },
//      { (string)_(" Load options "), true, (GtkSignalFunc) OptionsWindow::load,
//        (gpointer) this },
//      { (string)_(" Save options "), true, (GtkSignalFunc) OptionsWindow::save,
//        (gpointer) this },
//      { (string)_(" Save as default "), true, (GtkSignalFunc) OptionsWindow::save_def,
//        (gpointer) this },
//      { (string)_(" Apply "), true, (GtkSignalFunc) OptionsWindow::apply,
//        (gpointer) this },
//      { (string)_(" Ok "), true, (GtkSignalFunc) OptionsWindow::ok,
//        (gpointer) this },
//      { (string)_(" Cancel "), true, (GtkSignalFunc) OptionsWindow::cancel,
//        (gpointer) this } };

//    for( int i=0; i<7; i++ )
//      {
//        GtkWidget* button_w =
//          gtk_button_new_with_label( buttons[i].label.c_str() );
//        gtk_signal_connect( GTK_OBJECT( button_w ), "clicked",
//                            buttons[i].func, buttons[i].data );
//        gtk_box_pack_start( GTK_BOX( hbox ), button_w, TRUE,TRUE, 0 );
//        gtk_widget_show( button_w );
//      }

//    gtk_widget_show( window_p );
//    filesel = NULL;
}

OptionsWindow::~OptionsWindow()
{
//    gtk_widget_destroy( window_p );
}

void
OptionsWindow::set_all_options()
{
//    bool allowed = false;

//    if( the_arena_controller.is_started() )
//      {
//        if( ( the_arena.get_game_mode() == COMPETITION_MODE &&
//              ( the_arena.get_state() == NO_STATE ||
//                the_arena.get_state() == NOT_STARTED ||
//                the_arena.get_state() == FINISHED ) ) ||
//            ( the_arena.get_game_mode() != COMPETITION_MODE ) )
//          allowed = true;
//      }
//    if( !the_arena_controller.is_started() || allowed )
//      {
//        option_info_t<double>* double_opts = the_opts.get_all_double_options();
//        option_info_t<long>* long_opts = the_opts.get_all_long_options();
//        option_info_t<string>* string_opts = the_opts.get_all_string_options();
//        for(int i=0;i<LAST_DOUBLE_OPTION;i++)
//          {
//            double entry_value = 0;
//            entry_value = str2dbl
//              ( gtk_entry_get_text( GTK_ENTRY( double_opts[i].entry ) ) );
//            if( entry_value > double_opts[i].max_value )
//              entry_value = double_opts[i].max_value;
//            if( entry_value < double_opts[i].min_value )
//              entry_value = double_opts[i].min_value;

//            double_opts[i].value = entry_value;
//          }

//        for(int i=0;i<LAST_LONG_OPTION;i++)
//          {
//            long entry_value = 0;
//            if( long_opts[i].datatype == ENTRY_INT )
//              entry_value = str2long
//                ( gtk_entry_get_text( GTK_ENTRY( long_opts[i].entry ) ) );
//            if( long_opts[i].datatype == ENTRY_HEX )
//              entry_value = str2hex
//                ( gtk_entry_get_text( GTK_ENTRY( long_opts[i].entry ) ) );

//            if( entry_value > long_opts[i].max_value )
//              entry_value = long_opts[i].max_value;
//            if( entry_value < long_opts[i].min_value )
//              entry_value = long_opts[i].min_value;

//            long_opts[i].value = entry_value;
//          }

//        for(int i=0;i<LAST_STRING_OPTION;i++)
//          string_opts[i].value =
//            gtk_entry_get_text( GTK_ENTRY( string_opts[i].entry ) );

//        the_gui.set_colours();
//      }
}

void
OptionsWindow::update_all_gtk_entries()
{
//    option_info_t<double>* double_opts = the_opts.get_all_double_options();
//    option_info_t<long>* long_opts = the_opts.get_all_long_options();
//    option_info_t<string>* string_opts = the_opts.get_all_string_options();

//    for(int i=0;i<LAST_DOUBLE_OPTION;i++)
//      gtk_entry_set_text( GTK_ENTRY( double_opts[i].entry ),
//                          string(double_opts[i].value).chars() );
//    for(int i=0;i<LAST_LONG_OPTION;i++)
//      {
//        if( long_opts[i].datatype == ENTRY_INT )
//          gtk_entry_set_text( GTK_ENTRY( long_opts[i].entry ),
//                              string( long_opts[i].value).chars() );
//        else if (long_opts[i].datatype == ENTRY_HEX)
//          gtk_entry_set_text( GTK_ENTRY( long_opts[i].entry ),
//                              hex2str(long_opts[i].value).chars() );
//      }
//    for(int i=0;i<LAST_STRING_OPTION;i++)
//      gtk_entry_set_text( GTK_ENTRY( string_opts[i].entry ),
//                          string_opts[i].value.chars() );
}

void
OptionsWindow::default_opts( GtkWidget* widget,
                             class OptionsWindow* optionswindow_p )
{
//    option_info_t<double>* double_opts = the_opts.get_all_double_options();
//    option_info_t<long>* long_opts = the_opts.get_all_long_options();
//    option_info_t<string>* string_opts = the_opts.get_all_string_options();

//    for(int i=0;i<LAST_DOUBLE_OPTION;i++)
//      double_opts[i].value = double_opts[i].default_value;
//    for(int i=0;i<LAST_LONG_OPTION;i++)
//      long_opts[i].value = long_opts[i].default_value;
//    for(int i=0;i<LAST_STRING_OPTION;i++)
//      string_opts[i].value = string_opts[i].default_value;
//    optionswindow_p->update_all_gtk_entries();
}

void
OptionsWindow::load( GtkWidget* widget,
                     class OptionsWindow* optionswindow_p )
{
//    if( optionswindow_p->get_filesel() == NULL )
//      {
//        GtkWidget* fs =
//          gtk_file_selection_new( _("Choose an options file to load") );
//        gtk_signal_connect( GTK_OBJECT( fs ), "destroy",
//                            (GtkSignalFunc) OptionsWindow::destroy_filesel,
//                            (gpointer) optionswindow_p );
//        gtk_signal_connect
//          ( GTK_OBJECT( GTK_FILE_SELECTION( fs )->cancel_button ), "clicked",
//            (GtkSignalFunc) OptionsWindow::destroy_filesel,
//            (gpointer) optionswindow_p );
//        gtk_signal_connect
//          ( GTK_OBJECT( GTK_FILE_SELECTION( fs )->ok_button ), "clicked",
//            (GtkSignalFunc) OptionsWindow::load_options,
//            (gpointer) optionswindow_p );
//        gtk_widget_show( fs );
//        optionswindow_p->set_filesel( fs );
//      }
}

void
OptionsWindow::save( GtkWidget* widget,
                     class OptionsWindow* optionswindow_p )
{
//    if( optionswindow_p->get_filesel() == NULL )
//      {
//        GtkWidget* fs =
//          gtk_file_selection_new( _("Choose an options file to save") );
//        gtk_signal_connect( GTK_OBJECT( fs ), "destroy",
//                            (GtkSignalFunc) OptionsWindow::destroy_filesel,
//                            (gpointer) optionswindow_p );
//        gtk_signal_connect
//          ( GTK_OBJECT( GTK_FILE_SELECTION( fs )->cancel_button ), "clicked",
//            (GtkSignalFunc) OptionsWindow::destroy_filesel,
//            (gpointer) optionswindow_p );
//        gtk_signal_connect
//          ( GTK_OBJECT( GTK_FILE_SELECTION( fs )->ok_button ), "clicked",
//            (GtkSignalFunc) OptionsWindow::save_options,
//            (gpointer) optionswindow_p );
//        gtk_widget_show( fs );
//        optionswindow_p->set_filesel( fs );
//      }
}

void
OptionsWindow::load_options( GtkWidget* widget,
                             class OptionsWindow* optionswindow_p )
{
//    the_opts.read_options_file
//      ( gtk_file_selection_get_filename
//        ( GTK_FILE_SELECTION( optionswindow_p->get_filesel() )), false );
//    optionswindow_p->update_all_gtk_entries();
//    destroy_filesel( optionswindow_p->get_filesel(), optionswindow_p );
}

void
OptionsWindow::save_options( GtkWidget* widget,
                             class OptionsWindow* optionswindow_p )
{
//    the_opts.save_all_options_to_file
//      ( gtk_file_selection_get_filename
//        ( GTK_FILE_SELECTION( optionswindow_p->get_filesel() )), false );
//    destroy_filesel( optionswindow_p->get_filesel(), optionswindow_p );
}

void
OptionsWindow::destroy_filesel( GtkWidget* widget,
                               class OptionsWindow* optionswindow_p )
{
//    gtk_widget_destroy( optionswindow_p->get_filesel() );
//    optionswindow_p->set_filesel( NULL );
}

void
OptionsWindow::save_def( GtkWidget* widget,
                         class OptionsWindow* optionswindow_p )
{
//    the_opts.save_all_options_to_file("",true);
}

void
OptionsWindow::apply( GtkWidget* widget,
                      class OptionsWindow* optionswindow_p )
{
//    optionswindow_p->set_all_options();
}

void
OptionsWindow::ok( GtkWidget* widget,
                   class OptionsWindow* optionswindow_p )
{
//    optionswindow_p->set_all_options();
  //  the_opts.close_optionswindow();
}

void
OptionsWindow::cancel( GtkWidget* widget,
                       class OptionsWindow* optionswindow_p )
{
  //  the_opts.close_optionswindow();
}

void
OptionsWindow::delete_event_occured( GtkWidget* widget, GdkEvent* event,
                                     class OptionsWindow* optionswindow_p )
{
  //  the_opts.close_optionswindow();
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
