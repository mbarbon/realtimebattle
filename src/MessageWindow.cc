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

#include <gtk/gtk.h>

#include "MessageWindow.h"
#include "IntlDefs.h"
#include "ScoreWindow.h"
#include "ControlWindow.h"
#include "Gui.h"
#include "ArenaController.h"
#include "ArenaRealTime.h"
#include "String.h"
#include "Robot.h"
#include "Various.h"

extern class ControlWindow* controlwindow_p;

MessageWindow::MessageWindow( const int default_width,
                              const int default_height,
                              const int default_x_pos,
                              const int default_y_pos )
{
  // The window widget

  viewed_robot = NULL;

  window_p = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  gtk_widget_set_name( window_p, "RTB Message" );

  set_window_title();

  gtk_container_border_width( GTK_CONTAINER( window_p ), 12 );

  if( default_width != -1 && default_height != -1 )
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
    {
      gtk_window_set_default_size( GTK_WINDOW( window_p ),
                                   default_width, default_height );
      gtk_widget_set_usize( window_p , 300, 110 );
    }
#else
    gtk_widget_set_usize( window_p, default_width, default_height );
#endif
  if( default_x_pos != -1 && default_y_pos != -1 )
    gtk_widget_set_uposition( window_p, default_x_pos, default_y_pos );

  gtk_signal_connect( GTK_OBJECT( window_p ), "delete_event",
                      (GtkSignalFunc) MessageWindow::hide_window,
                      (gpointer) this );

  // Main box

  GtkWidget* vbox = gtk_vbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_p ), vbox );
  gtk_widget_show( vbox );

  // Buttons

  GtkWidget* hbox = gtk_hbox_new( FALSE, 10 );
  gtk_box_pack_start( GTK_BOX( vbox ), hbox, FALSE, FALSE, 0 );
  gtk_widget_show( hbox );

  struct button_t { String label; GtkSignalFunc func; };
  struct button_t buttons[] = {
    { (String)_(" Clear all messages "), 
      (GtkSignalFunc) MessageWindow::clear_clist },
    { (String)_(" Show only marked robot "), 
      (GtkSignalFunc) MessageWindow::show_one_robot },
    { (String)_(" Show all "),
      (GtkSignalFunc) MessageWindow::show_all } };
  for(int i = 0;i < 3; i++)
    {
      GtkWidget* button =
        gtk_button_new_with_label( buttons[i].label.chars() );
      gtk_signal_connect( GTK_OBJECT( button ), "clicked",
                          (GtkSignalFunc) buttons[i].func,
                          (gpointer) this );
      gtk_box_pack_start( GTK_BOX( hbox ), button,
                          TRUE, TRUE, 0 );
      gtk_widget_show( button );
    }
    
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  GtkObject* hadj = gtk_adjustment_new( 0.0, 0.0, 100.0, 1.0, 1.0, 1.0 );
  GtkObject* vadj = gtk_adjustment_new( 0.0, 0.0, 100.0, 1.0, 1.0, 1.0 );
  GtkWidget* scrolled_win =
    gtk_scrolled_window_new( GTK_ADJUSTMENT( hadj ),
                             GTK_ADJUSTMENT( vadj ) );
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrolled_win ),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC );

  gtk_box_pack_start( GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0 );
  gtk_widget_show( scrolled_win );
#endif

  char* titles[2] = { _(" Robot "), _(" Message ") };
  clist = gtk_clist_new_with_titles( 2, titles );
  gtk_clist_set_selection_mode( GTK_CLIST( clist ),
                                GTK_SELECTION_BROWSE );
  gtk_clist_set_column_width( GTK_CLIST( clist ), 0, 130 );
  gtk_clist_set_column_width( GTK_CLIST( clist ), 1, 1000 );
  gtk_clist_set_column_justification( GTK_CLIST( clist ), 0,
                                      GTK_JUSTIFY_LEFT );
  gtk_clist_set_column_justification( GTK_CLIST( clist ), 1,
                                      GTK_JUSTIFY_LEFT );
  gtk_clist_column_titles_passive( GTK_CLIST( clist ) );
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  gtk_clist_set_shadow_type( GTK_CLIST( clist ), GTK_SHADOW_IN );
  gtk_container_add( GTK_CONTAINER( scrolled_win ), clist );

  GtkStyle* clist_style = gtk_style_copy(gtk_rc_get_style(window_p));
  clist_style->base[GTK_STATE_NORMAL] = *(the_gui.get_bg_gdk_colour_p());
  clist_style->base[GTK_STATE_ACTIVE] = make_gdk_colour( 0xffffff );
  clist_style->bg[GTK_STATE_SELECTED] = make_gdk_colour( 0xf0d2b4 );
  clist_style->fg[GTK_STATE_SELECTED] = *(the_gui.get_fg_gdk_colour_p());
  gtk_widget_set_style( clist, clist_style );
