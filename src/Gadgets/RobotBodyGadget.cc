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

#include "RobotBodyGadget.h"
#include "Variable.h"

const VariableDefinition 
RobotBodyGadget::variable_def[RobotBodyGadget::LAST_ROBOTBODYVAR] =
{
  {"RotateSpeed", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, true},
  {"Friction", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"BounceCoefficient", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"Hardness", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"Protection", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"DamagePerHeat", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"DamagePerTime", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"HeatPerBrakeEffect", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},
  {"MaxBrakeHeat", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"MaxWeaponMass", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"MaxNumberOfWeapons", INT_V, 0, 0, INT_MAX, 0, false, true, false},
  {"MaxSensorMass", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"MaxNumberOfSensors", INT_V, 0, 0, INT_MAX, 0, false, true, false},
  {"MaxDefenseMass", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"MaxNumberOfDefenses", INT_V, 0, 0, INT_MAX, 0, false, true, false},
  {"MaxTotalLoad", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"CoolDownPerTime", DOUBLE_V, 0, 0, DBL_MAX, false, false, false},
  {"StartEnergy", DOUBLE_V, 0, 0, DBL_MAX, false, true, false},
  {"Colour", INT_V, 0, 0, INT_MAX, 0, false, false, false},//How should a colour be represented?
  {"Price", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"Mass", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false}
};

const FunctionDefinition 
RobotBodyGadget::function_def[RobotBodyGadget::LAST_ROBOTBODYFCN] = 
{
  { "Rotate", true },
  { "RotateTo", true },
  { "RotateAmount", true },
  { "Brake", true },
};

RobotBodyGadget::RobotBodyGadget( const char* name, Gadget* const p )
  : Gadget(name, p, GAD_ROBOTBODY)
{
  init_variables( variable_def, LAST_ROBOTBODYVAR );
  init_functions( function_def, LAST_ROBOTBODYFCN ); 
}

void
RobotBodyGadget::eval_function( const int fcn )
{
  switch( RobotBodyFcns(fcn) )
    {
    case ROTATE:
      break;
    case ROTATETO:
      break;
    case ROTATEAMOUNT:
      break;
    case BRAKE:
      break;
    default:
      break;
    }
}
