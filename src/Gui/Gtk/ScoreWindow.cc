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
#include <string>

#include "IntlDefs.h"
#include "ScoreWindow.h"

ScoreWindow::ScoreWindow()
{
  list_of_team_stats.clear();
  list_of_robot_stats.clear();
}

ScoreWindow::~ScoreWindow()
{
  destroy();
}

void
ScoreWindow::create( const int&  default_width,
                     const int&  default_height,
                     const int&  default_x_pos,
                     const int&  default_y_pos )
{
  // Check that the window isn't already created

  if( is_window_shown() )
    return;

  // Create the window.

  Window::create( GTK_WINDOW_TOPLEVEL,
                  "Score",
                  175, 80, // Subject to change
                  default_width,
                  default_height,
                  default_x_pos,
                  default_y_pos );

  set_window_title();
  gtk_container_border_width( GTK_CONTAINER( window_p ), 12 );

  gtk_signal_connect( GTK_OBJECT( window_p ),
                      "delete_event",
                      (GtkSignalFunc) ScoreWindow::close_window,
                      (gpointer) this );

  // TODO: No team view if not in team mode!

  // Create the pane separating the team and robot views.

  GtkWidget* the_pane = gtk_vpaned_new();
  gtk_paned_set_gutter_size( GTK_PANED( the_pane ), 14 );
  gtk_container_add( GTK_CONTAINER( window_p ), the_pane );
  gtk_widget_show( the_pane );

  // The scrolled window containing the team view.

  team_scrolled_window = gtk_scrolled_window_new( 0, 0 );
  gtk_container_set_border_width( GTK_CONTAINER( team_scrolled_window ), 0 );
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( team_scrolled_window ),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_ALWAYS );
  gtk_paned_pack1( GTK_PANED( the_pane ),
                   team_scrolled_window,
                   true, true );
  gtk_widget_show( team_scrolled_window );

  // The team view.

  create_team_view( team_scrolled_window );

  // The scrolled window containing the robot view.

  robot_scrolled_window = gtk_scrolled_window_new( 0, 0 );
  gtk_container_set_border_width( GTK_CONTAINER( robot_scrolled_window ), 0 );
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( robot_scrolled_window ),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_ALWAYS );
  gtk_paned_pack2( GTK_PANED( the_pane ),
                   robot_scrolled_window,
                   true, true );
  gtk_widget_show( robot_scrolled_window );

  // The robot view.

  create_robot_view( robot_scrolled_window );

  // Set the position of the pane
  // TODO: This could be an option (have not decided yet).
  gtk_paned_set_position( GTK_PANED( the_pane ), 110 );

  // Show the window

  gtk_widget_show( window_p );
}

void
ScoreWindow::destroy()
{
  Window::destroy();
}

void
ScoreWindow::set_window_title()
{
  gtk_window_set_title( GTK_WINDOW( window_p ), _("Score") );
}

void
ScoreWindow::create_team_view( GtkWidget*  parent )
{
  char* titles[ list_of_team_stats.size() + 1 ];

  string team_title( _("Team") );
  titles[0] = const_cast<char*>( team_title.c_str() );
  int counter = 0;
  list<string>::const_iterator sci;
  for( sci  = list_of_team_stats.begin();
       sci != list_of_team_stats.end();
       sci++ )
    {
      counter++;
      titles[counter] = const_cast<char*>( (*sci).c_str() );
    }

  team_view = gtk_clist_new_with_titles( list_of_team_stats.size() + 1,
                                         titles );

  gtk_clist_column_titles_passive( GTK_CLIST( team_view ) );
  gtk_clist_set_selection_mode( GTK_CLIST( team_view ), GTK_SELECTION_BROWSE );

  gtk_container_add( GTK_CONTAINER( parent ), team_view );
  gtk_widget_show( team_view );
}

void
ScoreWindow::create_robot_view( GtkWidget*  parent )
{
  char* titles[ list_of_robot_stats.size() + 1 ];

  string robot_title( _("Robot") );
  titles[0] = const_cast<char*>( robot_title.c_str() );
  int counter = 0;
  list<string>::const_iterator sci;
  for( sci  = list_of_robot_stats.begin();
       sci != list_of_robot_stats.end();
       sci++ )
    {
      counter++;
      titles[counter] = const_cast<char*>( (*sci).c_str() );
    }

  // TODO: It is sufficient with a clist if not in team mode.
  robot_view = gtk_ctree_new_with_titles( 1, 0, titles );

  gtk_clist_column_titles_passive( GTK_CLIST( robot_view ) );
  gtk_clist_set_selection_mode( GTK_CLIST( robot_view ), GTK_SELECTION_BROWSE );
  gtk_ctree_set_line_style( GTK_CTREE( robot_view ), GTK_CTREE_LINES_DOTTED );

  gtk_container_add( GTK_CONTAINER( parent ), robot_view );
  gtk_widget_show( robot_view );
}

gint
ScoreWindow::close_window( GtkWidget*    widget,
                           GdkEvent*     event,
                           ScoreWindow*  object_p )
{
  object_p->destroy();
  return true;
}
