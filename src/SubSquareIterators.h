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

#ifndef __SUBSQUARE_ITERATOS__
#define __SUBSQUARE_ITERATOS__

class Vector2D;

enum direction_t { RIGHT, DOWN, LEFT, UP };


//-------------------------------------------------------------------
//   SubSquareLineIterator
//-------------------------------------------------------------------

class SubSquareLineIterator
{
public:
  SubSquareLineIterator() : is_ok(false) {}
  SubSquareLineIterator(const Vector2D& pos, const Vector2D& vel);
  ~SubSquareLineIterator() {}

  void begin(const Vector2D& pos, const Vector2D& vel);
  

  // Moves to the next subsquare
  SubSquareLineIterator& operator++ (int); // postfix increment

  // Moves to the next subsquare in the main direction
  void move_one(); 



  int x() { assert( is_ok ); return xx; }
  int y() { assert( is_ok ); return yy; }

  bool ok() { return is_ok; }

  direction_t get_start_dir_for_sector();

protected:
  
  // Current subsquare coordinates
  int xx;
  int yy;


  // The side-direction coordinate. When s >= 1.0 move one subsquare
  // in the side-direction. 
  double s;


  // How much added to the side-direction when moving one
  // subsquare in the main direction
  double d;


  // The main direction and the 'side'-direction. This is
  // used to find the direction of the next subsquare.

  int main_x, main_y;
  int side_x, side_y;


  // After moved in the side direction, this variable will be true
  bool next_move_in_main_direction_without_check;

  // Whether the iterator is activated and pointing to a subsquare
  bool is_ok;
};



//-------------------------------------------------------------------
//   SubSquareSectorIterator
//-------------------------------------------------------------------

class SubSquareSectorIterator
{
public:
  SubSquareSectorIterator(const Vector2D& pos, 
                          const double left_angle, 
                          const double right_angle );
  ~SubSquareSectorIterator() {}

  SubSquareSectorIterator& operator++ (int); // postfix increment

  int x() { assert( is_ok ); return xx; }
  int y() { assert( is_ok ); return yy; }

  bool ok() { return is_ok; }

protected:
  
  // Current subsquare coordinates
  int xx;
  int yy;

  int center_x;
  int center_y;

  SubSquareLineIterator left_line;
  SubSquareLineIterator right_line;

  // We are examining subsquare with distance 'distance' from center
  int distance;

  // Moving in direction 'dir'
  direction_t dir;


  // Whether the iterator is activated and pointing to a subsquare
  bool is_ok;
};




//-------------------------------------------------------------------
//   SubSquareCircleIterator
//-------------------------------------------------------------------

class SubSquareCircleIterator
{
public:
  SubSquareCircleIterator(const Vector2D& pos);
  ~SubSquareCircleIterator() {}

  SubSquareCircleIterator& operator++ (int); // postfix increment

  int x() { assert( is_ok ); return xx; }
  int y() { assert( is_ok ); return yy; }

  bool ok() { return is_ok; }

protected:
  
  // Current subsquare coordinates
  int xx;
  int yy;

  int center_x;
  int center_y;

  // We are examining subsquares with distance 'distance' from center
  int distance;

  // Moving in direction 'dir'
  direction_t dir;


  // Whether the iterator is activated and pointing to a subsquare
  bool is_ok;
};

#endif __SUBSQUARE_ITERATOS__
