#ifndef __WEAPON_GADGET__
#define __WEAPON_GADGET__

#include <limits.h>
#include <float.h>

#include "Gadget.h"
//#include "Variable.h"
//#include "Function.h"
#include "ShotGadget.h"
//#include "../Arena.h"
#include "AllGadgets.h"


class WeaponGadget : public Gadget
{
 public:
  WeaponGadget( const char*, Gadget* const);
  Gadget* NewInstance( const char* Name, Gadget* const p ) 
    { 
      return new WeaponGadget(Name, p); 
    };
   int Read(FILE*, GtkWidget*, GtkCTreeNode*);
   vector<GadgetDefinition*> *Gadget_def();


 private:
  enum WeaponVars
  {
    AMMUNITION=0,  ROTATETOANGLE, ROTATEAMOUNTANGLE, ROTATESPEED,
    AUTOFIRE, 
    //MAXROTATESPEED, SOUNDLEVEL,
    //MOUNTTIME, UNMOUNTTIME, MOUNTCOST, UNMOUNTCOST,
    //USECOSTPERTIME, AMMOLIMIT, STARTAMMO, RELOADTIME,
    //TEMPERATUREPERDAMAGE, MAXTEMPERATURE, COOLDOWNPERTIME,
    //COLOUR, PRICE, MASS, 
    LAST_WEAPONVAR
  };

  

  vector<GadgetDefinition*> gadget_def;

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

  void Print();

  ShotGadget* shot;

};


#endif __WEAPON_GADGET__

