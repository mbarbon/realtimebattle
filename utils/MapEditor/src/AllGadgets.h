#ifndef __AllGadgets_hh__
#define __AllGadgets_hh__

#include <string>

class Gadget;
struct GadgetDefinition;

#include "ShotGadget.h"
#include "WeaponGadget.h"
#include "WallGadget.h"

struct BasicGadget
{
  string Name;
  Gadget *theGadget;  //Find a way not to use pointers...
};

extern BasicGadget AllGadgets[]; 

#endif
