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

#ifndef __ARENA__
#define __ARENA__

#include <string>
#include <vector>
#include <set>

#include "Event.h"
#include "Shot.h"
#include "Robot.h"
#include "Wall.h"

#include "SubSquare.h"

class Arena;
extern Arena* the_arenap;
#define the_arena (*the_arenap)

enum gadget_t;

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

  Arena( const string& filname, set<Robot*>& rbts );
  ~Arena() {garbage_collector();}

  void load_arena_file( const string& filename, Gadget& hierarchy );


  void add_shot(Shot* s, bool from_robot=true);
  void update_shots() {}
  void update_robots();

  void collision( Shape* obj1, Shape* obj2, const double time_to_collsion ) {}

  set<Robot*>* get_robot_set() { return &robots; }


  void print_to_logfile( const char first_letter ... ) {}



  double get_shortest_distance(const Vector2D& pos, const Vector2D& vel, 
                               const double size, enum object_type& closest_shape, 
                               Shape*& colliding_object, 
                               const class Shape* from_shape = NULL );

  void find_closest_object( const Vector2D& point, const Vector2D& direction, 
                            Shape* closest_shape, double distance );
  
  void find_objects_in_sector( const Vector2D& point, const Vector2D& left_direction, 
                               const Vector2D& right_direction,
                               Shape** shapes_found);
  
  double get_heat_in_direction( const Vector2D& point, const Vector2D& direction );



  void find_subsquares_for_circle( const Circle& c);
  void find_subsquares_for_small_circle( const Circle& c);

// Slow but general algorithm used by static objects
  void find_subsquares_for_shape( Shape* s); 


  SubSquare* get_subsquare_at( const Vector2D& pos ) const;
  void get_subsquare_at( const Vector2D& pos, int& x, int& y ) const;

  const Vector2D& get_subsquare_size() const { return subsquare_size; }
  int get_number_of_subsquares_x() { return number_of_subsquares_x; }
  int get_number_of_subsquares_y() { return number_of_subsquares_y; }

  // cleans subsquares and removes killed objects from lists.
  void garbage_collector();

protected:

  set<Shot*> shots;  
  set<Robot*> robots;

  set<MovingObject*> moving_objects; // Balls
  set<Shape*> updatable_objects;  // Explosion, etc.
  set<Shape*> static_objects; // Walls, mines, etc.


  Gadget gadget_hierarchy;

  SubSquare** subsquares;  // 2-dim array of SubSquares

  
  // number of subrectangles in each direction
  int number_of_subsquares_x;
  int number_of_subsquares_y;

  Vector2D top_left;
  Vector2D bottom_right;
  
  Vector2D size;  // bottom_right - top_left
  Vector2D subsquare_size; // size / number_of_subsquares 

private:
  // load_arena_file() internals
  const bool find_full_arena_filename ( string& filename,
                                        const string& top_file_path = "",
                                        const bool included = false ) const;
  const bool sufficient_arena_version ( vector<string>& wordlist ) const;
};



#endif // __ARENA__
