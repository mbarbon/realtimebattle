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

#include <list>
#include <map>
#include <string>

#include "Gui.h"
#include "GuiVarious.h"
#include "IntlDefs.h"
#include "MessageWindow.h"
#include "Structs.h"
#include "String.h"

MessageWindow::MessageWindow()
{
  viewed_robot.erase();
}

MessageWindow::~MessageWindow()
{
  destroy();
}

void
MessageWindow::create( const int&  default_width,
                       const int&  default_height,
                       const int&  default_x_pos,
                       const int&  default_y_pos )
{
  // Check that the window isn't already created

  if( is_window_shown() )
    return;

  // Create the window.

  Window::create( GTK_WINDOW_TOPLEVEL,
                  "Message",
                  300, 110, // Subject to change
                  default_width,
                  default_height,
                  default_x_pos,
                  default_y_pos );

  set_window_title();
  gtk_container_border_width( GTK_CONTAINER( window_p ), 12 );

  gtk_signal_connect( GTK_OBJECT( window_p ),
                      "delete_event",
                      (GtkSignalFunc) MessageWindow::close_window,
                      (gpointer) this );

  // Main box

  GtkWidget* main_box = gtk_vbox_new( false, 10 );
  gtk_container_add( GTK_CONTAINER( window_p ), main_box );
  gtk_widget_show( main_box );

  // Create change robot button.

  GtkWidget* button = gtk_button_new_with_label( _("Change robot" ) );
  gtk_box_pack_start( GTK_BOX( main_box ), button, false, true, 0 );
  gtk_signal_connect( GTK_OBJECT( button ),
                      "clicked",
                      (GtkSignalFunc) MessageWindow::change_callback,
                      (gpointer) this );
  gtk_widget_show( button );

  // Create a label to hold the currently viewed robot.

  viewed_robot_label = gtk_label_new( "" );
  gtk_box_pack_start( GTK_BOX( main_box ), viewed_robot_label, false, true, 0 );
  gtk_widget_show( viewed_robot_label );

  set_viewed_robot_label();

  // Create a paned view that separate robot info view and message view.

  GtkWidget* the_pane = gtk_vpaned_new();
  gtk_paned_set_gutter_size( GTK_PANED( the_pane ), 14 );
  gtk_box_pack_start( GTK_BOX( main_box ), the_pane, true, true, 0 );
  gtk_widget_show( the_pane );

  // Create the scolled window to contain the robot info clist.

  GtkWidget* scroll_window = gtk_scrolled_window_new( 0, 0 );
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scroll_window ),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_ALWAYS );
  gtk_paned_pack1( GTK_PANED( the_pane ),
                   scroll_window,
                   true, true );
  gtk_widget_show( scroll_window );

  // Create the robot info view.
  // The robot info view is thought to contain information such
  // as netload, used cputime.

  robot_data_view = gtk_clist_new( 2 );
  gtk_clist_column_titles_hide( GTK_CLIST( robot_data_view ) );
  gtk_clist_set_selection_mode( GTK_CLIST( robot_data_view ),
                                GTK_SELECTION_BROWSE );
  gtk_container_add( GTK_CONTAINER( scroll_window ), robot_data_view );
  gtk_widget_show( robot_data_view );

  // Create the scrolled window to contatin the message view.

  scroll_window = gtk_scrolled_window_new( 0, 0 );
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scroll_window ),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_ALWAYS );
  gtk_paned_pack2( GTK_PANED( the_pane ),
                   scroll_window,
                   true, true );
  gtk_widget_show( scroll_window );

  // Create the message view.

  message_view = gtk_clist_new( 2 );
  gtk_clist_column_titles_hide( GTK_CLIST( message_view ) );
  gtk_clist_set_selection_mode( GTK_CLIST( message_view ),
                                GTK_SELECTION_BROWSE );
  gtk_container_add( GTK_CONTAINER( scroll_window ), message_view );
  gtk_widget_show( message_view );

  gtk_widget_show( window_p );

  set_styles();
}

void
MessageWindow::destroy()
{
  Window::destroy();
}

