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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <math.h>

#include "DefenseGadget.h"
#include "Variable.h"

const VariableDefinition
DefenseGadget::variable_def[DefenseGadget::LAST_DEFENSEVAR] =
{
  {"CollisionProtection", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"HeatProtection", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"ShotProtection", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"FrontAngle", DOUBLE_V, 0, 0, 2*M_PI, 0, false, true, false},
  {"MountTime", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"UnmountTime", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"MountCost", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"UnmountCost", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"UseCostPerTime", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"Attachable", BOOL_V, false, 0, 0, false, true, false},
  {"Colour", INT_V, 0, 0, INT_MAX, 0, false, false, false},//How should a colour be represented?
  {"Price", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"Mass", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false}
};

DefenseGadget::DefenseGadget( const char* name, Gadget* const p )
  : Gadget(name, p, GAD_DEFENSE)
{
  init_variables( variable_def, LAST_DEFENSEVAR );
  // Is there any need for this to be here?
  //init_functions( function_def, LAST_DEFENSEFCN ); 
}

// Sorry, no functions to be evaluated.
void
DefenseGadget::eval_function( const int fcn )
{
}
