#ifndef __SHOT_GADGET_H__
#define __SHOT_GADGET_H__

#include "Gadget.h"
/*
#include "Variable.h"
#include "Function.h"
*/

class ShotGadget : public Gadget
{
 public:
  ShotGadget(const char* Name, Gadget* const p);
  Gadget* NewInstance( const char* Name, Gadget* const p) 
  { 
    //cout<<"Creating a new ShotGadget\n"; 
    return new ShotGadget(Name, p); 
  };
  int Read(FILE*);

  enum ShotVars
  {
    DAMAGE=0, SPEED, SIZE, COSTPERDAMAGE, COSTPERSPEED,
    MAX_DAMAGE, MIN_DAMAGE, WEAPONHEATPERDAMAGE, TEMPERATURE,
    MAXTIME, TARGETSEEKING, MAXANGULARVELOCITY, COLOUR,
    PRICE, MASS, LAST_SHOTVAR
  };
  
  //const static VariableDefinition variable_def[LAST_SHOTVAR]; 
  
  enum ShotFcns
  {
    LAST_SHOTFCN
  };

  //const static FunctionDefinition function_def[LAST_SHOTFCN];


  void eval_function(const int fcn);
};

#endif
