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

#ifndef __GADGET_SET__
#define __GADGET_SET__

#include <string>
#include <set>

enum gadget_t
{
  GAD_NONE = -1,

  GAD_WEAPON,
  GAD_SHOT,
  GAD_EXPLOSION,
  GAD_DEFENSE,
  GAD_ENGINE,
  GAD_SENSOR,
  GAD_COMMUNICATION,
  GAD_WALL,
  GAD_BALL,
  GAD_EQUIPMENT,
  GAD_ENVIRONMENT,
  GAD_GEOMETRY,
  GAD_ROBOTBODY,
  GAD_SCORING,

  GAD_VARIABLE,
  GAD_FUNCTION,
  GAD_SCRIPT,

  LAST_GADGET
};

static const char gadget_types[][LAST_GADGET] =
{
  "Weapon",
  "Shot",
  "Explosion",
  "Defense",
  "Engine",
  "Sensor",
  "Communication",
  "Wall",
  "Ball",
  "Equipment",
  "Environment",
  "Geometry",
  "RobotBody",
  "Scoring",

  "Variable",
  "Function",
  "Script"
};

class Gadget;


struct GadgetInfo
{

  GadgetInfo(Gadget* const gp, long int id_nr, const char* nm, gadget_t tp )
    : gadgetp(gp), id(id_nr), name(nm), type(tp) {}
 
  GadgetInfo(const string& str ) 
    : gadgetp(NULL), id(0), name(str), type(GAD_NONE) {}

  ~GadgetInfo() {}

  friend bool operator<(const GadgetInfo& a, const GadgetInfo& b) 
    { return a.name < b.name; }
  friend bool operator==(const GadgetInfo& a, const GadgetInfo& b)
  {
    return a.name == b.name;
  }

  Gadget* gadgetp;
  long int id;
  string name;
  gadget_t type;
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


  Gadget* find_by_name( const GadgetInfo& gi) const;
  Gadget* find_by_name( const string& str) const;
  
  friend class Gadget;

  //protected:

  set<GadgetInfo> the_set;
};


#endif // __GADGET_SET__
