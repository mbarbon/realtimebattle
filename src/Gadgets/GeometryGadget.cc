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

#include "GeometryGadget.h"
#include "Variable.h"
#include "RobotBodyGadget.h"
#include <string>
#include "String.h"

const VariableDefinition 
GeometryGadget::variable_def[GeometryGadget::LAST_GEOMETRYVAR] = 
{
  // Name,      type, value, min,max, inaccm, random, read, write
  {"Friction",    DOUBLE_V, 0, 0.0, DBL_MAX,0, false, true, true },    
  {" AirResistance", DOUBLE_V, 0, 0.0, DBL_MAX,0, false, true, true },
  {"Gravity",    DOUBLE_V, 0, 0.0, DBL_MAX,0, false, true, true },   
  {"Visibility", DOUBLE_V, 0, 0.0, DBL_MAX,0, false, true, true },   
  {"Temperature",   DOUBLE_V, 0, 0.0, DBL_MAX,0, false, true, true },
  
  {"MatchTimeLimit",   BOOL_V, false,0,0,0,false,true,true},
  {"TimeLimit",       BOOL_V, false,0,0,0,false,true,true},
  {"FixedLength",     BOOL_V, false,0,0,0,false,true,true},
  {"Timeout",       DOUBLE_V, 0, 0.0, DBL_MAX,0, false, true, true }

};


GeometryGadget::GeometryGadget( const char* name, Gadget* const p ) 
  : Gadget(name, p, GAD_GEOMETRY)
{
  init_variables(variable_def, LAST_GEOMETRYVAR);
}

Gadget* 
GeometryGadget::build_in_variable( const string & s, const Gadget* build_as )
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
