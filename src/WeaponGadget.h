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

#ifndef __WEAPON_GADGET__
#define __WEAPON_GADGET__

#include <limits.h>
#include <float.h>

#include "Gadget.h"
#include "Variable.h"
#include "Function.h"


class WeaponGadget : public Gadget
{
public:

  WeaponGadget( const char* name, Gadget* const p );

private:

  enum WeaponVars
  {
    AMMUNITION=0,  ROTATETOANGLE, ROTATEAMOUNTANGLE, ROTATESPEED,
    AUTOFIRE, MAXROTATESPEED, SOUNDLEVEL,
    MOUNTTIME, UNMOUNTTIME, MOUNTCOST, UNMOUNTCOST,
    USECOSTPERTIME, AMMOLIMIT, STARTAMMO, RELOADTIME,
    TEMPERATUREPERDAMAGE, MAXTEMPERATURE, COOLDOWNPERTIME,
    ATTACHABLE, COLOUR, PRICE, MASS, LAST_WEAPONVAR
  };

  const VariableDefinition variable_def[LAST_WEAPONVAR] = 
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

  Variable variables[LAST_WEAPONVAR];

  enum WeaponFcns
  {
    SHOOT=0, MOUNT, UNMOUNT, DROP, ROTATE, ROTATETO, ROTATEAMOUNT, LAST_WEAPONFCN
  };

  const FunctionDefinition function_def[LAST_WEAPONFCN] = 
  {
    { "Shoot", true },
    { "Mount", true },
    { "Unmount", true },
    { "Drop", true },
    { "Rotate", true },
    { "RotateTo", true },
    { "RotateAmount", true }
  };

  Function functions[LAST_WEAPONFCN];

  void shoot() {}
  
  void eval_function(const int fcn);
  



  //  ShotGadget* shot;



};


#endif __WEAPON_GADGET__
