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

#ifndef __GEOMETRY_GADGET__
#define __GEOMETRY_GADGET__

#include <limits.h>
#include <float.h>
#include <vector>

#include "Gadget.h"
#include "Variable.h"
#include "Function.h"
#include "Arena.h"
#include "EventHandler.h"

  class WallGadget;

class GeometryGadget : public Gadget
{
public:

  GeometryGadget( const char* name, Gadget* const p );

  ~GeometryGadget();

  static Gadget* new_geometrygadget( const char* name, Gadget* const p )
  {
    return new GeometryGadget( name, p );
  }

  Gadget* create_instance( const string & s, const Gadget*  );

  void set_scale( double );
  void set_boundary( vector<string>& );
  void set_shape(vector<string>&, WallGadget*);

private:
  set<Wall*> walls;

  double scale;
  double boundary[4];

  void eval_function(const int fcn);
};


#endif // __GEOMETRY_GADGET__
