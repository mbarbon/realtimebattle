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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <strstream>

#include "GeometryGadget.h"
#include "Variable.h"
#include "RobotBodyGadget.h"
#include <string>
#include "Vector2D.h"
#include "String.h"

GeometryGadget::GeometryGadget( const char* name, Gadget* const p ) 
  : Gadget(name, p, GAD_GEOMETRY)
{
}

GeometryGadget::~GeometryGadget()
{
  for(set<Wall*>::iterator i = walls.begin();
      i != walls.end(); i ++)
    delete *i;
}

Gadget* 
GeometryGadget::create_instance( const string & s, const Gadget* build_as )
{
    return NULL;
}

void
GeometryGadget::eval_function( const int fcn )
{
  
}


void 
GeometryGadget::set_scale( double val ) 
{
  scale = val;
}

void 
GeometryGadget::set_boundary( vector<string>& b ) 
{
  for(int i = 0; i < 4; i ++)
    {
      istrstream is(b[i].c_str());
      is >> boundary[i];
    }
}

void 
GeometryGadget::set_shape(vector<string>& the_shape, WallGadget* wg) 
{
  //cout<<"creating a shape looking like "<<the_shape[0]<<" with a "<<wg->get_info().name<<" wall\n"; 

  //Back to a full string
  //NOTE : this is very stupid !!!
  //TODO : implement pairs and lists in the parser !!!
  string to_strstream;
  for(unsigned int i = 2; i < the_shape.size(); i ++)
    to_strstream += (the_shape[i] + " ");
  istrstream args(to_strstream.c_str());

  //Create the shape
  Wall * new_wall;
  if(the_shape[0] == "Line") {
    Vector2D v1, v2; double len;
    args >> v1 >> v2 >> len;
    new_wall = new WallLine( v1, v2, len, wg );
    walls.insert( new_wall );
  } else if(the_shape[0] == "Circle") {
    Vector2D center; double radius;
    args >> center >> radius;
    walls.insert( new WallCircle( center, radius, wg ) );
  } else if(the_shape[0] == "InnerCircle") {
    Vector2D center; double radius;
    args >> center >> radius;
    walls.insert( new WallInnerCircle( center, radius, wg ) );
  } else if(the_shape[0] == "Arc") {
    Vector2D center; double r1, r2, a1, a2;
    args >> center >> r1 >> r2 >> a1 >> a2;
    walls.insert( new WallArc( center, r1, r2, a1, a2, wg ) ); 
  }
}
