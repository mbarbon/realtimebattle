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

#ifndef RTB_GTKGUI__MESSAGE_WINDOW__
#define RTB_GTKGUI__MESSAGE_WINDOW__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <gtk/gtk.h>

#include <list>
#include <map>
#include <string>

#include "Structs.h"
#include "Window.h"

class MessageWindow : public Window
{
public:
  MessageWindow                  ();
  ~MessageWindow                 ();

  void create                    ( const int&             default_width  = -1,
                                   const int&             default_height = -1,
                                   const int&             default_x_pos  = -1,
                                   const int&             default_y_pos  = -1 );
  void destroy                   ();

  void set_window_title          ();

  void add_messages              ( const list<message_t>& message_list );

private:

  // Data
  string                         viewed_robot;
  map<string, list<string> >     message_history;

private:

  // Graphics helper functions.

  void set_styles                ();
  void set_viewed_robot_label    ();
  void add_messages_to_clist     ( const int&             n_o_new_messages,
                                   const int&             n_o_removed_messages );
  void add_a_message             ( const string&          message );
  void change_robot              ( const string&          robot_name );

  // Graphics callbacks
  static gint close_window       ( GtkWidget*             widget,
                                   GdkEvent*              event,
                                   MessageWindow*         object_p );
  static void change_callback    ( GtkWidget*             widget,
                                   MessageWindow*         object_p );

  // Graphics data

  GtkWidget*                     viewed_robot_label;
  GtkWidget*                     robot_data_view;
  GtkWidget*                     message_view;

  GtkStyle*                      style_rtb_message;
};

#endif // RTB_GTKGUI__MESSAGE_WINDOW__
