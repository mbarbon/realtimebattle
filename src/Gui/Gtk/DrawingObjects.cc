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
#include <list>

#include "DrawingObjects.h"
#include "Vector2D.h"
#include "GuiVarious.h"
#include "OptionHandler.h"
#include "Gui.h"
#include "GuiStructs.h"
#include "ArenaWindow.h"
#include "ScoreWindow.h"
#include "String.h"

extern class Gui* gui_p;

// --------- DrawingShape -----------

DrawingShape::DrawingShape( const int _id, const long int rgb_col )
{
  id = _id;
  set_colour( rgb_col );
}

void
DrawingShape::set_colour( long int colour )
{
  if( colour != rgb_colour )
    {
      rgb_colour = colour;
      gdk_colour = make_gdk_colour( colour );
    }
}

// --------- DrawingLine -----------

DrawingLine::DrawingLine( const int _id, const long int rgb_col )
  : DrawingShape( _id, rgb_col )
{
  start_point = Vector2D( 0.0, 0.0 );
  direction   = Vector2D( 0.0, 0.0 );
  length      = 0.0;
  thickness   = 0.0;
}

void
DrawingLine::draw_shape( const bool erase )
{
  the_gui.get_arenawindow_p()->draw_line( start_point, direction,
                                          length, thickness,
                                          gdk_colour );
}

void
DrawingLine::set_position( const Vector2D& sp, const Vector2D& dir,
                           const double len, const double th )
{
  start_point = sp;
  direction = dir;
  length = len;
  thickness = th;
}

// --------- DrawingCircle -----------

DrawingCircle::DrawingCircle( const int _id, const long int rgb_col )
  : DrawingShape( _id, rgb_col )
{
  center = last_drawn_center = Vector2D( 0.0, 0.0 );
  radius = last_drawn_radius = 0.0;
}

void
DrawingCircle::draw_shape( const bool erase )
{
  if( erase )
    the_gui.get_arenawindow_p()->draw_circle( last_drawn_center,
                                              last_drawn_radius,
                                              *(the_gui.get_bg_gdk_colour_p()),
                                              true );
  last_drawn_center = center;
  last_drawn_radius = radius;
  the_gui.get_arenawindow_p()->draw_circle( center, radius,
                                            gdk_colour, true );
}

void
DrawingCircle::set_position( const Vector2D& c, const double r )
{
  center = c;
  radius = r;
}

// --------- DrawingInnerCircle -----------

DrawingInnerCircle::DrawingInnerCircle( const int _id, const long int rgb_col )
  : DrawingShape( _id, rgb_col )
{
  center = last_drawn_center = Vector2D( 0.0, 0.0 );
  radius = last_drawn_radius = 0.0;
}

void
DrawingInnerCircle::draw_shape( const bool erase )
{
  if( erase )
    the_gui.get_arenawindow_p()->draw_arc( last_drawn_center,
                                           last_drawn_radius,
                                           last_drawn_radius*1.5,
                                           0.0, 2*M_PI,
                                           *(the_gui.get_bg_gdk_colour_p()) );

  last_drawn_center = center;
  last_drawn_radius = radius;

  the_gui.get_arenawindow_p()->draw_arc( center, radius, radius*1.5,
                                         0.0, 2*M_PI, gdk_colour );
}

void
DrawingInnerCircle::set_position( const Vector2D& c, const double r )
{
  center = c;
  radius = r;
}

// --------- DrawingArc -----------

DrawingArc::DrawingArc( const int _id, const long int rgb_col )
  : DrawingShape( _id, rgb_col )
{
  center = last_drawn_center = Vector2D( 0.0, 0.0 );
}
void
DrawingArc::draw_shape(const bool erase)
{
  if( erase )
    the_gui.get_arenawindow_p()->draw_arc( last_drawn_center,
                                           inner_radius, outer_radius,
                                           start_angle, end_angle,
                                           *(the_gui.get_bg_gdk_colour_p()) );

  last_drawn_center = center;

  the_gui.get_arenawindow_p()->draw_arc( center, inner_radius, outer_radius,
                                         start_angle, end_angle, gdk_colour );
}

