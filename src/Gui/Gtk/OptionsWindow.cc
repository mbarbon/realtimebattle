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

#include <gtk/gtk.h>
#include <fstream.h>

#include "OptionsWindow.h"
#include "IntlDefs.h"
#include "ArenaRealTime.h"
#include "ArenaController.h"
#include "ArenaWindow.h"
#include "ControlWindow.h"
#include "MessageWindow.h"
#include "ScoreWindow.h"
#include "StatisticsWindow.h"
#include "Options.h"
#include "String.h"
#include "GuiVarious.h"
#include "Gui.h"

extern class Options the_opts;
extern class Gui* gui_p;
extern class ControlWindow* controlwindow_p;

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

  const char* page_titles[LAST_PAGE] = { _("Environment"), _("Robot"), _("Shot"),
                                         _("Extras"), _("Time"), _("Window sizes"),
                                         _("Misc") };

  option_info_t<double>* double_opts = the_opts.get_all_double_options();
  option_info_t<long>* long_opts = the_opts.get_all_long_options();
  option_info_t<String>* string_opts = the_opts.get_all_string_options();

  for( int i=0; i < LAST_PAGE; i++ )
    {
      int number_of_options = 0;
      for( int opt=0; opt<LAST_DOUBLE_OPTION; opt++ )
        if( double_opts[opt].page == i )
          number_of_options++;
      for( int opt=0; opt<LAST_LONG_OPTION; opt++ )
        if( long_opts[opt].page == i )
          number_of_options++;
      for( int opt=0; opt<LAST_STRING_OPTION; opt++ )
        if( string_opts[opt].page == i )
          number_of_options++;

      // Page Boxes

      GtkWidget* page_vbox = gtk_vbox_new( FALSE, 10 );
      gtk_container_border_width( GTK_CONTAINER( page_vbox ), 10 );
      gtk_widget_show( page_vbox );

      GtkWidget* page_hbox = gtk_hbox_new( FALSE, 10 );
      gtk_box_pack_start( GTK_BOX( page_vbox ), page_hbox,
                          FALSE, FALSE, 0 );
      gtk_widget_show( page_hbox );

      // Page tables

      GtkWidget* description_table = gtk_table_new( number_of_options, 1, TRUE );
      GtkWidget* entry_table = gtk_table_new( number_of_options, 1, TRUE );
      GtkWidget* button_table = gtk_table_new( number_of_options, 3, TRUE );
      gtk_box_pack_start( GTK_BOX( page_hbox ), description_table, FALSE, TRUE, 0 );
      gtk_box_pack_start( GTK_BOX( page_hbox ), entry_table, TRUE, TRUE, 0 );
      gtk_box_pack_start( GTK_BOX( page_hbox ), button_table, FALSE, TRUE, 0 );

      int row = -1;

      for( int opt=0; opt<LAST_DOUBLE_OPTION; opt++ )
        if( double_opts[opt].page == i )
          {
            row++;

            bool sign = false;
            if( double_opts[opt].min_value < 0.0 )
              sign = true;

            entry_t* info = new entry_t( ENTRY_DOUBLE, sign );

//              double_opts[opt].entry =
//                gtk_entry_new_with_max_length
//                ( double_opts[opt].max_letters_in_entry );

            struct button_t buttons[] = {
              { (String)_(" Min "), true, (GtkSignalFunc) OptionsWindow::double_min,
                (gpointer) &double_opts[opt] },
              { (String)_(" Def "), true, (GtkSignalFunc) OptionsWindow::double_def,
                (gpointer) &double_opts[opt] },
              { (String)_(" Max "), true, (GtkSignalFunc) OptionsWindow::double_max,
                (gpointer) &double_opts[opt] } };

//              add_option_to_notebook( description_table,
//                                      entry_table, button_table,
//                                      row, double_opts[opt].translated_label,
//                                      double_opts[opt].entry,
//                                      String( double_opts[opt].value ),
//                                      info, buttons );

          }
      for( int opt=0; opt<LAST_LONG_OPTION; opt++ )
        if( long_opts[opt].page == i )
          {
            row++;

            bool sign = false;
            if( long_opts[opt].min_value < 0.0 )
              sign = true;

            String entry_text;
            if( long_opts[opt].datatype == ENTRY_INT )
              entry_text = String( long_opts[opt].value );
            else if( long_opts[opt].datatype == ENTRY_HEX )
              entry_text = hex2str( long_opts[opt].value );

            entry_t* info = new entry_t( long_opts[opt].datatype, sign );

//              long_opts[opt].entry =
//                gtk_entry_new_with_max_length
//                ( long_opts[opt].max_letters_in_entry );

            struct button_t buttons[] = {
              { (String)_(" Min "), true, (GtkSignalFunc) OptionsWindow::long_min,
                (gpointer) &long_opts[opt] },
              { (String)_(" Def "), true, (GtkSignalFunc) OptionsWindow::long_def,
                (gpointer) &long_opts[opt] },
              { (String)_(" Max "), true, (GtkSignalFunc) OptionsWindow::long_max,
                (gpointer) &long_opts[opt] } };

//              add_option_to_notebook( description_table,
//                                      entry_table, button_table,
//                                      row, long_opts[opt].translated_label,
//                                      long_opts[opt].entry,
//                                      entry_text, info, buttons );

          }
      for( int opt=0; opt<LAST_STRING_OPTION; opt++ )
        if( string_opts[opt].page == i )
          {
            row++;

            entry_t* info = new entry_t( ENTRY_CHAR, false );

//              string_opts[opt].entry =
//                gtk_entry_new_with_max_length
//                ( string_opts[opt].max_letters_in_entry );

            struct button_t buttons[] = {
              { (String)"" , false, (GtkSignalFunc) NULL, (gpointer) NULL },
              { (String)_(" Def "), true, (GtkSignalFunc) OptionsWindow::string_def,
                (gpointer) &string_opts[opt] },
              { (String)"" , false, (GtkSignalFunc) NULL, (gpointer) NULL } };

//              add_option_to_notebook( description_table,
//                                      entry_table, button_table,
//                                      row, string_opts[opt].translated_label,
//                                      string_opts[opt].entry,
//                                      string_opts[opt].value,
//                                      info, buttons );

          }
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
      if( i == PAGE_SIZE_OF_WINDOWS )
        {
          GtkWidget* grab_button = gtk_button_new_with_label
            ( _(" Grab sizes and positions from present windows ") );
          gtk_signal_connect( GTK_OBJECT( grab_button ), "clicked",
                              (GtkSignalFunc) OptionsWindow::grab_windows, this );
          gtk_box_pack_start( GTK_BOX( page_vbox ), grab_button, FALSE, FALSE, 0 );
          gtk_widget_show( grab_button );
        }
#endif

      GtkWidget* label = gtk_label_new( page_titles[i] );
      gtk_widget_show( label );
      gtk_notebook_append_page( GTK_NOTEBOOK( notebook ),
                                page_vbox, label );
    }

  // Lower buttons

  GtkWidget* hbox = gtk_hbox_new( FALSE, 10 );
  gtk_box_pack_start( GTK_BOX( vbox ), hbox, FALSE, FALSE, 0 );
  gtk_widget_show( hbox );

  struct button_t buttons[] = {
    { (String)_(" Default "), true, (GtkSignalFunc) OptionsWindow::default_opts,
      (gpointer) this },
    { (String)_(" Load options "), true, (GtkSignalFunc) OptionsWindow::load,
      (gpointer) this },
    { (String)_(" Save options "), true, (GtkSignalFunc) OptionsWindow::save,
      (gpointer) this },
    { (String)_(" Save as default "), true, (GtkSignalFunc) OptionsWindow::save_def,
      (gpointer) this },
    { (String)_(" Apply "), true, (GtkSignalFunc) OptionsWindow::apply,
      (gpointer) this },
    { (String)_(" Ok "), true, (GtkSignalFunc) OptionsWindow::ok,
      (gpointer) this },
    { (String)_(" Cancel "), true, (GtkSignalFunc) OptionsWindow::cancel,
      (gpointer) this } };

  for( int i=0; i<7; i++ )
    {
      GtkWidget* button_w =
        gtk_button_new_with_label( buttons[i].label.chars() );
      gtk_signal_connect( GTK_OBJECT( button_w ), "clicked",
                          buttons[i].func, buttons[i].data );
      gtk_box_pack_start( GTK_BOX( hbox ), button_w, TRUE,TRUE, 0 );
      gtk_widget_show( button_w );
    }

  gtk_widget_show( window_p );
  filesel = NULL;
}

