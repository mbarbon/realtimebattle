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

#include <assert.h>
#include <gdk/gdktypes.h>
#include <gtk/gtk.h>
#include <math.h>

#include <algorithm>
#include <functional>
#include <iostream>

#include "ArenaDisplay.h"
#include "DrawingObjects.h"
#include "Gui.h"

#define GDK_360_DEGREES 23040     // 64 * 360 degrees

ArenaDisplay::ArenaDisplay( GtkWidget* parent )
{
  // Make sure parent is a container
  assert( GTK_IS_CONTAINER( parent ) );

  zoom_level = 1;

  non_changeable_display  = 0;
  semi_changeable_display = 0;
  current_display         = 0;

  // Create the table containing the scrollbars and the drawingarea.

  GtkWidget* ad_table = gtk_table_new( 2, 2, false );

  // Create the scrollbars to control the arena placement.

  x_adjustment = GTK_ADJUSTMENT( gtk_adjustment_new( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 ) );
  gtk_signal_connect( GTK_OBJECT( x_adjustment ), "value_changed",
                      (GtkSignalFunc) ArenaDisplay::move_viewed_area,
                      (gpointer) this );
  y_adjustment = GTK_ADJUSTMENT( gtk_adjustment_new( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 ) );
  gtk_signal_connect( GTK_OBJECT( y_adjustment ), "value_changed",
                      (GtkSignalFunc) ArenaDisplay::move_viewed_area,
                      (gpointer) this );

  x_scrollbar = gtk_hscrollbar_new( GTK_ADJUSTMENT( x_adjustment ) );
  gtk_table_attach( GTK_TABLE( ad_table ),
                    x_scrollbar,
                    0, 1, 1, 2,
                    static_cast<GtkAttachOptions>( GTK_EXPAND | GTK_FILL ),
                    static_cast<GtkAttachOptions>( GTK_SHRINK | GTK_FILL ),
                    2, 2 );
  gtk_widget_show( x_scrollbar );

  y_scrollbar = gtk_vscrollbar_new( GTK_ADJUSTMENT( y_adjustment ) );
  gtk_table_attach( GTK_TABLE( ad_table ),
                    y_scrollbar,
                    1, 2, 0, 1,
                    static_cast<GtkAttachOptions>( GTK_SHRINK | GTK_FILL ),
                    static_cast<GtkAttachOptions>( GTK_EXPAND | GTK_FILL ),
                    2, 2 );
  gtk_widget_show( y_scrollbar );

  // Create the arena.

  drawing_area = gtk_drawing_area_new();
  gtk_widget_set_events( drawing_area, GDK_EXPOSURE_MASK );
  gtk_signal_connect( GTK_OBJECT( drawing_area ), "expose_event",
                      (GtkSignalFunc) ArenaDisplay::expose_event,
                      (gpointer) this );
  gtk_signal_connect( GTK_OBJECT( drawing_area ), "configure_event",
                      (GtkSignalFunc) ArenaDisplay::configure_event,
                      (gpointer) this );
  gtk_table_attach( GTK_TABLE( ad_table ),
                    drawing_area,
                    0, 1, 0, 1,
                    static_cast<GtkAttachOptions>( GTK_EXPAND | GTK_FILL ),
                    static_cast<GtkAttachOptions>( GTK_EXPAND | GTK_FILL ),
                    5, 5 );

  gdk_window_set_background( drawing_area->window,
                             the_gui.get_bg_gdk_colour_p() );

  gtk_widget_show( drawing_area );

  // Prepare table and show it.

  gtk_table_set_row_spacings( GTK_TABLE( ad_table ), 0 );
  gtk_table_set_col_spacings( GTK_TABLE( ad_table ), 0 );
  gtk_container_add( GTK_CONTAINER( parent ), ad_table );
  gtk_widget_show( ad_table );
}

