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
#include <config.h>
#endif

#include <typeinfo>
#include <math.h>

#include "DrawingObjects.h"
#include "Shape.h"
#include "Vector2D.h"
#include "GuiVarious.h"
#include "OptionHandler.h"
#include "Gui.h"
#include "ArenaController.h"
#include "GuiStructs.h"
#include "Robot.h"
#include "GeometricalObjects.h"
#include "ArenaWindow.h"
#include "ScoreWindow.h"
#include "String.h"

extern class Gui* gui_p;

#define line_p ((Line*)shape_p)
#define circle_p ((Circle*)shape_p)
#define icircle_p ((InnerCircle*)shape_p)
#define arc_p ((Arc*)shape_p)
// --------- DrawingShape -----------

DrawingShape::DrawingShape( Shape* _shape_p )
{
  shape_p = _shape_p;
  id = shape_p->get_id();
  set_colour( shape_p->get_rgb_colour() );
}

void
DrawingShape::set_colour(long int colour)
{
  if( colour != rgb_colour )
    {
      rgb_colour = colour;
      gdk_colour = make_gdk_colour( colour );
    }
}

// --------- DrawingLine -----------

DrawingLine::DrawingLine( Shape* s )
  : DrawingShape(s)
{
  last_drawn_start_point = ((Line*)s)->get_start_point();
  last_drawn_direction = ((Line*)s)->get_direction();
  last_drawn_length = ((Line*)s)->get_length();
  last_drawn_thickness = ((Line*)s)->get_thickness();
}

void
DrawingLine::draw_shape( bool erase )
{
  if( erase )
    the_gui.get_arenawindow_p()->draw_line( last_drawn_start_point,
                                            last_drawn_direction,
                                            last_drawn_length,
                                            last_drawn_thickness,
                                            *(the_gui.get_bg_gdk_colour_p()) );
  last_drawn_start_point = line_p->get_start_point();
  last_drawn_direction = line_p->get_direction();
  last_drawn_length = line_p->get_length();
  last_drawn_thickness = line_p->get_thickness();
  the_gui.get_arenawindow_p()->draw_line( line_p->get_start_point(),
                                          line_p->get_direction(),
                                          line_p->get_length(),
                                          line_p->get_thickness(),
                                          gdk_colour );
}

// --------- DrawingCircle -----------

DrawingCircle::DrawingCircle( Shape* s )
  : DrawingShape(s)
{
  last_drawn_center = ((Circle*)s)->get_center();
  last_drawn_radius = ((Circle*)s)->get_radius();
}

void
DrawingCircle::draw_shape(bool erase)
{
  if( erase )
    the_gui.get_arenawindow_p()->draw_circle( last_drawn_center,
                                              last_drawn_radius,
                                              *(the_gui.get_bg_gdk_colour_p()),
                                              true );
  last_drawn_center = circle_p->get_center();
  last_drawn_radius = circle_p->get_radius();
  the_gui.get_arenawindow_p()->draw_circle( circle_p->get_center(),
                                            circle_p->get_radius(),
                                            gdk_colour, true );

  // Draw radar and cannon

  if( typeid( *shape_p ) != typeid( Robot ))
    return;

  Robot* robot_p = (Robot*)shape_p;

  double scale = the_gui.get_arenawindow_p()->get_drawing_area_scale();

  Vector2D center = robot_p->get_center();
  double radius = robot_p->get_radius();
  rotation_t robot_angle = robot_p->get_robot_angle();
  rotation_t cannon_angle = robot_p->get_cannon_angle();
  rotation_t radar_angle = robot_p->get_radar_angle();

  if( radius*scale < 2.5 ) return;
  // Draw Cannon
  the_gui.get_arenawindow_p()->
    draw_line( center,
               angle2vec(cannon_angle.pos+robot_angle.pos),
               radius - the_opts.get_d(OPTION_SHOT_RADIUS) - 1.0 / scale,
               the_opts.get_d(OPTION_SHOT_RADIUS),
               *(the_gui.get_fg_gdk_colour_p()) );

  // Draw radar lines
  Vector2D radar_dir = angle2vec(radar_angle.pos+robot_angle.pos);
  the_gui.get_arenawindow_p()->
    draw_line( center - radius * 0.25 * radar_dir,
               rotate( radar_dir, M_PI / 4.0 ),
               radius / 1.5,
               radius / 20.0,
               *(the_gui.get_fg_gdk_colour_p()) );
  the_gui.get_arenawindow_p()->
    draw_line( center - radius * 0.25 * radar_dir,
               rotate( radar_dir, - (M_PI / 4.0) ),
               radius / 1.5,
               radius / 20.0,
               *(the_gui.get_fg_gdk_colour_p()) );

  // Draw robot angle line
  the_gui.get_arenawindow_p()->
    draw_line( center,
               angle2vec(robot_angle.pos),
               radius * 0.9 - 2.0 / scale,
               *(the_gui.get_fg_gdk_colour_p()) );
}

