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
#include "RobotBodyGadget.h"
#include "Shot.h"


const VariableDefinition 
WeaponGadget::variable_def[WeaponGadget::LAST_WEAPONVAR] = 
{
  // Name,      type, value, min,max, inaccm, random, read, write
  {"Ammunition", INT_V, 0, 0, INT_MAX, 0, false, true, false},
  
  {"StopAngle", DOUBLE_V, 0, -DBL_MAX, DBL_MAX,0, false, true, true },
  {"LeftAngle", DOUBLE_V, 0, -DBL_MAX, DBL_MAX,0, false, true, true },
  {"RightAngle", DOUBLE_V, 0, -DBL_MAX, DBL_MAX,0, false, true, true },

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
  { "RotateAmount", true },
  { "Sweep", true }
  
};

WeaponGadget::WeaponGadget( const char* name, Gadget* const p ) 
  : Gadget(name, p, GAD_WEAPON)
{
  init_variables(variable_def, LAST_WEAPONVAR);
  init_functions(function_def, LAST_WEAPONFCN);

  last_shoot_time = -1e10;
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

void
WeaponGadget::shoot()
{
  assert( parent->get_info().type == GAD_ROBOTBODY );
  // TODO: other objects should be able to carry weapon 

  if( variables[AMMUNITION] < 0 )
    {
      // TODO: Send warning message
      return;
    }

  double cur_time = the_eventhandler.get_game_time();

  if( cur_time < last_shoot_time + variables[RELOADTIME] )
    return;

  last_shoot_time = cur_time;
  
  variables[AMMUNITION] -= 1;

  RobotBodyGadget* body = (RobotBodyGadget*) parent;

  Vector2D dir = angle2vec(body->get_variables()[RobotBodyGadget::DIRECTION]);
  Vector2D c( body->get_variables()[RobotBodyGadget::X_POS],
              body->get_variables()[RobotBodyGadget::Y_POS] );

  c += dir * (body->get_variables()[RobotBodyGadget::RADIUS] * 1.01);
  dir *= shot->get_variables()[ShotGadget::SPEED];

  Shot* s = new Shot( c, dir * shot->get_variables()[ShotGadget::SPEED], *shot);

  the_arena.add_shot( s );
}