OptionsWindow::~OptionsWindow()
{
  gtk_widget_destroy( window_p );
}

void
OptionsWindow::add_option_to_notebook( GtkWidget* description_table,
                                       GtkWidget* entry_table,
                                       GtkWidget* button_table,
                                       int row, String description,
                                       GtkWidget* entry,
                                       String entry_text,
                                       entry_t* info,
                                       button_t* buttons )
{
  GtkWidget* label =
    gtk_label_new( description.chars() );
  gtk_table_attach_defaults( GTK_TABLE( description_table ),
                             label, 0, 1, row, row + 1 );
  gtk_widget_show( label );
  
  gtk_signal_connect( GTK_OBJECT( entry ), "changed",
                      (GtkSignalFunc) entry_handler, info);
  gtk_entry_set_text( GTK_ENTRY( entry ), entry_text.chars() );
  gtk_widget_set_usize( entry, 112, 22 );
  gtk_table_attach_defaults( GTK_TABLE( entry_table ),
                             entry, 0, 1, row, row + 1 );
  gtk_widget_show( entry );

  GtkWidget* button_w;
  for( int i=0; i<3; i++ )
    if( buttons[i].used )
      {
        button_w = gtk_button_new_with_label( buttons[i].label.chars() );
        gtk_signal_connect( GTK_OBJECT( button_w ), "clicked",
                            buttons[i].func, buttons[i].data );
        gtk_table_attach_defaults( GTK_TABLE( button_table ), button_w,
                                   i, i+1, row, row+1 );
        gtk_widget_show( button_w );
      }
}