ArenaDisplay::~ArenaDisplay()
{
  gtk_widget_destroy( drawing_area );
  gtk_widget_destroy( x_scrollbar );
  gtk_widget_destroy( y_scrollbar );
  if( non_changeable_display != NULL )
    gdk_pixmap_unref( non_changeable_display );
  if( semi_changeable_display != NULL )
    gdk_pixmap_unref( semi_changeable_display );
  if( current_display != NULL )
    gdk_pixmap_unref( current_display );
}

void
ArenaDisplay::update()
{
  // TODO
}

void
ArenaDisplay::set_zoom_level( const int& level )
{
  if( level != zoom_level )
    {
      zoom_level = level;
      change_size();
    }
}

void
ArenaDisplay::recreate_non_changeable_display()
{
  // Unref the old pixmap
  if( non_changeable_display != NULL )
    gdk_pixmap_unref( non_changeable_display );

  int width  = drawing_area->allocation.width;
  int height = drawing_area->allocation.height;
  int actual_width  = width * zoom_level;
  int actual_height = height * zoom_level;

  // Create a new pixmap
  non_changeable_display = gdk_pixmap_new( drawing_area->window,
                                           width, height, -1 );

  // Clear the pixmap
  gdk_draw_rectangle( non_changeable_display,
                      // TODO: Use the correct background color.
                      drawing_area->style->white_gc,
                      true, 0, 0,
                      width, height );

  // TODO: Use new object_lists (when they are finished).
  list<DrawingShape*>* object_lists = the_gui.get_drawing_objects_lists();
  for_each( object_lists[WALL].begin(), object_lists[WALL].end(),
            bind2nd( mem_fun1( &DrawingShape::draw_shape ), this ) );
}

void
ArenaDisplay::recreate_semi_changeable_display()
{
  // Unref the old pixmap
  if( semi_changeable_display != NULL )
    gdk_pixmap_unref( semi_changeable_display );

  int width  = drawing_area->allocation.width;
  int height = drawing_area->allocation.height;
  int actual_width  = width * zoom_level;
  int actual_height = height * zoom_level;

  // Create a new pixmap
  semi_changeable_display = gdk_pixmap_new( drawing_area->window,
                                            width, height, -1 );

  // Copy the non_changeable_display.
  gdk_draw_pixmap( semi_changeable_display,
                   drawing_area->style->black_gc,
                   non_changeable_display,
                   0, 0,
                   0, 0,
                   width, height );

  // TODO: Use new object_lists (when they are finished).
  list<DrawingShape*>* object_lists = the_gui.get_drawing_objects_lists();
  for_each( object_lists[COOKIE].begin(), object_lists[COOKIE].end(),
            bind2nd( mem_fun1( &DrawingShape::draw_shape ), this ) );
  for_each( object_lists[MINE].begin(), object_lists[MINE].end(),
            bind2nd( mem_fun1( &DrawingShape::draw_shape ), this ) );
}

void
ArenaDisplay::recreate_current_display()
{
  // Unref the old pixmap
  if( current_display != NULL )
    gdk_pixmap_unref( current_display );

  int width  = drawing_area->allocation.width;
  int height = drawing_area->allocation.height;
  int actual_width  = width * zoom_level;
  int actual_height = height * zoom_level;

  // Create a new pixmap
  current_display = gdk_pixmap_new( drawing_area->window,
                                    width, height, -1 );

  // Copy the non_changeable_display.
  gdk_draw_pixmap( current_display,
                   drawing_area->style->black_gc,
                   semi_changeable_display,
                   0, 0,
                   0, 0,
                   width, height );

  // TODO: Use new object_lists (when they are finished).
  list<DrawingShape*>* object_lists = the_gui.get_drawing_objects_lists();
  for_each( object_lists[SHOT].begin(), object_lists[SHOT].end(),
            bind2nd( mem_fun1( &DrawingShape::draw_shape ), this ) );
  for_each( object_lists[ROBOT].begin(), object_lists[ROBOT].end(),
            bind2nd( mem_fun1( &DrawingShape::draw_shape ), this ) );
}

