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

#ifndef __GADGET_SET__
#define __GADGET_SET__

#include <string>
#include <set>

class Gadget;


enum gadget_type
{
  NO_GDT = -1,

  VARIABLE_GDT,
  FUNCTION_GDT,
  SCRIPT_GDT,

  WALL_GDT,
  BALL_GDT,

  GEOMETRY_GDT,
  ENVIRONMENT_GDT,
  SCORING_GDT,

  EXPLOSION_GDT,
  WEAPON_GDT,
  SHOT_GDT,
  
  DEFENSE_GDT,
  ENGINE_GDT,
  SENSOR_GDT,
  COMMUNICATION_GDT,
  EQUIPMENT_GDT,
  ROBOTBODY_GDT,

  LAST_GDT
};


struct GadgetInfo
{
  GadgetInfo(Gadget* const gp, long int id_nr, const char* nm, gadget_type tp ) 
    : gadgetp(gp), id(id_nr), name(nm), type(tp) {}

  GadgetInfo(const string& str ) : gadgetp(NULL), id(0), name(str), type(NO_GDT) {}

  ~GadgetInfo() {}

  friend bool operator<(const GadgetInfo& a, const GadgetInfo& b) 
    { return a.name < b.name; }

  Gadget* gadgetp;
  long int id;
  string name;
  gadget_type type;
};
  

// Stores GadgetInfos in a set, so that they can be found by name. 

// Used by Gadgets to find subgadgets.


typedef set<GadgetInfo>::iterator set_iterator;

class GadgetSet
{
public:

  GadgetSet() {}
  ~GadgetSet() {}

  void add(const GadgetInfo& gi);
  void remove(const GadgetInfo& gi);


  set_iterator find_by_name( const GadgetInfo& gi) const;
  set_iterator find_by_name( const string& str) const;
  
  friend class Gadget;

protected:

  set<GadgetInfo> the_set;
};


#endif __GADGET_SET__