void
DrawingArc::set_position( const Vector2D& c, const double ir, const double or,
                          const double sa, const double ea )
{
  center = c;
  inner_radius = ir;
  outer_radius = or;
  start_angle = sa;
  end_angle = ea;
}

// --------- DrawingWeaponGadget -----------

DrawingWeaponGadget::DrawingWeaponGadget( const int _id, const long int rgb_col )
  : DrawingShape( _id, rgb_col )
{
  angle = 0.0;
}

// This function should not be called!
void
DrawingWeaponGadget::draw_shape( const bool erase )
{
  draw_shape( erase, Vector2D( 0.0, 0.0 ), 0.0, 0.0 );
}

void
DrawingWeaponGadget::draw_shape( const bool erase, const Vector2D& center,
                                 const double radius, const double robot_angle )
{
  double scale = the_gui.get_arenawindow_p()->get_drawing_area_scale();

 the_gui.get_arenawindow_p()->
    draw_line( center,
               angle2vec( angle + robot_angle ),
               radius - the_opts.get_d(OPTION_SHOT_RADIUS) - 1.0 / scale,
               the_opts.get_d(OPTION_SHOT_RADIUS),
               *(the_gui.get_fg_gdk_colour_p()) );
}

// --------- DrawingSensorGadget -----------

DrawingSensorGadget::DrawingSensorGadget( const int _id, const long int rgb_col )
  : DrawingShape( _id, rgb_col )
{
  angle = 0.0;
}

// This function should not be called!
void
DrawingSensorGadget::draw_shape( const bool erase )
{
  draw_shape( erase, Vector2D( 0.0, 0.0 ), 0.0, 0.0 );
}

void
DrawingSensorGadget::draw_shape( const bool erase, const Vector2D& center,
                                 const double radius, const double robot_angle )
{
  Vector2D radar_dir = angle2vec( angle + robot_angle );
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
}

// --------- DrawingRobot -----------

DrawingRobot::DrawingRobot( const int _id, const long int rgb_col )
  : DrawingCircle( _id, rgb_col )
{
  reset_last_displayed();

  robot_angle  = 0.0;

  energy             = 0;
  position_this_game = 1;
  last_place         = 1;
  score              = 0;
}

void
DrawingRobot::draw_shape( const bool erase )
{
  DrawingCircle::draw_shape( erase );
  // Draw radar and cannon

  double scale = the_gui.get_arenawindow_p()->get_drawing_area_scale();

  if( radius*scale < 2.5 ) return;

  list<DrawingWeaponGadget>::iterator dwg_i;
  for( dwg_i = weapon_list.begin(); dwg_i != weapon_list.end(); dwg_i++ )
    dwg_i->draw_shape( erase, center, radius, robot_angle );

  list<DrawingSensorGadget>::iterator dsg_i;
  for( dsg_i = sensor_list.begin(); dsg_i != sensor_list.end(); dsg_i++ )
    dsg_i->draw_shape( erase, center, radius, robot_angle );

  // Draw robot angle line
  the_gui.get_arenawindow_p()->
    draw_line( center,
               angle2vec( robot_angle ),
               radius * 0.9 - 2.0 / scale,
               *(the_gui.get_fg_gdk_colour_p()) );
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
  if( last_displayed_energy != energy )
    {
      last_displayed_energy = energy;
      char* energy_str = int2chars( energy );
      gtk_clist_set_text(GTK_CLIST(the_gui.get_scorewindow_p()->get_clist()),
                         row_in_score_clist, 2, energy_str);
      delete [] energy_str;
    }

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

  if( last_place != 0 && last_place != last_displayed_last_place  )
    {
      last_displayed_last_place = last_place;
      char* str = int2chars( last_place );
      gtk_clist_set_text(GTK_CLIST(the_gui.get_scorewindow_p()->get_clist()),
                         row_in_score_clist, 4, str);
      delete [] str;
    }

  if( last_displayed_score != score )
    {
      last_displayed_score = score;
      char* str = double2chars( score );
      gtk_clist_set_text(GTK_CLIST(the_gui.get_scorewindow_p()->get_clist()),
                         row_in_score_clist, 5, str);
      delete [] str;
    }
}

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
