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

#ifndef __ARENA__
#define __ARENA__

#include <string>
#include <set>

#include "Event.h"
#include "Shot.h"
#include "Robot.h"
#include "Wall.h"

#include "SubSquare.h"

class Vector2D;
class Shape;
class Circle;
class Gadget;
class Shot;
class Robot;

// The Arena class is storing all geometric information of the current 
// arena. It can determine the closest object and find objects in given 
// directions and sectors.

class Arena
{
public:

  Arena() {}
  ~Arena() {}

  void load_arena_file( const string& filename, Gadget& hierarchy );

  void setup_at_roundstart();

  void add_shot(const Shot& s);
  void update_shots();

  void collision( Shape* obj1, Shape* obj2, const double time_to_collsion ) {}

  set<Robot*>* get_robot_set() { return &robots; }


  void print_to_logfile( const char first_letter ... ) {}



  double get_shortest_distance(const Vector2D& pos, const Vector2D& vel, 
                               const double size, enum object_type& closest_shape, 
                               Shape*& colliding_object, 
                               const class Robot* the_robot = NULL ) { return 0.0; }

  void find_closest_object( const Vector2D& point, const Vector2D& direction, 
                            Shape* closest_shape, double distance );
  
  void find_objects_in_sector( const Vector2D& point, const Vector2D& left_direction, 
                               const Vector2D& right_direction,
                               Shape** shapes_found);
  
  double get_heat_in_direction( const Vector2D& point, const Vector2D& direction );



  void find_subsquares_for_circle( const Circle& c);
  void find_subsquares_for_small_circle( const Circle& c);

// Slow but general algorithm used by static objects
  void find_subsquares_for_shape( const Shape& s); 

protected:

  set<Shot> shots;  
  set<Robot*> robots;


  SubSquare** subsquares;  // 2-dim array of SubSquares

  
  // number of subrectangles in each direction
  int number_of_subsquares_x;
  int number_of_subsquares_y;

  Vector2D top_left;
  Vector2D bottom_right;
  
  Vector2D size;  // bottom_right - top_left
  Vector2D subsquare_size; // size / number_of_subsquares 
};






class ShotUpdateEvent : public Event
{
public:
  ShotUpdateEvent( const double time, Arena* ar) 
    : Event(time), my_arena(ar) {}

  void eval() const { my_arena->update_shots(); }

protected:
  Arena* my_arena;
};


#endif __ARENA__
