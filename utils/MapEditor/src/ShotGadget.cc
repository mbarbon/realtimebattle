#include "ShotGadget.h"


/*
const VariableDefinition 
ShotGadget::variable_def[ShotGadget::LAST_SHOTVAR] =
{
  {"Damage", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, true},
  {"Speed", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},//Should it be readable?
  {"Size", DOUBLE_V, 0, 0, DBL_MAX, 0, false, false, false},//Should it be readable?
  {"CostPerDamage", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, 0, false, true, false},
  {"CostPerSpeed", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, 0, false, true, false},
  {"MaxDamage", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"MinDamage", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"WeaponHeatPerDamage", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, 0, false, false, false},//Should it be readable?
  {"Temperature", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, 0, false, false, false},//Should it be readable?
  {"MaxTime", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"TargetSeeking", BOOL_V, false, 0, 0, 0, false, true, false},//Should min-,max-value be this?
  {"MaxAngularVelocity", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"Colour", INT_V, 0, 0, INT_MAX, 0, false, false, false},//How should a colour be represented?
  {"Price", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"Mass", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false}
};
*/

ShotGadget::ShotGadget( const char* name, Gadget* const p ) : Gadget ( name, p )
{
  //init_variables( variable_def, LAST_SHOTVAR );
  // Is there any need for this to be here?
  //init_functions( function_def, LAST_SHOTFCN ); 
}

// Sorry, no functions to be evaluated.
void
ShotGadget::eval_function( const int fcn )
{
}