// --------- DrawingInnerCircle -----------

DrawingInnerCircle::DrawingInnerCircle( Shape* s )
  : DrawingShape(s)
{
  last_drawn_center = ((InnerCircle*)s)->get_center();
  last_drawn_radius = ((InnerCircle*)s)->get_radius();
}

void
DrawingInnerCircle::draw_shape(bool erase)
{
  if( erase )
    the_gui.get_arenawindow_p()->
      draw_arc( last_drawn_center,
                last_drawn_radius, last_drawn_radius*1.5,
                0.0, 2*M_PI,
                *(the_gui.get_bg_gdk_colour_p()) );

  last_drawn_center = icircle_p->get_center();
  last_drawn_radius = icircle_p->get_radius();

    the_gui.get_arenawindow_p()->
      draw_arc( icircle_p->get_center(), 
                icircle_p->get_radius(), 
                icircle_p->get_radius()*1.5,
                0.0, 2*M_PI, gdk_colour );
}

// --------- DrawingInnerArc -----------
DrawingArc::DrawingArc( Shape* s )
  : DrawingShape(s)
{
  last_drawn_center = ((Arc*)s)->get_center();
}
void
DrawingArc::draw_shape(bool erase)
{

  if( erase )
    the_gui.get_arenawindow_p()->draw_arc( last_drawn_center,
                                           arc_p->get_inner_radius(), 
                                           arc_p->get_outer_radius(),
                                           arc_p->get_start_angle(),
                                           arc_p->get_end_angle(),
                                           *(the_gui.get_bg_gdk_colour_p()) );
  last_drawn_center = arc_p->get_center();

  
  the_gui.get_arenawindow_p()->draw_arc( arc_p->get_center(),
                                         arc_p->get_inner_radius(), 
                                         arc_p->get_outer_radius(),
                                         arc_p->get_start_angle(),
                                         arc_p->get_end_angle(),
                                         gdk_colour );
}


// --------- DrawingRobot -----------

DrawingRobot::DrawingRobot( Robot* r )
{
  robot_p = r;
  reset_last_displayed();
  id = robot_p->get_id();
  set_colour( robot_p->get_rgb_colour() );
}

void
DrawingRobot::set_colour(long int colour)
{
  if( colour != rgb_colour )
    {
      rgb_colour = colour;
      gdk_colour = make_gdk_colour( colour );
    }
}


void
DrawingRobot::reset_last_displayed()
{
  last_displayed_energy = -1;
  last_displayed_place = 0;
  last_displayed_last_place = 0;
  last_displayed_score = -1;
}

void
DrawingRobot::display_score()
{
  int p;

  int energy = (int)(robot_p->get_energy());

  if( last_displayed_energy != energy )
    {
      last_displayed_energy = energy;
      char* energy_str = int2chars( energy );
      gtk_clist_set_text(GTK_CLIST(the_gui.get_scorewindow_p()->get_clist()),
                         row_in_score_clist, 2, energy_str);
      delete [] energy_str;
    }

  int position_this_game = robot_p->get_position_this_game();

  if( last_displayed_place != position_this_game )
    {
      char* str;
      if( position_this_game != 0 )
        str = int2chars(position_this_game);
      else
        {
          str = new char[1];
          str[0] = 0;
        }
      last_displayed_place = position_this_game;
      gtk_clist_set_text(GTK_CLIST(the_gui.get_scorewindow_p()->get_clist()),
                         row_in_score_clist, 3, str );
      delete [] str;
    }

  p = robot_p->get_last_position();
  if( p != 0 && p != last_displayed_last_place  )
    {
      last_displayed_last_place = p;
      char* str = int2chars( p );
      gtk_clist_set_text(GTK_CLIST(the_gui.get_scorewindow_p()->get_clist()),
                         row_in_score_clist, 4, str);
      delete [] str;
    }

  double pnts = robot_p->get_total_points();
  if( last_displayed_score != (int)(10 * pnts) )
    {
      last_displayed_score = (int)(10 * pnts);
      char* str = double2chars( pnts );
      gtk_clist_set_text(GTK_CLIST(the_gui.get_scorewindow_p()->get_clist()),
                         row_in_score_clist, 5, str);
      delete [] str;
    }
}

//TODO: Fix a way to use gdk_colour from DrawingCircle

void
DrawingRobot::get_score_pixmap( GdkWindow* win, GdkPixmap*& pixm, GdkBitmap*& bitm )
{
  score_pixmap.get_pixmap( gdk_colour, win, pixm, bitm ); 
}

void
DrawingRobot::get_stat_pixmap( GdkWindow* win, GdkPixmap*& pixm, GdkBitmap*& bitm )
{
  stat_pixmap.get_pixmap( gdk_colour, win, pixm, bitm ); 
}
