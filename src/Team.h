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

#include <list>

#include "Robot.h"

class Robot;

class Team
{
public:
  Team();
  ~Team();
  
  int get_quantity();	// ??? Really needed ???
  
  void insert(const int target, const Robot *source);
            		// Which robot should be inserted in which team.
  void remove(const Robot *target);
                	// 
  void change(const int target, const Robot *source);

  void change(const Robot *source);
  
  void send(const int team_nr);

protected:
  list<Robot*> My_Team[9];
  int quantity;
  
};
