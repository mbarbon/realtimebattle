#ifndef __WEAPON_GADGET__
#define __WEAPON_GADGET__

#include <limits.h>
#include <float.h>

#include "Gadget.h"
#include "Variable.h"
#include "Function.h"
#include "ShotGadget.h"
#include "../Arena.h"



class WeaponGadget : public Gadget
{
 public:
   Gadget* NewInstance() { cout<<"Creating a new Weapon\n"; return new WeaponGadget; };
   int Read(FILE*);

 private:
  enum WeaponVars
  {
    AMMUNITION=0,  ROTATETOANGLE, ROTATEAMOUNTANGLE, ROTATESPEED,
    AUTOFIRE, MAXROTATESPEED, SOUNDLEVEL,
    MOUNTTIME, UNMOUNTTIME, MOUNTCOST, UNMOUNTCOST,
    USECOSTPERTIME, AMMOLIMIT, STARTAMMO, RELOADTIME,
    TEMPERATUREPERDAMAGE, MAXTEMPERATURE, COOLDOWNPERTIME,
    COLOUR, PRICE, MASS, LAST_WEAPONVAR
  };

  const static VariableDefinition variable_def[LAST_WEAPONVAR]; 

  enum WeaponFcns
  {
    SHOOT=0, MOUNT, UNMOUNT, DROP, ROTATE, ROTATETO, ROTATEAMOUNT, LAST_WEAPONFCN
  };

  const static FunctionDefinition function_def[LAST_WEAPONFCN];


  void shoot();
  
  void eval_function(const int fcn);
  

  // is true if attached to robot or ball

  bool movable;

  

  ShotGadget* shot;

};


#endif __WEAPON_GADGET__

