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

#include "WeaponGadget.h"
#include "Variable.h"
#include "RobotBodyGadget.h"
#include "Shot.h"
#include <string>
#include "String.h"

const VariableDefinition 
WeaponGadget::variable_def[WeaponGadget::LAST_WEAPONVAR] = 
{
  // Name,      type, value, min,max, inaccm, random, read, write
  {"Ammunition", INT_V, 0, 0, INT_MAX, 0, false, true, false},
  
  {"StopAngle", DOUBLE_V, 0, -DBL_MAX, DBL_MAX,0, false, true, true },
  {"LeftAngle", DOUBLE_V, 0, -DBL_MAX, DBL_MAX,0, false, true, true },
  {"RightAngle", DOUBLE_V, 0, -DBL_MAX, DBL_MAX,0, false, true, true },

  {"RotateSpeed", DOUBLE_V, 0, 0.0, DBL_MAX,0, false, true, true },
  
  {"AutoFire", BOOL_V, false,0,0,0,false,true,true},

  {"MaxRotateSpeed", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, false, true, false },

  {"SoundLevel", INT_V, 0, 0, INT_MAX, 0, false, true, false},
  
  {"MountTime", DOUBLE_V, 0, 0, DBL_MAX, false, true, false },
  {"UnmountTime", DOUBLE_V, 0, 0, DBL_MAX, false, true, false },
  {"MountCost", DOUBLE_V, 0, 0, DBL_MAX, false, true, false },
  {"UnmountCost", DOUBLE_V, 0, 0, DBL_MAX, false, true, false },
  
  {"UseCostPerTime", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, false, true, false },
  
  {"AmmoLimit", BOOL_V, false,0,0,0,false,true,true},
  {"StartAmmo", BOOL_V, false,0,0,0,false,true,true},
  {"ReloadTime", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, false, true, false },

  {"TemperaturePerDamage", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, false, true, false },
  {"MaxTemperature", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, false, true, false },

  {"CoolDownPerTime", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, false, true, false },

  {"Attachable", BOOL_V, false,0,0,0,false,true,true},

  {"Colour", INT_V, 0, 0, INT_MAX, false, true, false },
  {"Price", DOUBLE_V, 0, 0, DBL_MAX, false, true, false },
  {"Mass", DOUBLE_V, 0, 0, DBL_MAX, false, true, false }
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

Gadget* 
WeaponGadget::create_instance( const string & s, const Gadget* build_as )
{
  if( equal_strings_nocase( s, "Shot") )
    {
      //TODO : dynamic_cast<ShotGadget*> (build_as);
      cout<<"Here\n";
      ShotGadget* build_as_shot = (ShotGadget*) build_as;
      instance["Shot"]  = new ShotGadget( s.c_str(), this );
      (*instance["Shot"]) = *build_as_shot;
      return instance["Shot"];
    }
  else
    return NULL;
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

  ShotGadget* shot = dynamic_cast<ShotGadget*> (instance["Shot"]);  //TODO : dynamic_cast

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
