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

#include "ExplosionGadget.h"
#include "Variable.h"

const VariableDefinition
ExplosionGadget::variable_def[ExplosionGadget::LAST_EXPLOSIONVAR] =
{
  {"Size", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"GrowTime", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"AtMaxTime", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"ShrinkTime", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"Temperature", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, 0, false, false, false},
  {"Damage", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"AirResistance", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"Visibility", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"SoundLevel", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"Colour", INT_V, 0, 0, INT_MAX, 0, false, false, false}//How should a colour be represented?
};

ExplosionGadget::ExplosionGadget( const char* name, Gadget* const p )
{
  init_variables( variable_def, LAST_EXPLOSIONVAR );
  init_functions( function_def, LAST_EXPLOSIONFCN ); 
}

void
ExplosionGadget::eval_function( const int fcn )
{
  switch( ExplosionFcns(fcn) )
    {
    case EXPLODE:
      break;
    default:
      break;
    }
}