void
ArenaDisplay::change_size()
{
  int width  = drawing_area->allocation.width;
  int height = drawing_area->allocation.height;

  // Change the X Adjustment

  double old_x_percentile = 0.0;
  if( x_adjustment->upper != 0.0 )
    old_x_percentile = x_adjustment->value /
      (x_adjustment->upper - x_adjustment->page_size);

  x_adjustment = GTK_ADJUSTMENT( gtk_adjustment_new( old_x_percentile *
                                                     width * (zoom_level - 1),
                                                     0.0, width * zoom_level,
                                                     ((double)width)/10, width,
                                                     width ) );
  gtk_signal_connect( GTK_OBJECT( x_adjustment ), "value_changed",
                      (GtkSignalFunc) ArenaDisplay::move_viewed_area,
                      (gpointer) this );
  gtk_range_set_adjustment( GTK_RANGE( x_scrollbar ), x_adjustment );

  // Change the Y Adjustment

  double old_y_percentile = 0.0;
  if( y_adjustment->upper != 0.0 )
    old_y_percentile = y_adjustment->value /
      (y_adjustment->upper - y_adjustment->page_size);

  y_adjustment = GTK_ADJUSTMENT( gtk_adjustment_new( old_y_percentile *
                                                     height * (zoom_level - 1),
                                                     0.0, height * zoom_level,
                                                     ((double)height)/10, height,
                                                     height ) );
  gtk_signal_connect( GTK_OBJECT( y_adjustment ), "value_changed",
                      (GtkSignalFunc) ArenaDisplay::move_viewed_area,
                      (gpointer) this );
  gtk_range_set_adjustment( GTK_RANGE( y_scrollbar ), y_adjustment );

  // Recreate all pixmaps

  recreate_non_changeable_display();
  recreate_semi_changeable_display();
  recreate_current_display();
  change_display();
}

void
ArenaDisplay::change_display()
{
  gdk_draw_pixmap( drawing_area->window,
                   drawing_area->style->black_gc,
                   current_display,
                   0, 0,
                   0, 0,
                   drawing_area->allocation.width,
                   drawing_area->allocation.height );
}

void
ArenaDisplay::move_viewed_area( GtkAdjustment* adjustment,
                                ArenaDisplay*  object_p )
{
  object_p->recreate_non_changeable_display();
  object_p->recreate_semi_changeable_display();
  object_p->recreate_current_display();
  object_p->change_display();
}

gint
ArenaDisplay::expose_event( GtkWidget*      widget,
                            GdkEventExpose* event,
                            ArenaDisplay*   object_p )
{
  gdk_draw_pixmap( event->window,
                   widget->style->black_gc,
                   object_p->current_display,
                   event->area.x, event->area.y,
                   event->area.x, event->area.y,
                   event->area.width, event->area.height );

  return true;
}

gint
ArenaDisplay::configure_event( GtkWidget*         widget,
                               GdkEventConfigure* event,
                               ArenaDisplay*      object_p )
{
  object_p->change_size();
  return true;
}

// Functions used by DrawingObjects to draw on the arena.
// TODO: Do not use the drawing_area->window directly.
// TODO: Determine which pixmap to draw on.
void
ArenaDisplay::draw_line( const Vector2D&  start,
                         const Vector2D&  direction,
                         const double     length,
                         const double     thickness,
                         GdkColor&        colour )
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
      g_points[i].x = boundary2pixel_x( vector_points[i][0] );
      g_points[i].y = boundary2pixel_y( vector_points[i][1] );
    }
  gdk_draw_polygon( drawing_area->window, colour_gc, true, g_points, 4 );

  gdk_gc_destroy( colour_gc );
}

void
ArenaDisplay::draw_line( const Vector2D&  start,
                         const Vector2D&  direction,
                         const double     length,
                         GdkColor&        colour )
{
  GdkGC * colour_gc;  
  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, &colour );

  Vector2D end_point = start + length * direction;

  gdk_draw_line( drawing_area->window, colour_gc,
                 boundary2pixel_x( start[0] ), 
                 boundary2pixel_y( start[1] ), 
                 boundary2pixel_x( end_point[0] ), 
                 boundary2pixel_y( end_point[1] ) );

  gdk_gc_destroy( colour_gc );
}

