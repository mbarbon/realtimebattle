/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-1999  Erik Ouchterlony and Ragnar Ouchterlony

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

#ifndef __STRUCTS__
#define __STRUCTS__

#include "String.h"


enum game_mode_t { DEBUG_MODE, NORMAL_MODE, COMPETITION_MODE };

struct stat_t
{
  stat_t(int s, int g, int pos, double p, double  t, double tp) :
    sequence_nr(s), game_nr(g), position(pos), points(p), time_survived(t), total_points(tp) {}
  int sequence_nr;
  int game_nr;
  int position;
  double points;
  double time_survived;
  double total_points;
};

enum rotation_mode_t { NORMAL_ROT, SWEEP_LEFT, SWEEP_RIGHT, ROTATE_TO_RIGHT, ROTATE_TO_LEFT };

struct rotation_t
{
  rotation_t() : pos(0), vel(0), left(0), right(0), mode(NORMAL_ROT) {}
  rotation_t(const double p, const double v, const double l, const double r, const rotation_mode_t m) :
    pos(p), vel(v), left(l), right(r), mode(m) {}
  void set_rot(const double p, const double v, const double l, const double r, const rotation_mode_t m) 
    { pos=p; vel=v; left=l; right=r; mode=m; }
  double pos;
  double vel;
  double left;
  double right;
  rotation_mode_t mode;
};


enum state_t { NO_STATE, NOT_STARTED, STARTING_ROBOTS, GAME_IN_PROGRESS, 
               PAUSING_BETWEEN_GAMES, PAUSED, BEFORE_GAME_START,
               SHUTTING_DOWN_ROBOTS, FINISHED, EXITING };

struct start_tournament_info_t
{
  start_tournament_info_t(const int r, const bool s,
                          const class String fn, const class String dir) :
    row(r), selected(s), filename(fn), directory(dir) {}
  int row;
  bool selected;
  class String filename;
  class String directory;
};


#endif __STRUCTS__
