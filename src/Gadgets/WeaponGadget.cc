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

#include "WeaponGadget.h"
#include "Variable.h"


const VariableDefinition 
WeaponGadget::variable_def[WeaponGadget::LAST_WEAPONVAR] = 
{
  {"Ammunition", INT_V, 0, 0, INT_MAX,0,false,true,false},
  
  {"RotateToAngle", DOUBLE_V, 0, -DBL_MAX, DBL_MAX,0, false, true, true },
  {"RotateAmountAngle", DOUBLE_V, 0, -DBL_MAX, DBL_MAX,0, false, true, true },
  {"RotateSpeed", DOUBLE_V, 0, 0.0, DBL_MAX,0, false, true, true },
  
  {"AutoFire", BOOL_V, false,0,0,0,false,true,true}

//      "MaxRotateSpeed",

//      "SoundLevel",

//      "MountTime",
//      "UnmountTime",
//      "MountCost",
//      "UnmountCost",

//      "UseCostPerTime",

//      "AmmoLimit",
//      "StartAmmo",
//      "ReloadTime",

//      "TemperaturePerDamage",
//      "MaxTemperature",

//      "CoolDownPerTime",

//      "Attachable",

//      "Colour",
//      "Price",
//      "Mass"
};


const FunctionDefinition 
WeaponGadget::function_def[WeaponGadget::LAST_WEAPONFCN] = 
{
  { "Shoot", true },
  { "Mount", true },
  { "Unmount", true },
  { "Drop", true },
  { "Rotate", true },
  { "RotateTo", true },
  { "RotateAmount", true }
};

WeaponGadget::WeaponGadget( const char* name, Gadget* const p ) 
  : Gadget(name, p)
{
  init_variables(variable_def, LAST_WEAPONVAR);
  init_functions(function_def, LAST_WEAPONFCN);
}

void
WeaponGadget::eval_function( const int fcn )
{
  
  switch( WeaponFcns(fcn) )
    {
    case SHOOT:
      shoot();
      break;
    case MOUNT:
      break;
    case UNMOUNT:
      break;
    case DROP:
      break;
    case ROTATE:
      break;
    case ROTATETO:
      break;
    case ROTATEAMOUNT:
      break;
    default:
      break;
    }
}
