
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
  DrawingShape( Shape* _shape_p );
  virtual ~DrawingShape() {}

  virtual void draw_shape(bool erase) = 0;
  GdkColor& get_gdk_colour() { return gdk_colour; }
  void set_colour(const long int colour);

  int get_id() const { return id; }

  Shape* get_shape() { return shape_p; }

protected:
  long int rgb_colour;
  GdkColor gdk_colour;

  int id;
  Shape* shape_p;
};

// ---------------------------------------------------------------------------
// class DrawingLine
// ---------------------------------------------------------------------------

class DrawingLine : public DrawingShape
{
public:
  DrawingLine( Shape* s );
  ~DrawingLine() {}

  void draw_shape(bool erase);

protected:
  Vector2D last_drawn_start_point;
  Vector2D last_drawn_direction;
  double last_drawn_length;
  double last_drawn_thickness;
};

// ---------------------------------------------------------------------------
// class DrawingCircle
// ---------------------------------------------------------------------------

class DrawingCircle : public DrawingShape
{
public:
  DrawingCircle( Shape* s );
  ~DrawingCircle() {}

  void draw_shape(bool erase);

protected:
  Vector2D last_drawn_center;
  double last_drawn_radius;
};

// ---------------------------------------------------------------------------
// class DrawingInnerCircle
// ---------------------------------------------------------------------------

class DrawingInnerCircle : public DrawingShape
{
public:
  DrawingInnerCircle( Shape* s );
  ~DrawingInnerCircle() {}

  void draw_shape(bool erase);

protected:
  Vector2D last_drawn_center;
  double last_drawn_radius;
};

// ---------------------------------------------------------------------------
// class DrawingRobot
// ---------------------------------------------------------------------------

class DrawingRobot
{
public:
  DrawingRobot( Robot* );
  ~DrawingRobot() {}

  GdkColor& get_gdk_colour() { return gdk_colour; }
  void set_colour(const long int colour);

  int get_row_in_score_clist() { return row_in_score_clist; }
  void get_score_pixmap( GdkWindow* win, GdkPixmap*& pixm, GdkBitmap*& bitm );
  void get_stat_pixmap( GdkWindow* win, GdkPixmap*& pixm, GdkBitmap*& bitm );
  int get_id() const { return id; }
  Robot* get_robot_p() const { return robot_p; }

  void set_row_in_score_clist( int row ) { row_in_score_clist = row; }
  void reset_last_displayed();
  void display_score();

private:
  Robot* robot_p;

  long int rgb_colour;
  GdkColor gdk_colour;

  int row_in_score_clist;

  int last_displayed_energy;
  int last_displayed_place;
  int last_displayed_last_place;
  long last_displayed_score;

  pixmap_t score_pixmap;
  pixmap_t stat_pixmap;

  int id;
};

#endif __DRAWINGOBJECTS__