void
OptionsWindow::set_all_options()
{
  bool allowed = false;

  if( the_arena_controller.is_started() )
    {
      if( ( the_arena.get_game_mode() == COMPETITION_MODE &&
            ( the_arena.get_state() == NO_STATE ||
              the_arena.get_state() == NOT_STARTED ||
              the_arena.get_state() == FINISHED ) ) ||
          ( the_arena.get_game_mode() != COMPETITION_MODE ) )
        allowed = true;
    }
  if( !the_arena_controller.is_started() || allowed )
    {
      option_info_t<double>* double_opts = the_opts.get_all_double_options();
      option_info_t<long>* long_opts = the_opts.get_all_long_options();
      option_info_t<String>* string_opts = the_opts.get_all_string_options();
      for(int i=0;i<LAST_DOUBLE_OPTION;i++)
        {
          double entry_value = 0;
//            entry_value = str2dbl
//              ( gtk_entry_get_text( GTK_ENTRY( double_opts[i].entry ) ) );
          if( entry_value > double_opts[i].max_value )
            entry_value = double_opts[i].max_value;
          if( entry_value < double_opts[i].min_value )
            entry_value = double_opts[i].min_value;

          double_opts[i].value = entry_value;
        }

      for(int i=0;i<LAST_LONG_OPTION;i++)
        {
          long entry_value = 0;
//            if( long_opts[i].datatype == ENTRY_INT )
//              entry_value = str2long
//                ( gtk_entry_get_text( GTK_ENTRY( long_opts[i].entry ) ) );
//            if( long_opts[i].datatype == ENTRY_HEX )
//              entry_value = str2hex
//                ( gtk_entry_get_text( GTK_ENTRY( long_opts[i].entry ) ) );

          if( entry_value > long_opts[i].max_value )
            entry_value = long_opts[i].max_value;
          if( entry_value < long_opts[i].min_value )
            entry_value = long_opts[i].min_value;

          long_opts[i].value = entry_value;
        }

//        for(int i=0;i<LAST_STRING_OPTION;i++)
//          string_opts[i].value =
//            gtk_entry_get_text( GTK_ENTRY( string_opts[i].entry ) );

#ifndef NO_GRAPHICS
      if( !no_graphics )
        the_gui.set_colours();
#endif
    }
}