void
MessageWindow::set_window_title()
{
  gtk_window_set_title( GTK_WINDOW( window_p ), _("Robot messages") );
}

void
MessageWindow::set_viewed_robot_label()
{
  string label;
  if( viewed_robot.empty() )
    label = (string)_("No robot is being viewed.");
  else
    label = (string)_("Viewed_robot:") + (string)" " + viewed_robot;

  gtk_label_set_text( GTK_LABEL( viewed_robot_label ), label.c_str() );
}

void
MessageWindow::set_styles()
{
  style_rtb_message = gtk_rc_get_style( message_view );
  if( style_rtb_message == 0 )
    style_rtb_message = gtk_style_new();
  else
    style_rtb_message = gtk_style_copy( style_rtb_message );

  GdkColor colour_rtb_message =
    make_gdk_colour( the_gui.get_gtk_opts()->get_l( "RTB message colour" ) );

  style_rtb_message->fg[GTK_STATE_NORMAL] = colour_rtb_message;
  style_rtb_message->fg[GTK_STATE_SELECTED] = colour_rtb_message;
}

void
MessageWindow::add_messages( const list<message_t>& message_list )
{
  int viewed_robot_new_messages = 0;
  int viewed_robot_removed_messages = 0;

  bool viewed_is_sender = false;
  list<string>* string_list_p = 0;
  list<message_t>::const_iterator mci;
  for( mci  = message_list.begin();
       mci != message_list.end();
       mci++ )
    {
      viewed_is_sender = ((*mci).sender == viewed_robot);
      string_list_p = &( message_history[ (*mci).sender ] );
      string_list_p->push_front( (*mci).message );

      if( viewed_is_sender )
        viewed_robot_new_messages++;

      if( string_list_p->size() >
          (unsigned long)the_gui.get_gtk_opts()->get_l( "Message history length" ) )
        {
          string_list_p->pop_back();
          if( viewed_is_sender )
            viewed_robot_removed_messages++;
        }
    }

  add_messages_to_clist( viewed_robot_new_messages,
                         viewed_robot_removed_messages );
}

void
MessageWindow::add_messages_to_clist( const int&  n_o_new_messages,
                                      const int&  n_o_removed_messages )
{
  if( !is_window_shown() )
    return;

  gtk_clist_freeze( GTK_CLIST( message_view ) );

  list<string>* string_list_p = &( message_history[ viewed_robot ] );
  int counter = 0;
  list<string>::const_iterator sci;
  for( sci  = string_list_p->begin();
       sci != string_list_p->end() || counter < n_o_new_messages;
       sci++ )
    {
      add_a_message( *sci );
      counter++;
    }

  for( int i = 0; i < n_o_removed_messages; i++ )
    gtk_clist_remove( GTK_CLIST( message_view ),
                      the_gui.get_gtk_opts()->get_l( "Message history length" ) );

  gtk_clist_thaw( GTK_CLIST( message_view ) );
}

void
MessageWindow::add_a_message( const string&  message )
{
  if( !is_window_shown() )
    return;

  char* cmessage;
  if( message.substr( 0, 4 ) == "RTB:" )
    cmessage = copy_to_c_string( message.substr( 4, string::npos ) );
  else
    cmessage = copy_to_c_string( message );
  char* row_strings[1] = { cmessage };

  int row = 0;
  row = gtk_clist_insert( GTK_CLIST( message_view ), row, row_strings );

  if( message.substr( 0, 4 ) == "RTB:" )
    gtk_clist_set_row_style( GTK_CLIST( message_view ),
                             row,
                             style_rtb_message );

  delete cmessage;
}

void
MessageWindow::change_robot( const string&  robot_name )
{
  viewed_robot = robot_name;
  gtk_clist_clear( GTK_CLIST( message_view ) );
  add_messages_to_clist( -1, 0 );
  // TODO: Change the robot info view.
}

gint
MessageWindow::close_window( GtkWidget*      widget,
                             GdkEvent*       event,
                             MessageWindow*  object_p )
{
  object_p->destroy();
  return true;
}

void
MessageWindow::change_callback( GtkWidget*      widget,
                                MessageWindow*  object_p )
{
  // TODO
}
