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
#include "String.h"

GeometryGadget::GeometryGadget( const char* name, Gadget* const p ) 
  : Gadget(name, p, GAD_GEOMETRY)
{
}

Gadget* 
GeometryGadget::create_instance( const string & s, const Gadget* build_as )
{
  /*  cout<<s<<endl;
  if( equal_strings_nocase( s, "Shot") )
    {
      //TODO : dynamic_cast<ShotGadget*> (build_as);
      cout<<"Here\n";
      ShotGadget* build_as_shot = (ShotGadget*) build_as;
      shot  = new ShotGadget( s.c_str(), this );
      (*shot) = *build_as_shot;
      return shot;
    }
    else*/
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
GeometryGadget::set_shape(vector<string>&, WallGadget*) 
{
  
}
