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
    AMMUNITION,  ROTATETOANGLE, ROTATEAMOUNTANGLE, ROTATESPEED,
    AUTOFIRE, MAXROTATESPEED, SOUNDLEVEL,
    MOUNTTIME, UNMOUNTTIME, MOUNTCOST, UNMOUNTCOST,
    USECOSTPERTIME, AMMOLIMIT, STARTAMMO, RELOADTIME,
    TEMPERATUREPERDAMAGE, MAXTEMPERATURE, COOLDOWNPERTIME,
    ATTACHABLE, COLOUR, PRICE, MASS, LAST_WEAPONVAR
  };

  const VariableDefinition variables[LAST_WEAPONVAR] = 
  {
    {"Ammunition", INT, 0, 0, INT_MAX,0,false,true,false},

    {"RotateToAngle", DOUBLE, 0, -DBL_MAX, DBL_MAX,0, false, true, true },
    {"RotateAmountAngle", DOUBLE, 0, -DBL_MAX, DBL_MAX,0, false, true, true },
    {"RotateSpeed", DOUBLE, 0, 0.0, DBL_MAX,0, false, true, true },

    {"AutoFire", BOOL, false,0,0,0,false,true,true}

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

  enum WeaponFcns
  {
    SHOOT, MOUNT, UNMOUNT, DROP, ROTATE, ROTATETO, ROTATEAMOUNT, LAST_WEAPONFCN
  };

  Function functions[LAST_WEAPONFCN];

  void shoot() {}
  
  void eval_function(const int fcn);
  



  //  ShotGadget* shot;



};


#endif __WEAPON_GADGET__
