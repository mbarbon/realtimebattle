#ifndef __AllGadgets_hh__
#define __AllGadgets_hh__

#include <string>

class Gadget;
struct GadgetDefinition;

#include "Gadget.h"
#include "ShotGadget.h"
#include "WeaponGadget.h"
#include "WallGadget.h"

struct GadgetDefinition
{
  string Name;
  string Type;
  Gadget *theGadget;
};

struct BasicGadget
{
  string Name;
  Gadget *theGadget;  //Find a way not to use pointers...
};

extern BasicGadget AllGadgets[]; 


GadgetDefinition* 
createGadgetDef(string /* GadgetType */, const char*, Gadget* const);


#endif
