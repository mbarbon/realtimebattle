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

#ifndef __SHAPE__
#define __SHAPE__

class Vector2D;
class SubSquare;


/*

  Shape is the base class for all objects in the arena. The class hierarchy
  is as follows:

                  ---------
                  | Shape | 
                  ---------
            _____/    |     \_____________
           /          |                   \
   --------       ----------               ---------------
   | Line |       | Circle |               | InnerCircle |
   --------       ----------               ---------------
      |            |  |    \                       \
      |            |  |     \                       \
 ------------      |  |     --------------       -------------------
 | WallLine |     /   |     | WallCircle |       | WallInnerCircle |
 ------------    /     \    --------------       -------------------
                /       \
       ----------        ----------------
       | Extras |        | MovingObject |
       ----------        ----------------
       /      \             |        \      
      /        \            |         \     
 ----------  --------    --------  ---------
 | Cookie |  | Mine |    | Shot |  | Robot |
 ----------  --------    --------  ---------

 */







class Shape
{
public:
  Shape();
  Shape(int long colour);

  virtual ~Shape() {}

  virtual double get_distance(const Vector2D& pos, const Vector2D& vel, 
                              const double size) const = 0;
  virtual bool within_distance(const Vector2D& pos, const double size) = 0;
  virtual Vector2D get_normal(const Vector2D& pos) = 0;

  virtual void find_subsquares() const = 0 ;
  virtual bool is_inside_subsquare( const SubSquare& ss ) const = 0;
  bool is_still_inside_subsquare( const SubSquare& ss ) const { return !killed; }


  bool is_killed() const { return killed; }

  // Returns true if object if killed by the collision
  //  virtual bool collided( const Shape* colliding_object ) = 0;


  int get_id() { return id; }
  long int get_rgb_colour() { return rgb_colour; }

  double get_bounce_coeff( const double angle=0 ) { return bounce_coeff; }
  double get_hardness_coeff( const double angle=0 ) { return hardness_coeff; }

  //  virtual arenaobject_t get_arenaobject_t() = 0;

  void set_colour(const long int colour);

protected:
  long int rgb_colour;

  double bounce_coeff;
  double hardness_coeff;
  int id;

  bool killed;
};

#endif __SHAPE__
