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

#include "ArenaWindow.h"
#include "ArenaBase.h"
#include "ArenaController.h"
#include "ArenaRealTime.h"
#include "Extras.h"
#include "Robot.h"
#include "Shot.h"
#include "String.h"
#include "Structs.h"
#include "Vector2D.h"

#define GDK_360_DEGREES 23040     // 64 * 360 degrees

ArenaWindow::ArenaWindow( const int default_width,
                          const int default_height,
                          const int default_x_pos,
                          const int default_y_pos )
{
  // The window widget

  window_p = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  gtk_widget_set_name( window_p, "RTB Arena" );

  set_window_title();

  gtk_container_border_width( GTK_CONTAINER( window_p ), 12 );

  if( default_width != -1 && default_height != -1 )
#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
    {
      gtk_window_set_default_size( GTK_WINDOW( window_p ),
                                   default_width, default_height );
      gtk_widget_set_usize( window_p , 185, 120 );
    }
#else
    gtk_widget_set_usize( window_p, default_width, default_height );
#endif
  if( default_x_pos != -1 && default_y_pos != -1 )
    gtk_widget_set_uposition( window_p, default_x_pos, default_y_pos );

  gtk_signal_connect( GTK_OBJECT( window_p ), "delete_event",
                      (GtkSignalFunc) ArenaWindow::hide_window,
                      (gpointer) this );

  // Main box

  GtkWidget* vbox = gtk_vbox_new( FALSE, 10 );
  gtk_container_add( GTK_CONTAINER( window_p ), vbox );
  gtk_widget_show( vbox );
  
  // Zoom buttons

  struct button_t { String label; GtkSignalFunc func; };

  struct button_t buttons[] = {
    { " No Zoom " , (GtkSignalFunc) ArenaWindow::no_zoom  },
    { " Zoom In " , (GtkSignalFunc) ArenaWindow::zoom_in  },
    { " Zoom Out ", (GtkSignalFunc) ArenaWindow::zoom_out } };

  GtkWidget* button_table = gtk_table_new( 1, 3, TRUE );
  gtk_box_pack_start( GTK_BOX( vbox ), button_table, FALSE, FALSE, 0 );

  for( int i=0; i < 3; i++ )
    {
      GtkWidget* button =
        gtk_button_new_with_label( buttons[i].label.chars() );
      gtk_signal_connect( GTK_OBJECT( button ), "clicked",
                          buttons[i].func, (gpointer) this );
      gtk_table_attach_defaults( GTK_TABLE( button_table ),
                                 button, i, i+1, 0, 1 );
      gtk_widget_show( button );
    }

  gtk_table_set_col_spacings( GTK_TABLE( button_table ), 5 );
  gtk_widget_show( button_table );

  // Scrolled Window

  scrolled_window = gtk_scrolled_window_new( NULL, NULL );
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrolled_window ),
                                  GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS );
  gtk_container_add( GTK_CONTAINER( vbox ), scrolled_window );
  gtk_widget_show( scrolled_window );

  // Drawing Area

  drawing_area = gtk_drawing_area_new();
  gtk_drawing_area_size( GTK_DRAWING_AREA( drawing_area ),
                         default_width - 48, default_height - 80 );
  gtk_signal_connect( GTK_OBJECT( drawing_area ), "expose_event",
                      (GtkSignalFunc) ArenaWindow::redraw, (gpointer) this );

  gtk_widget_set_events( drawing_area, GDK_EXPOSURE_MASK );

#if GTK_MAJOR_VERSION == 1 && GTK_MINOR_VERSION >= 1
  gtk_scrolled_window_add_with_viewport
    ( GTK_SCROLLED_WINDOW( scrolled_window ), drawing_area );
#else
  gtk_container_add( GTK_CONTAINER( scrolled_window ), drawing_area );
#endif
  gtk_widget_show( drawing_area );

  gtk_widget_show( window_p  );
  window_shown = true;

  gdk_window_set_background( drawing_area->window,
                             the_arena.get_bg_gdk_colour_p() );
  gdk_window_clear( drawing_area->window );
  zoom = 1;
}

ArenaWindow::~ArenaWindow()
{
  gtk_widget_destroy( window_p );
}

void
ArenaWindow::set_window_title()
{
  String title = "RealTimeBattle Arena  " +
    the_arena.get_current_arena_filename();
  gtk_window_set_title( GTK_WINDOW( window_p ), title.chars() );
}

void
ArenaWindow::clear_area()
{
  GdkGC * colour_gc;

  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, the_arena.get_bg_gdk_colour_p() );

  gdk_draw_rectangle( drawing_area->window,
                      colour_gc,
                      true,
                      0, 0, drawing_area->allocation.width,
                      drawing_area->allocation.height );

  gdk_gc_destroy( colour_gc );
}

void
ArenaWindow::draw_everything()
{
  List<Shape>* object_lists;

  object_lists = the_arena.get_object_lists();
  ListIterator<Shape> li;

  // Must begin with innercircles (they are destructive)
  for( int obj_type=WALL_INNERCIRCLE_T; obj_type > SHOT_T; obj_type--) 
    {
      for( object_lists[obj_type].first(li); li.ok(); li++ )
        {
          if( !( ( obj_type == MINE_T || obj_type == COOKIE_T ) &&
                 !( (Extras*)li() )->is_alive() ) )
            {
              li()->draw_shape( false );
            }
        }
    }

  draw_moving_objects( false );
}