void
ArenaDisplay::draw_rectangle( const Vector2D&  start,
                              const Vector2D&  end,
                              GdkColor&        colour,
                              const bool       filled )
{
  GdkGC * colour_gc;

  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, &colour );

  gdk_draw_rectangle( drawing_area->window,
                      colour_gc,
                      filled,
                      boundary2pixel_x( start[0] ),
                      boundary2pixel_y( end[1] ),
                      boundary2pixel_x( end[0] - start[0] ),
                      boundary2pixel_y( end[1] - start[1] ) );

  gdk_gc_destroy( colour_gc );
}

void
ArenaDisplay::draw_circle( const Vector2D&  center,
                           const double     radius,
                           GdkColor&        colour,
                           const bool       filled )
{
  GdkGC * colour_gc;

  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, &colour );

  double r;
  if( ( r = radius * ( drawing_area->allocation.height * zoom_level ) /
        ( the_gui.get_arena_boundary()[1][1] - 
          the_gui.get_arena_boundary()[0][1] ) ) > 1.0 )
    {
      gdk_draw_arc( drawing_area->window,
                    colour_gc,
                    filled,
                    boundary2pixel_x( center[0]-radius ),
                    boundary2pixel_y( center[1]+radius ),
                    (int)(r*2.0), (int)(r*2.0),
                    0, GDK_360_DEGREES );
    }
  else
    {
      gdk_draw_point( drawing_area->window,
                      colour_gc,
                      boundary2pixel_x( center[0] ), 
                      boundary2pixel_y( center[1] ) );
    }
  gdk_gc_destroy( colour_gc );
}

void
ArenaDisplay::draw_arc( const Vector2D&  center, 
                        const double     inner_radius,
                        const double     outer_radius,
                        const double     angle1,
                        const double     angle2,
                        GdkColor&        colour )
{
  const double rad2GDK = ((double)GDK_360_DEGREES) / ( 2.0 * M_PI );
      
  gint a1 = (gint)( ( angle1 < 0.0 ? angle1 + 2 * M_PI : angle1 ) 
                    * rad2GDK + 0.5 );
      
  double angle_diff = angle2 - angle1;
  gint a2 = (gint)( ( angle_diff < 0.0 ? angle_diff + 2 * M_PI : angle_diff ) 
                    * rad2GDK + 0.5 );
    
    
  GdkGC * colour_gc;

  colour_gc = gdk_gc_new( drawing_area->window );
  gdk_gc_set_foreground( colour_gc, &colour );

  int line_width = (int)((outer_radius - inner_radius) *
                         ( drawing_area->allocation.height * zoom_level ) /
                         ( the_gui.get_arena_boundary()[1][1] - 
                           the_gui.get_arena_boundary()[0][1] ) + 0.5);
  gdk_gc_set_line_attributes (colour_gc,
                              line_width,
                              GDK_LINE_SOLID,
                              GDK_CAP_NOT_LAST,
                              GDK_JOIN_MITER);

  double r = 0.5 * ( outer_radius + inner_radius );
  int box_size = (int)( r * 2.0 * ( drawing_area->allocation.height * zoom_level ) /
                        ( the_gui.get_arena_boundary()[1][1] - 
                          the_gui.get_arena_boundary()[0][1] ) + 0.5 );
  if( box_size >= 2.0 )
    {
      gdk_draw_arc( drawing_area->window,
                    colour_gc,
                    false,
                    boundary2pixel_x( center[0] - r ),
                    boundary2pixel_y( center[1] + r ),
                    box_size, box_size,
                    a1, a2 );
    }
  else
    {
      gdk_draw_point( drawing_area->window,
                      colour_gc,
                      boundary2pixel_x( center[0] ), 
                      boundary2pixel_y( center[1] ) );
    }
  gdk_gc_destroy( colour_gc );
}
