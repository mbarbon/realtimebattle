/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony

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

#ifndef RTB_GUIGTK__ARENADISPLAY_H__
#define RTB_GUIGTK__ARENADISPLAY_H__

#include <gtk/gtk.h>

#include "Vector2D.h"

class ArenaDisplay
{
public:
  ArenaDisplay                           ( GtkWidget*          parent );
  ~ArenaDisplay                          ();

  void update                            ();

  void set_zoom_level                    ( const int&          level );
  int  get_zoom_level                    () const
  { return zoom_level; }

private:
  void change_size                       ();
  int zoom_level;

public:
  // Functions used by DrawingObjects to draw on the arena.
  void draw_line                         ( const Vector2D&     start,
                                           const Vector2D&     direction,
                                           const double        length, 
                                           GdkColor&           colour );
  void draw_line                         ( const Vector2D&     start,
                                           const Vector2D&     direction,
                                           const double        length, 
                                           const double        thickness,
                                           GdkColor&           colour );
  void draw_rectangle                    ( const Vector2D&     start,
                                           const Vector2D&     end,
                                           GdkColor&           colour,
                                           const bool          filled );
  void draw_circle                       ( const Vector2D&     center,
                                           const double        radius,
                                           GdkColor&           colour,
                                           const bool          filled );
  void draw_arc                          ( const Vector2D&     center, 
                                           const double        inner_radius,
                                           const double        outer_radius,
                                           const double        angle1,
                                           const double        angle2,
                                           GdkColor&           colour );
  
  inline int boundary2pixel_x            ( const double&       x );
  inline int boundary2pixel_y            ( const double&       y );

private:
  // Graphics information
  void recreate_non_changeable_display   ();
  void recreate_semi_changeable_display  ();
  void recreate_current_display          ();
  void change_display                    ();

  static void move_viewed_area           ( GtkAdjustment*      adjustment,
                                           ArenaDisplay*       object_p );
  static gint expose_event               ( GtkWidget*          widget,
                                           GdkEventExpose*     event,
                                           ArenaDisplay*       object_p );
  static gint configure_event            ( GtkWidget*          widget,
                                           GdkEventConfigure*  event,
                                           ArenaDisplay*       object_p );

  GtkWidget* parent;
  GtkWidget* drawing_area;

  GtkAdjustment* x_adjustment;
  GtkAdjustment* y_adjustment;

  GtkWidget* x_scrollbar;
  GtkWidget* y_scrollbar;

  GdkPixmap* non_changeable_display;
  GdkPixmap* semi_changeable_display;
  GdkPixmap* current_display;
};

// TODO: Some better way to get the boundary.
// TODO: Some better way of getting the (drawing_area_size / boundary_size) factor.
#include "Gui.h"

inline int
ArenaDisplay::boundary2pixel_x( const double& x )
{
  return (int)( ( x - gui_p->get_arena_boundary()[0][0] ) * 
                ( drawing_area->allocation.width * zoom_level ) /
                ( gui_p->get_arena_boundary()[1][0] - 
                  gui_p->get_arena_boundary()[0][0] ) + 0.5 );
}

inline int
ArenaDisplay::boundary2pixel_y( const double& y )
{
  return (int)( ( y - gui_p->get_arena_boundary()[1][1] ) * 
                ( drawing_area->allocation.height * zoom_level ) /
                ( gui_p->get_arena_boundary()[1][1] - 
                  gui_p->get_arena_boundary()[0][1] ) + 0.5 );
}

#endif // RTB_GUIGTK__ARENADISPLAY_H__