void
ArenaWindow::draw_moving_objects( const bool clear_objects_first )
{
  List<Shape>* object_lists = the_arena.get_object_lists();
  Robot* robotp;

  if( ( scrolled_window->allocation.width - 24 != scrolled_window_size[0]) ||
      ( scrolled_window->allocation.height - 24 !=  scrolled_window_size[1]) )
    drawing_area_scale_changed();

  ListIterator<Shape> li;
  for( object_lists[SHOT_T].first(li); li.ok(); li++ )
    if( ((Shot*)li())->is_alive() )
      ((Shot*)li())->draw_shape( clear_objects_first );

  ListIterator<Shape> li2;
  for( object_lists[ROBOT_T].first(li2); li2.ok(); li2++ )
    {
      robotp = (Robot*)li2();

      if( robotp->is_alive() )
        {
          robotp->draw_shape( clear_objects_first );
          robotp->draw_radar_and_cannon();
        }
    }
}

void
ArenaWindow::draw_circle( const Vector2D& center, const double radius,
                          GdkColor& colour, const bool filled )
{
  GdkGC * colour_gc;

  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, &colour );

  double r;
  if( ( r = radius * drawing_area_scale ) > 1.0 )
    {
      gdk_draw_arc( drawing_area->window,
                    colour_gc,
                    filled,
                    boundary2pixel( center[0]-radius, 0 ),
                    boundary2pixel( center[1]-radius, 1 ),
                    (int)(r*2.0), (int)(r*2.0),
                    0, GDK_360_DEGREES );
    }
  else
    {
      gdk_draw_point( drawing_area->window,
                      colour_gc,
                      boundary2pixel( center[0], 0 ), 
                      boundary2pixel( center[1], 1 ) );
    }
  gdk_gc_destroy( colour_gc );
}

void
ArenaWindow::draw_rectangle( const Vector2D& start,
                             const Vector2D& end,
                             GdkColor& colour,
                             const bool filled )
{
  GdkGC * colour_gc;

  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, &colour );

  gdk_draw_rectangle( drawing_area->window,
                      colour_gc,
                      filled,
                      boundary2pixel( start[0], 0 ),
                      boundary2pixel( start[1], 1 ),
                      boundary2pixel( end[0] - start[0], 0 ),
                      boundary2pixel( end[1] - start[1], 1 ) );

  gdk_gc_destroy( colour_gc );
}

void
ArenaWindow::draw_line( const Vector2D& start, const Vector2D& direction,
                        const double length, const double thickness,
                        GdkColor& colour )
{
  GdkGC * colour_gc;
  GdkPoint g_points[4];
  Vector2D vector_points[4];

  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, &colour );

  Vector2D line_thick = unit( direction );
  line_thick = rotate90( line_thick );
  line_thick *= thickness;
  vector_points[0] = start + line_thick;
  vector_points[1] = start - line_thick;
  vector_points[2] = start - line_thick + direction * length;
  vector_points[3] = start + line_thick + direction * length;

  for(int i=0;i<4;i++)
    {
      g_points[i].x = boundary2pixel( vector_points[i][0], 0 );
      g_points[i].y = boundary2pixel( vector_points[i][1], 1 );
    }
  gdk_draw_polygon( drawing_area->window, colour_gc, true, g_points, 4 );

  gdk_gc_destroy( colour_gc );
}

void
ArenaWindow::drawing_area_scale_changed()
{
  int width = scrolled_window->allocation.width - 24;
  int height = scrolled_window->allocation.height - 24;
  scrolled_window_size = Vector2D( (double)width,
                                   (double)height );
  double w = (double)( width * zoom );
  double h = (double)( height * zoom );
  double bw = the_arena.get_boundary()[1][0] -
    the_arena.get_boundary()[0][0];
  double bh = the_arena.get_boundary()[1][1] -
    the_arena.get_boundary()[0][1];
  if( w / bw >= h / bh )
    {
      drawing_area_scale = h / bh;
      w = drawing_area_scale * bw;
    }
  else
    {
      drawing_area_scale = w / bw;
      h = drawing_area_scale * bh;
    }

  gtk_widget_set_usize( drawing_area, (int)w, int(h) );
  clear_area();
  draw_everything();
}

void
ArenaWindow::set_window_shown( bool win_shown )
{
  window_shown = win_shown;
}

void
ArenaWindow::hide_window( GtkWidget* widget,
                          class ArenaWindow* arenawindow_p )
{
  if( arenawindow_p->is_window_shown() )
    {
      gtk_widget_hide( arenawindow_p->get_window_p() );
      arenawindow_p->set_window_shown( false );
    }
}

void
ArenaWindow::show_window( GtkWidget* widget,
                          class ArenaWindow* arenawindow_p )
{
  if( !arenawindow_p->is_window_shown() )
    {
      gtk_widget_show( arenawindow_p->get_window_p() );
      arenawindow_p->set_window_shown( true );
    }
}

void
ArenaWindow::no_zoom( GtkWidget* widget, class ArenaWindow* arenawindow_p )
{
  arenawindow_p->set_zoom( 1 );
  arenawindow_p->drawing_area_scale_changed();
}

void
ArenaWindow::zoom_in( GtkWidget* widget, class ArenaWindow* arenawindow_p )
{
  int z = arenawindow_p->get_zoom();
  z++;
  arenawindow_p->set_zoom( z );
  arenawindow_p->drawing_area_scale_changed();
}

void
ArenaWindow::zoom_out( GtkWidget* widget, class ArenaWindow* arenawindow_p )
{
  int z = arenawindow_p->get_zoom();
  if( z > 1 )
    z--;
  arenawindow_p->set_zoom( z );
  arenawindow_p->drawing_area_scale_changed();
}

gint
ArenaWindow::redraw( GtkWidget* widget, GdkEventExpose* event,
                     class ArenaWindow* arenawindow_p )
{
  if( the_arena.get_state() == GAME_IN_PROGRESS )
    arenawindow_p->draw_everything();
  return FALSE;
}