#else
  gtk_clist_set_border( GTK_CLIST( clist ), GTK_SHADOW_IN );
  gtk_clist_set_policy( GTK_CLIST( clist ),
                        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
  gtk_box_pack_start( GTK_BOX( vbox ), clist, TRUE, TRUE, 0 );
#endif
  gtk_widget_show( clist );

#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  rtb_message_row_style = gtk_style_copy(gtk_rc_get_style(window_p));
  rtb_message_row_style->base[GTK_STATE_NORMAL] =
    make_gdk_colour( the_gui.get_bg_rgb_colour() );
  rtb_message_row_style->base[GTK_STATE_ACTIVE] = make_gdk_colour( 0xffffff );
  rtb_message_row_style->bg[GTK_STATE_SELECTED] = make_gdk_colour( 0xf0d2b4 );
  rtb_message_row_style->fg[GTK_STATE_NORMAL] = 
    *( the_gui.get_rtb_message_gdk_colour_p() );
  rtb_message_row_style->fg[GTK_STATE_SELECTED] =
    *( the_gui.get_rtb_message_gdk_colour_p() );

  robot_message_row_style = gtk_style_copy(gtk_rc_get_style(window_p));
  robot_message_row_style->base[GTK_STATE_NORMAL] =
    make_gdk_colour( the_gui.get_bg_rgb_colour() );
  robot_message_row_style->base[GTK_STATE_ACTIVE] = make_gdk_colour( 0xffffff );
  robot_message_row_style->bg[GTK_STATE_SELECTED] = make_gdk_colour( 0xf0d2b4 );
  robot_message_row_style->fg[GTK_STATE_NORMAL] = *( the_gui.get_fg_gdk_colour_p() );
  robot_message_row_style->fg[GTK_STATE_SELECTED] = *( the_gui.get_fg_gdk_colour_p() );
#endif

  if( window_shown = ( controlwindow_p->is_messagewindow_checked() ) )
    gtk_widget_show_now( window_p );
}

MessageWindow::~MessageWindow()
{
  gtk_widget_destroy( window_p );
}

void
MessageWindow::set_window_title()
{
  String title = (String)_("Messages");
  if( viewed_robot != NULL )
    title += "  -  " + viewed_robot->get_robot_name();
  else
    title += "  -  " + (String)_(" All ");

  gtk_window_set_title( GTK_WINDOW( window_p ), title.chars() );
}

void
MessageWindow::add_message( const String& name_of_messager, 
                            const String& message )
{
  if( window_shown )
    {
      //      gtk_clist_freeze( GTK_CLIST( clist ) );
      if( viewed_robot != NULL &&
          viewed_robot->get_robot_name() != name_of_messager &&
          name_of_messager != "RealTimeBattle" )
        return;

      char* lst[2] = { name_of_messager.non_const_chars(),
                       message.non_const_chars() };
  
      int row = 0;
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
      row = gtk_clist_insert( GTK_CLIST( clist ), row, lst );
#else
      gtk_clist_insert( GTK_CLIST( clist ), row, lst );
#endif


#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
      if( name_of_messager == "RealTimeBattle" )
        gtk_clist_set_row_style( GTK_CLIST( clist ), row, rtb_message_row_style );
      else
        gtk_clist_set_row_style( GTK_CLIST( clist ), row, robot_message_row_style );
#else
      GdkColor* fg_colour = NULL;
      if( name_of_messager == "RealTimeBattle" )
        fg_colour = the_gui.get_rtb_message_gdk_colour_p();
      else
        fg_colour = the_gui.get_fg_gdk_colour_p();

      gtk_clist_set_foreground( GTK_CLIST( clist ), row,
                                fg_colour );
      gtk_clist_set_background( GTK_CLIST( clist ), row,
                                the_gui.get_bg_gdk_colour_p() );
#endif
      //      gtk_clist_thaw( GTK_CLIST( clist ) );
    }
}

void
MessageWindow::freeze_clist()
{
  if( window_shown )
    gtk_clist_freeze( GTK_CLIST( clist ) );
}

void
MessageWindow::thaw_clist()
{
  if( window_shown )
    gtk_clist_thaw( GTK_CLIST( clist ) );
}

void
MessageWindow::set_viewed_robot( class Robot* robot_p )
{
  viewed_robot = robot_p;
}

void
MessageWindow::set_window_shown( bool win_shown )
{
  window_shown = win_shown;
}

// Warning: event can be NULL, do not use event!
void
MessageWindow::hide_window( GtkWidget* widget, GdkEvent* event,
                            class MessageWindow* messagewindow_p )
{
  if( messagewindow_p->is_window_shown() )
    {
      gtk_widget_hide( messagewindow_p->get_window_p() );
      messagewindow_p->set_window_shown( false );
      if( controlwindow_p->is_messagewindow_checked() )
        {
          GtkWidget* menu_item = controlwindow_p->get_show_message_menu_item();
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
          gtk_check_menu_item_set_active( GTK_CHECK_MENU_ITEM( menu_item ), FALSE );
#else
          gtk_check_menu_item_set_state( GTK_CHECK_MENU_ITEM( menu_item ), FALSE );
#endif
        }
    }
}

void
MessageWindow::show_window( GtkWidget* widget,
                            class MessageWindow* messagewindow_p )
{
  if( !messagewindow_p->is_window_shown() )
    {
      gtk_widget_show_now( messagewindow_p->get_window_p() );
      messagewindow_p->set_window_shown( true );
    }
}

// warning Do not use the widget variable. It may be NULL.
void
MessageWindow::clear_clist( GtkWidget* widget,
                            class MessageWindow* messagewindow_p )
{
  gtk_clist_clear( GTK_CLIST( messagewindow_p->get_clist() ) );
}

void
MessageWindow::show_one_robot( GtkWidget* widget,
                               class MessageWindow* messagewindow_p )
{
  messagewindow_p->set_viewed_robot
    ( the_gui.get_scorewindow_p()->get_selected_robot() );
  messagewindow_p->set_window_title();
}

void
MessageWindow::show_all( GtkWidget* widget,
                         class MessageWindow* messagewindow_p )
{
  messagewindow_p->set_viewed_robot( NULL );
  messagewindow_p->set_window_title();
}
