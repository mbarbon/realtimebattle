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

#include "EngineGadget.h"
#include "Variable.h"

const VariableDefinition
EngineGadget::variable_def[EngineGadget::LAST_ENGINEVAR] =
{
  {"MaxPower", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"HeatPerPower", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"SoundLevelPerPower", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"UseCostPerTime", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, 0, false, true, false},
  {"UseCostPerPower", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, 0, false, true, false},
  {"MaxTemperature", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"Colour", INT_V, 0, 0, INT_MAX, 0, false, false, false},//How should a colour be represented?
  {"Price", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"Mass", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false}
};

const FunctionDefinition 
EngineGadget::function_def[EngineGadget::LAST_ENGINEFCN] = 
{
  { "Accelerate", true }
};

EngineGadget::EngineGadget( const char* name, Gadget* const p )
{
  init_variables( variable_def, LAST_ENGINEVAR );
  init_functions( function_def, LAST_ENGINEFCN ); 
}

void
EngineGadget::eval_function( const int fcn )
{
  switch( EngineFcns(fcn) )
    {
    case ACCELERATE:
      break;
    default:
      break;
    }
}