void
OptionsWindow::update_all_gtk_entries()
{
  option_info_t<double>* double_opts = the_opts.get_all_double_options();
  option_info_t<long>* long_opts = the_opts.get_all_long_options();
  option_info_t<String>* string_opts = the_opts.get_all_string_options();

//    for(int i=0;i<LAST_DOUBLE_OPTION;i++)
//      gtk_entry_set_text( GTK_ENTRY( double_opts[i].entry ),
//                          String(double_opts[i].value).chars() );
//    for(int i=0;i<LAST_LONG_OPTION;i++)
//      {
//        if( long_opts[i].datatype == ENTRY_INT )
//          gtk_entry_set_text( GTK_ENTRY( long_opts[i].entry ),
//                              String( long_opts[i].value).chars() );
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
  option_info_t<double>* double_opts = the_opts.get_all_double_options();
  option_info_t<long>* long_opts = the_opts.get_all_long_options();
  option_info_t<String>* string_opts = the_opts.get_all_string_options();

  for(int i=0;i<LAST_DOUBLE_OPTION;i++)
    double_opts[i].value = double_opts[i].default_value;
  for(int i=0;i<LAST_LONG_OPTION;i++)
    long_opts[i].value = long_opts[i].default_value;
  for(int i=0;i<LAST_STRING_OPTION;i++)
    string_opts[i].value = string_opts[i].default_value;
  optionswindow_p->update_all_gtk_entries();
}

void
OptionsWindow::load( GtkWidget* widget,
                     class OptionsWindow* optionswindow_p )
{
  if( optionswindow_p->get_filesel() == NULL )
    {
      GtkWidget* fs =
        gtk_file_selection_new( _("Choose an options file to load") );
      gtk_signal_connect( GTK_OBJECT( fs ), "destroy",
                          (GtkSignalFunc) OptionsWindow::destroy_filesel,
                          (gpointer) optionswindow_p );
      gtk_signal_connect
        ( GTK_OBJECT( GTK_FILE_SELECTION( fs )->cancel_button ), "clicked",
          (GtkSignalFunc) OptionsWindow::destroy_filesel,
          (gpointer) optionswindow_p );
      gtk_signal_connect
        ( GTK_OBJECT( GTK_FILE_SELECTION( fs )->ok_button ), "clicked",
          (GtkSignalFunc) OptionsWindow::load_options,
          (gpointer) optionswindow_p );
      gtk_widget_show( fs );
      optionswindow_p->set_filesel( fs );
    }
}

