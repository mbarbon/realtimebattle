/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2001  Erik Ouchterlony and Ragnar Ouchterlony

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

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <string>

#include "ArenaDisplay.h"
#include "ArenaWindow.h"
#include "IntlDefs.h"

const int ArenaWindow::arenawindow_min_height = 120;
const int ArenaWindow::arenawindow_min_width  = 185;

ArenaWindow::ArenaWindow()
{
}

ArenaWindow::~ArenaWindow()
{
  destroy();
}

void
ArenaWindow::create( const int&  default_width,
                     const int&  default_height,
                     const int&  default_x_pos,
                     const int&  default_y_pos )
{
  // Check that the window isn't already created

  if( is_window_shown() )
    return;

  // Create the window.

  Window::create( GTK_WINDOW_TOPLEVEL,
                  "Arena",
                  arenawindow_min_width,
                  arenawindow_min_height,
                  default_width,
                  default_height,
                  default_x_pos,
                  default_y_pos );

  set_window_title();
  gtk_container_border_width( GTK_CONTAINER( window_p ), 12 );

  gtk_signal_connect( GTK_OBJECT( window_p ),
                      "delete_event",
                      (GtkSignalFunc) ArenaWindow::close_window,
                      (gpointer) this );
  gtk_signal_connect( GTK_OBJECT( window_p ),
                      "key_press_event",
                      (GtkSignalFunc) ArenaWindow::keyboard_handler,
                      (gpointer) this );

  // Create the main box.

  GtkWidget* vbox = gtk_vbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_p ), vbox );
  gtk_widget_show( vbox );

  // Zoom buttons

  struct button_t { string label; GtkSignalFunc func; };

  static button_t buttons[] = {
    { (string)_("No Zoom") + (string)" [0]",
      (GtkSignalFunc) ArenaWindow::no_zoom  },
    { (string)_("Zoom In") + (string)" [+]",
      (GtkSignalFunc) ArenaWindow::zoom_in  },
    { (string)_("Zoom Out") + (string)" [-]",
      (GtkSignalFunc) ArenaWindow::zoom_out } };

  static const int number_of_buttons = sizeof( buttons ) / sizeof( buttons[0] );

  GtkWidget* button_table = gtk_table_new( 1, number_of_buttons, true );
  gtk_box_pack_start( GTK_BOX( vbox ), button_table, false, false, 0 );

  for( int i=0; i < number_of_buttons; i++ )
    {
      GtkWidget* button = gtk_button_new_with_label( buttons[i].label.c_str() );
      gtk_signal_connect( GTK_OBJECT( button ),
                          "clicked",
                          buttons[i].func,
                          (gpointer) this );
      gtk_table_attach_defaults( GTK_TABLE( button_table ),
                                 button, i, i+1, 0, 1 );
      gtk_widget_show( button );
    }

  gtk_table_set_col_spacings( GTK_TABLE( button_table ), 5 );
  gtk_widget_show( button_table );

  the_display = new ArenaDisplay( vbox );

  gtk_widget_show( window_p );
}

void
ArenaWindow::destroy()
{
  if( window_p == 0 )
    return;

  delete the_display;

  Window::destroy();
}

// Warning! Do not use widget, may be NULL or undefined
void
ArenaWindow::no_zoom( GtkWidget*    widget,
                      ArenaWindow*  object_p )
{
  object_p->the_display->set_zoom_level( 1 );
}

// Warning! Do not use widget, may be NULL or undefined
void
ArenaWindow::zoom_in( GtkWidget*    widget,
                      ArenaWindow*  object_p )
{
  int zoom = object_p->the_display->get_zoom_level();
  object_p->the_display->set_zoom_level( zoom * 2 );
}

// Warning! Do not use widget, may be NULL or undefined
void
ArenaWindow::zoom_out( GtkWidget*    widget,
                       ArenaWindow*  object_p )
{
  int zoom = object_p->the_display->get_zoom_level();
  zoom = zoom / 2;
  if( zoom < 1 )
    zoom = 1;
  object_p->the_display->set_zoom_level( zoom );
}

gint
ArenaWindow::close_window( GtkWidget*    widget,
                           GdkEvent*     event,
                           ArenaWindow*  object_p )
{
  object_p->destroy();
  return true;
}

gint
ArenaWindow::keyboard_handler( GtkWidget*    widget,
                               GdkEventKey*  event,
                               ArenaWindow*  object_p )
{
  switch( event->keyval )
    {
    case GDK_plus:
    case GDK_KP_Add:
      zoom_in( NULL, object_p );
      break;

    case GDK_minus:
    case GDK_KP_Subtract:
      zoom_out( NULL, object_p );
      break;

    case GDK_0:
    case GDK_KP_0:
    case GDK_KP_Insert:
      no_zoom( NULL, object_p );
      break;

    default:
      return false;
    }

  gtk_signal_emit_stop_by_name( GTK_OBJECT(widget), "key_press_event" );
  return false;
}
