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

#include "ShotGadget.h"
#include "Variable.h"

const VariableDefinition 
ShotGadget::variable_def[ShotGadget::LAST_SHOTVAR] =
{
  {"Damage", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, true},
  {"Speed", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},//Should it be readable?
  {"Size", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},//Should it be readable?
  {"CostPerDamage", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, 0, false, true, false},
  {"CostPerSpeed", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, 0, false, true, false},
  {"MaxDamage", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"MinDamage", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"WeaponHeatPerDamage", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, 0, false, false, false},//Should it be readable?
  {"Temperature", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, 0, false, false, false},//Should it be readable?
  {"MaxTime", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"TargetSeeking", BOOL_V, false, 0, 0, 0, false, true, false},//Should min-,max-value be this?
  {"MaxAngularVelocity", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"Colour", INT_V, 0, 0, INT_MAX, 0, false, false, false},//How should a colour be represented?
  {"Price", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"Mass", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false}
};

ShotGadget::ShotGadget( const char* name, Gadget* const p )
  : Gadget(name, p, GAD_SHOT)
{
  init_variables( variable_def, LAST_SHOTVAR );
  // Is there any need for this to be here?
  //init_functions( function_def, LAST_SHOTFCN ); 
}


const ShotGadget& 
ShotGadget::operator= (const ShotGadget& v) {
  for(int i = 0; i < LAST_SHOTVAR; i++)
    variables[i] = (Value&) v.variables[i];
  return (*this);
}


// Sorry, no functions to be evaluated.
void
ShotGadget::eval_function( const int fcn )
{
}