void
OptionsWindow::save( GtkWidget* widget,
                     class OptionsWindow* optionswindow_p )
{
  if( optionswindow_p->get_filesel() == NULL )
    {
      GtkWidget* fs =
        gtk_file_selection_new( _("Choose an options file to save") );
      gtk_signal_connect( GTK_OBJECT( fs ), "destroy",
                          (GtkSignalFunc) OptionsWindow::destroy_filesel,
                          (gpointer) optionswindow_p );
      gtk_signal_connect
        ( GTK_OBJECT( GTK_FILE_SELECTION( fs )->cancel_button ), "clicked",
          (GtkSignalFunc) OptionsWindow::destroy_filesel,
          (gpointer) optionswindow_p );
      gtk_signal_connect
        ( GTK_OBJECT( GTK_FILE_SELECTION( fs )->ok_button ), "clicked",
          (GtkSignalFunc) OptionsWindow::save_options,
          (gpointer) optionswindow_p );
      gtk_widget_show( fs );
      optionswindow_p->set_filesel( fs );
    }
}

void
OptionsWindow::load_options( GtkWidget* widget,
                             class OptionsWindow* optionswindow_p )
{
  the_opts.read_options_file
    ( gtk_file_selection_get_filename
      ( GTK_FILE_SELECTION( optionswindow_p->get_filesel() )), false );
  optionswindow_p->update_all_gtk_entries();
  destroy_filesel( optionswindow_p->get_filesel(), optionswindow_p );
}

void
OptionsWindow::save_options( GtkWidget* widget,
                             class OptionsWindow* optionswindow_p )
{
  the_opts.save_all_options_to_file
    ( gtk_file_selection_get_filename
      ( GTK_FILE_SELECTION( optionswindow_p->get_filesel() )), false );
  destroy_filesel( optionswindow_p->get_filesel(), optionswindow_p );
}

void
OptionsWindow::destroy_filesel( GtkWidget* widget,
                               class OptionsWindow* optionswindow_p )
{
  gtk_widget_destroy( optionswindow_p->get_filesel() );
  optionswindow_p->set_filesel( NULL );
}

void
OptionsWindow::save_def( GtkWidget* widget,
                         class OptionsWindow* optionswindow_p )
{
  the_opts.save_all_options_to_file("",true);
}

void
OptionsWindow::apply( GtkWidget* widget,
                      class OptionsWindow* optionswindow_p )
{
  optionswindow_p->set_all_options();
}

void
OptionsWindow::ok( GtkWidget* widget,
                   class OptionsWindow* optionswindow_p )
{
  optionswindow_p->set_all_options();
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
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  option_info_t<long>* long_opts = the_opts.get_all_long_options();
  if( the_gui.is_arenawindow_up() )
    {
      ArenaWindow* aw_p = the_gui.get_arenawindow_p();
      int width, height;
      int xpos, ypos;
      gdk_window_get_size( aw_p->get_window_p()->window,
                           &width, &height );
      gdk_window_get_root_origin( aw_p->get_window_p()->window,
                                  &xpos, &ypos );

      long_opts[OPTION_ARENA_WINDOW_SIZE_X].value = width;
      long_opts[OPTION_ARENA_WINDOW_SIZE_Y].value = height;
      long_opts[OPTION_ARENA_WINDOW_POS_X].value = xpos;
      long_opts[OPTION_ARENA_WINDOW_POS_Y].value = ypos;
      optionswindow_p->update_all_gtk_entries();
    }
  {
    int xpos, ypos;
    gdk_window_get_root_origin( controlwindow_p->get_window_p()->window,
                                &xpos, &ypos );

    long_opts[OPTION_CONTROL_WINDOW_POS_X].value = xpos;
    long_opts[OPTION_CONTROL_WINDOW_POS_Y].value = ypos;
    optionswindow_p->update_all_gtk_entries();
  }
  if( the_gui.is_messagewindow_up() )
    {
      MessageWindow* mw_p = the_gui.get_messagewindow_p();
      int width, height;
      int xpos, ypos;
      gdk_window_get_size( mw_p->get_window_p()->window,
                           &width, &height );
      gdk_window_get_root_origin( mw_p->get_window_p()->window,
                                  &xpos, &ypos );

      long_opts[OPTION_MESSAGE_WINDOW_SIZE_X].value = width;
      long_opts[OPTION_MESSAGE_WINDOW_SIZE_Y].value = height;
      long_opts[OPTION_MESSAGE_WINDOW_POS_X].value = xpos;
      long_opts[OPTION_MESSAGE_WINDOW_POS_Y].value = ypos;
      optionswindow_p->update_all_gtk_entries();
    }
  if( the_gui.is_scorewindow_up() )
    {
      ScoreWindow* sw_p = the_gui.get_scorewindow_p();
      int width, height;
      int xpos, ypos;
      gdk_window_get_size( sw_p->get_window_p()->window,
                           &width, &height );
      gdk_window_get_root_origin( sw_p->get_window_p()->window,
                                  &xpos, &ypos );

      long_opts[OPTION_SCORE_WINDOW_SIZE_X].value = width;
      long_opts[OPTION_SCORE_WINDOW_SIZE_Y].value = height;
      long_opts[OPTION_SCORE_WINDOW_POS_X].value = xpos;
      long_opts[OPTION_SCORE_WINDOW_POS_Y].value = ypos;
      optionswindow_p->update_all_gtk_entries();
    }
  if( the_gui.is_statisticswindow_up() )
    {
      StatisticsWindow* sw_p = the_gui.get_statisticswindow_p();
      int width, height;
      gdk_window_get_size( sw_p->get_window_p()->window,
                           &width, &height );

      long_opts[OPTION_STATISTICS_WINDOW_SIZE_X].value = width;
      long_opts[OPTION_STATISTICS_WINDOW_SIZE_Y].value = height;
      optionswindow_p->update_all_gtk_entries();
    }
#endif
}

