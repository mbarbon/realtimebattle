#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "WeaponGadget.h"
#include "Variable.h"


const VariableDefinition 
WeaponGadget::variable_def[WeaponGadget::LAST_WEAPONVAR] = 
{
  // Name,      type, value, min,max, inaccm, random, read, write
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

void
WeaponGadget::shoot()
{  
  // s = new Shot(c,vel,en, my_shotgadget);
  //  the_arena.add_shot( s );
}
