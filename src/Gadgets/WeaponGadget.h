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

#ifndef __WEAPON_GADGET__
#define __WEAPON_GADGET__

#include <limits.h>
#include <float.h>

#include "Gadget.h"
#include "Variable.h"
#include "Function.h"
#include "ShotGadget.h"
#include "../Arena.h"
#include "../EventHandler.h"

class WeaponGadget : public Gadget
{
public:

  WeaponGadget( const char* name, Gadget* const p );

  static Gadget* new_weapongadget( const char* name, Gadget* const p )
  {
    return new WeaponGadget( name, p );
  }


  enum WeaponVars
  {
    AMMUNITION=0,  STOPANGLE, LEFTANGLE, RIGHTANGLE, ROTATESPEED,
    AUTOFIRE, MAXROTATESPEED, SOUNDLEVEL,
    MOUNTTIME, UNMOUNTTIME, MOUNTCOST, UNMOUNTCOST,
    USECOSTPERTIME, AMMOLIMIT, STARTAMMO, RELOADTIME,
    TEMPERATUREPERDAMAGE, MAXTEMPERATURE, COOLDOWNPERTIME, ATTACHABLE,
    COLOUR, PRICE, MASS, LAST_WEAPONVAR
  };

  Gadget* create_instance( const string & s, const Gadget*  );

private:

  const static VariableDefinition variable_def[LAST_WEAPONVAR]; 

  enum WeaponFcns
  {
    SHOOT=0, MOUNT, UNMOUNT, DROP, ROTATE, ROTATETO, ROTATEAMOUNT, SWEEP, LAST_WEAPONFCN
  };

  const static FunctionDefinition function_def[LAST_WEAPONFCN];


  void shoot();
  
  void eval_function(const int fcn);
  

  // is true if attached to robot or ball

  bool movable;
  double last_shoot_time;  //Shoudn't this be a normal varible also ?
};


#endif // __WEAPON_GADGET__
