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

#include "AllGadgets.h"
#include "WallGadget.h"
#include "Variable.h"
#include "RobotBodyGadget.h"
#include "Shot.h"
#include <string>
#include "String.h"

const VariableDefinition 
WallGadget::variable_def[WallGadget::LAST_WALLVAR] = 
{
  //TODO : check the params 
  // Name,      type, value, min,max, inaccm, random, read, write
  {"Colour", INT_V, 0, 0, INT_MAX, false, true, false },
  {"Size", DOUBLE_V, 0, 0, DBL_MAX, false, true, false },
  {"Temperature", DOUBLE_V, 0, 0, DBL_MAX, false, true, false }
};


WallGadget::WallGadget( const char* name, Gadget* const p ) 
  : Gadget(name, p, GAD_WALL)
{
  init_variables(variable_def, LAST_WALLVAR);
}

Gadget* 
WallGadget::create_instance( const string & s, const Gadget* build_as )
{
  if( equal_strings_nocase( s, "ActionAtCollision") )
    {
      //TODO : dynamic_cast !!!
      ActionAtCollision = (Script*) build_as;
      return ActionAtCollision;
    }
  else if( equal_strings_nocase( s, "ActionWhenHit" ) )
    {
      ActionWhenHit = (Script*) build_as;
      return ActionWhenHit;
    }
  else 
    {
      //Create a new instance of the defined object !  Maybe this code must be in the Gadget::build_in_variable
      instance[s] =  AllGadgets::create_gadget_by_stringtype( lowercase( gadget_types[ build_as->get_info().type ] ),
							      s.c_str(), this);
      return instance[s];
    }

  return NULL;
}

void
WallGadget::eval_function( const int fcn )
{}
