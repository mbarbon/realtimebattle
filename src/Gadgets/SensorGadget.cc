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

#include "SensorGadget.h"
#include "Variable.h"
#include "RobotBodyGadget.h"
#include <string>
#include "String.h"

const VariableDefinition 
SensorGadget::variable_def[SensorGadget::LAST_SENSORVAR] = 
{
  // Name,      type, value, min,max, inaccm, random, read, write
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
  {"Mass", DOUBLE_V, 0, 0, DBL_MAX, false, true, false },


  {"Microphone",      BOOL_V, false,0,0,0,false,true,false},
  {"Sonar",           BOOL_V, false,0,0,0,false,true,false},
  {"InfraredLight",   BOOL_V, false,0,0,0,false,true,false},
  {"Laser",           BOOL_V, false,0,0,0,false,true,false},
  {"Radiowave",       BOOL_V, false,0,0,0,false,true,false},
  {"Vision",          BOOL_V, false,0,0,0,false,true,false},

  {"Sector",          BOOL_V, false,0,0,0,false,true,false},          
  {"Line",            BOOL_V, true,0,0,0,false,true,false},         
  {"LineWidth",       DOUBLE_V, 0, 0, DBL_MAX, false, true, false },            
  {"OmniDirectional", BOOL_V, false,0,0,0,false,true,false},       

  {"MaxObjectsSeen",  INT_V, 1, 0, INT_MAX, 0, false, true, false},           

  {"UpdateFrequency", DOUBLE_V, 0, 0, DBL_MAX, false, true, false }
};


const FunctionDefinition 
SensorGadget::function_def[SensorGadget::LAST_SENSORFCN] = 
{
  { "Mount", true },
  { "Unmount", true },
  { "Drop", true },
  { "Rotate", true },
  { "RotateTo", true },
  { "RotateAmount", true },
  { "Sweep", true },
};

SensorGadget::SensorGadget( const char* name, Gadget* const p ) 
  : Gadget(name, p, GAD_SENSOR)
{
  init_variables(variable_def, LAST_SENSORVAR);
  init_functions(function_def, LAST_SENSORFCN);
}

Gadget* 
SensorGadget::build_in_variable( const string & s, const Gadget* build_as )
{
  /*  cout<<s<<endl;
  if( equal_strings_nocase( s, "Shot") )
    {
      //TODO : dynamic_cast<ShotGadget*> (build_as);
      cout<<"Here\n";
      ShotGadget* build_as_shot = (ShotGadget*) build_as;
      shot  = new ShotGadget( s.c_str(), this );
      (*shot) = *build_as_shot;
      return shot;
    }
    else*/
    return NULL;
}

void
SensorGadget::eval_function( const int fcn )
{
  
  switch( SensorFcns(fcn) )
    {
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