void
OptionsWindow::double_min( GtkWidget* widget,
                           option_info_t<double>* option )
{
//    gtk_entry_set_text( GTK_ENTRY( option->entry ),
//                        String( option->min_value ).chars() );
}

void
OptionsWindow::double_def( GtkWidget* widget,
                           option_info_t<double>* option )
{
//    gtk_entry_set_text( GTK_ENTRY( option->entry ),
//                        String( option->default_value ).chars() );
}

void
OptionsWindow::double_max( GtkWidget* widget,
                           option_info_t<double>* option )
{
//    gtk_entry_set_text( GTK_ENTRY( option->entry ),
//                        String( option->max_value ).chars() );
}

void
OptionsWindow::long_min( GtkWidget* widget,
                         option_info_t<long>* option )
{
//    if( option->datatype == ENTRY_INT )
//      gtk_entry_set_text( GTK_ENTRY( option->entry ),
//                          String( option->min_value).chars() );
//    else if( option->datatype == ENTRY_HEX )
//      gtk_entry_set_text( GTK_ENTRY( option->entry ),
//                          hex2str( option->min_value).chars() );
}

void
OptionsWindow::long_def( GtkWidget* widget,
                         option_info_t<long>* option )
{
//    if( option->datatype == ENTRY_INT )
//      gtk_entry_set_text( GTK_ENTRY( option->entry ),
//                          String( option->default_value).chars() );
//    else if( option->datatype == ENTRY_HEX )
//      gtk_entry_set_text( GTK_ENTRY( option->entry ),
//                          hex2str( option->default_value).chars() );
}

void
OptionsWindow::long_max( GtkWidget* widget,
                         option_info_t<long>* option )
{
//    if( option->datatype == ENTRY_INT )
//      gtk_entry_set_text( GTK_ENTRY( option->entry ),
//                          String( option->max_value).chars() );
//    else if( option->datatype == ENTRY_HEX )
//      gtk_entry_set_text( GTK_ENTRY( option->entry ),
//                          hex2str( option->max_value).chars() );
}

void
OptionsWindow::string_def( GtkWidget* widget,
                           option_info_t<String>* option )
{
//    gtk_entry_set_text( GTK_ENTRY( option->entry ),
//                        option->default_value.chars() );
}