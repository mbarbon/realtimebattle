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

#include "Team.h"

Team::Team()
{
  for (int i = 0; i < 10; i++)
    My_Team[i].empty();
}

Team::~Team()
{
  for (int i = 0; i < 10; i++)
    My_Team[i].clear();
}

void Team::insert(const int target, const Robot *source)
{
  My_Team[target - 1].insert(My_Team[target - 1].begin(), source);
  source -> set_team_nr( target );
}

void Team::remove(const Robot *target)
{
  My_Team[target -> get_team_nr()].remove(target);
  target -> set_team_nr( 0 );
}

void Team::change(const int target, const Robot *source)
{
  My_Team[source -> get_team_nr()].remove(source);
  insert(target, source);
}

void Team::change(const Robot *source)
{
//  My_Team[source -> get_team_nr()].remove(source);
//  insert(target, source);
}

void Team::send(const int team_nr)
{
}
