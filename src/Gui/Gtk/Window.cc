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

#include "Window.h"

Window::Window()
{
  window_p = 0;
}

Window::~Window()
{
  destroy();
}

void
Window::create( const GtkWindowType  window_type,
                const string&        gtk_name,
                const int&           min_width,
                const int&           min_height,
                const int&           default_width,
                const int&           default_height,
                const int&           default_x_pos,
                const int&           default_y_pos )
{
  window_p = gtk_window_new( window_type );

  string rtb_prefix = "RTB ";
  if( gtk_name != "" )
    gtk_widget_set_name( window_p, (rtb_prefix + gtk_name).c_str() );

  if( min_width != -1 && min_height != -1 )
    gtk_widget_set_usize( window_p, min_width, min_height );
  else if( default_width != -1 && default_height != -1 )
    gtk_widget_set_usize( window_p, default_width, default_height );

  if( default_width != -1 && default_height != -1 )
    gtk_window_set_default_size( GTK_WINDOW( window_p ),
                                 default_width, default_height );

  if( default_x_pos != -1 && default_y_pos != -1 )
    gtk_widget_set_uposition( window_p, default_x_pos, default_y_pos );
}

void
Window::destroy()
{
  if( window_p != 0 )
    gtk_widget_destroy( window_p );
  window_p = 0;
}

bool
Window::is_window_shown() const
{
  return window_p != 0;
}
