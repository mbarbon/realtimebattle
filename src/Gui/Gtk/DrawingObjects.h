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

#ifndef __DRAWINGOBJECTS__
#define __DRAWINGOBJECTS__

#include <gdk/gdktypes.h>
#include <list>
#include <string>

#include "Vector2D.h"
#include "GuiStructs.h"
#include "Structs.h"

struct _GtkCList;
typedef struct _GtkCList GtkCList;

struct object_pos_info;
class Shape;
class Robot;
class ArenaDisplay;

// ---------------------------------------------------------------------------
// class DrawingShape
// ---------------------------------------------------------------------------

class DrawingShape
{
public:
  DrawingShape             ( const int _id, const long int rgb_col );
  virtual ~DrawingShape    () {}

  virtual void draw_shape  ( ArenaDisplay* display_p ) = 0;
  virtual void set_position( const object_pos_info* pos ) {}

  void set_colour          ( const long int colour );
  long int get_rgb_colour  () const { return rgb_colour; }
  const GdkColor& get_gdk_colour () const { return gdk_colour; }
  int get_id               () const { return id; }

protected:
  long int rgb_colour;
  GdkColor gdk_colour;

  int id;
  bool moveable;
};

// ---------------------------------------------------------------------------
// class DrawingLine
// ---------------------------------------------------------------------------

class DrawingLine : public DrawingShape
{
public:
  DrawingLine              ( const int _id, const long int rgb_col );
  ~DrawingLine             () {}

  void draw_shape          ( ArenaDisplay* display_p );
  void set_position        ( const object_pos_info* pos );
  void set_position        ( const Vector2D& sp, const Vector2D& dir,
                             const double len, const double th );

protected:
  Vector2D start_point;
  Vector2D direction;
  double length;
  double thickness;
};

// ---------------------------------------------------------------------------
// class DrawingCircle
// ---------------------------------------------------------------------------

class DrawingCircle : public DrawingShape
{
public:
  DrawingCircle            ( const int _id, const long int rgb_col );
  ~DrawingCircle           () {}

  virtual void draw_shape  ( ArenaDisplay* display_p );
  void set_position        ( const object_pos_info* pos );
  void set_position        ( const Vector2D& c, const double r );

protected:
  Vector2D center;
  double radius;
};

// ---------------------------------------------------------------------------
// class DrawingInnerCircle
// ---------------------------------------------------------------------------

class DrawingInnerCircle : public DrawingShape
{
public:
  DrawingInnerCircle       ( const int _id, const long int rgb_col );
  ~DrawingInnerCircle      () {}

  virtual void draw_shape  ( ArenaDisplay* display_p );
  void set_position        ( const object_pos_info* pos );
  void set_position        ( const Vector2D& c, const double r );

protected:
  Vector2D center;
  double radius;
};



// ---------------------------------------------------------------------------
// class DrawingArc
// ---------------------------------------------------------------------------

class DrawingArc : public DrawingShape
{
public:
  DrawingArc               ( const int _id, const long int rgb_col );
  ~DrawingArc              () {}

  virtual void draw_shape  ( ArenaDisplay* display_p );
  void set_position        ( const object_pos_info* pos );
  void set_position        ( const Vector2D& c, const double ir, const double or,
                             const double sa, const double ea );

protected:
  Vector2D center;
  double inner_radius;
  double outer_radius;
  double start_angle;
  double end_angle;
};

// ---------------------------------------------------------------------------
// class DrawingWeaponGadget
// ---------------------------------------------------------------------------

class DrawingWeaponGadget : public DrawingShape
{
public:
  DrawingWeaponGadget         ( const int _id, const long int rgb_col );
  ~DrawingWeaponGadget        () {}

  virtual void draw_shape     ( ArenaDisplay* display_p );
  void draw_shape             ( ArenaDisplay* display_p, const Vector2D& center,
                                const double radius, const double robot_angle );
  void set_angle              ( const double ang ) { angle = ang; }

protected:
  double angle;
};

// ---------------------------------------------------------------------------
// class DrawingSensorGadget
// ---------------------------------------------------------------------------

class DrawingSensorGadget : public DrawingShape
{
public:
  DrawingSensorGadget         ( const int _id, const long int rgb_col );
  ~DrawingSensorGadget        () {}

  virtual void draw_shape     ( ArenaDisplay* display_p );
  void draw_shape             ( ArenaDisplay* display_p, const Vector2D& center,
                                const double radius, const double robot_angle );
  void set_angle              ( const double ang ) { angle = ang; }

protected:
  double angle;
};

// ---------------------------------------------------------------------------
// class DrawingRobot
// ---------------------------------------------------------------------------

class DrawingRobot : public DrawingCircle
{
public:
  DrawingRobot                ( const int _id, const long int rgb_col );
  ~DrawingRobot               () {}

  virtual void draw_shape     ( ArenaDisplay* display_p );

  const string& get_name      () const { return name; }
  int get_row_in_score_clist  () const { return row_in_score_clist; }
  void get_score_pixmap       ( GdkWindow* win, GdkPixmap*& pixm, GdkBitmap*& bitm );
  void get_stat_pixmap        ( GdkWindow* win, GdkPixmap*& pixm, GdkBitmap*& bitm );

  void set_row_in_score_clist ( const int row ) { row_in_score_clist = row; }
  void reset_last_displayed   ();
  void display_score          ( GtkCList* clist );

  void set_robot_angle        ( const double ang )  { robot_angle        = ang; }
  void set_energy             ( const int en )      { energy             = en;  }
  void set_position_this_game ( const int pos )     { position_this_game = pos; }
  void set_last_place         ( const int pos )     { last_place         = pos; }
  void set_score              ( const long int sc ) { score              = sc;  }

private:
  string name;

  int row_in_score_clist;

  double robot_angle;

  list<DrawingWeaponGadget> weapon_list;
  list<DrawingSensorGadget> sensor_list;

  int energy;
  int position_this_game;
  int last_place;
  long int score;

  int last_displayed_energy;
  int last_displayed_place;
  int last_displayed_last_place;
  long int last_displayed_score;

  list<stat_t> statistics;

  pixmap_t score_pixmap;
  pixmap_t stat_pixmap;
};

#endif __DRAWINGOBJECTS__
