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

#include <string>
#include <algorithm>

#include <assert.h>


#include "Team.h"
#include "Robot.h"

void Team::insert(Robot* robot)
{
  // The robot should not be in the list already.
  assert( find(the_robots.begin(), the_robots.end(), robot) 
          == the_robots.end() );  

  the_robots.push_back(robot);

  size = the_robots.size();
}

void Team::remove(Robot* robot)
{
  the_robots.remove(robot);
  size = the_robots.size();
}


void Team::broadcast(const string message, const Robot& from)
{
}
