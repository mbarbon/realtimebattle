
#ifndef __DRAWINGOBJECTS__
#define __DRAWINGOBJECTS__

#include <gdk/gdktypes.h>

// structure of Drawingshape classes:
//
//             ----------------
//        -----| DrawingShape |-----------
//        |    ----------------          |
//        |            |                 |
//        |            |                 |
//  -------------  -----------------     ----------------------
//  |DrawingLine|  | DrawingCircle |     | DrawingInnerCircle |
//  -------------  -----------------     ----------------------
//

// DrawingRobot is separate

#include "Vector2D.h"
#include "GuiStructs.h"

class Shape;
class Robot;

// ---------------------------------------------------------------------------
// class DrawingShape
// ---------------------------------------------------------------------------

class DrawingShape
{
public:
  DrawingShape             ( const int _id, const long int rgb_col );
  virtual ~DrawingShape    () {}

  virtual void draw_shape  ( bool erase ) = 0;

  void set_colour          ( const long int colour );
  GdkColor& get_gdk_colour () { return gdk_colour; }
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
// Note that lines should always be at the same position!
// ---------------------------------------------------------------------------

class DrawingLine : public DrawingShape
{
public:
  DrawingLine              ( const int _id, const long int rgb_col );
  ~DrawingLine             () {}

  void draw_shape          ( bool erase );

protected:
  Vector2D start_point;
  Vector2D direction;
  double length;
  double thickness;
};

// ---------------------------------------------------------------------------
// class DrawingCircle
// ---------------------------------------------------------------------------
// Note that the radius may change (ex. explosions)
// ---------------------------------------------------------------------------

class DrawingCircle : public DrawingShape
{
public:
  DrawingCircle            ( const int _id, const long int rgb_col );
  ~DrawingCircle           () {}

  void draw_shape          ( bool erase );

protected:
  Vector2D center;
  double radius;

  Vector2D last_drawn_center;
  double last_drawn_radius;
};

// ---------------------------------------------------------------------------
// class DrawingInnerCircle
// ---------------------------------------------------------------------------

class DrawingInnerCircle : public DrawingShape
{
public:
  DrawingInnerCircle       ( const int _id, const long int rgb_col );
  ~DrawingInnerCircle      () {}

  void draw_shape          ( bool erase );

protected:
  Vector2D center;
  double radius;

  Vector2D last_drawn_center;
  double last_drawn_radius;
};



// ---------------------------------------------------------------------------
// class DrawingArc
// ---------------------------------------------------------------------------

class DrawingArc : public DrawingShape
{
public:
  DrawingArc               ( const int _id, const long int rgb_col );
  ~DrawingArc              () {}

  void draw_shape          ( bool erase );

protected:
  Vector2D center;
  double inner_radius;
  double outer_radius;
  double start_angle;
  double end_angle;

  Vector2D last_drawn_center;
};


// ---------------------------------------------------------------------------
// class DrawingRobot
// ---------------------------------------------------------------------------

class DrawingRobot : public DrawingCircle
{
public:
  DrawingRobot                ( const int _id, const long int rgb_col );
  ~DrawingRobot               () {}

  void draw_shape             ( bool erase );

  int get_row_in_score_clist  () const { return row_in_score_clist; }
  void get_score_pixmap       ( GdkWindow* win, GdkPixmap*& pixm, GdkBitmap*& bitm );
  void get_stat_pixmap        ( GdkWindow* win, GdkPixmap*& pixm, GdkBitmap*& bitm );

  void set_row_in_score_clist ( int row ) { row_in_score_clist = row; }
  void reset_last_displayed   ();
  void display_score          ();

private:

  int row_in_score_clist;

  double robot_angle;
  double cannon_angle;
  double radar_angle;

  int energy;
  int position_this_game;
  int last_place;
  long int score;

  int last_displayed_energy;
  int last_displayed_place;
  int last_displayed_last_place;
  long int last_displayed_score;

  pixmap_t score_pixmap;
  pixmap_t stat_pixmap;
};

#endif __DRAWINGOBJECTS__
