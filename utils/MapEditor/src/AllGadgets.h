#ifndef __AllGadgets_hh__
#define __AllGadgets_hh__

#include <string>

#include "Gadget.h"
#include "ShotGadget.h"
#include "WeaponGadget.h"

struct BasicGadget
{
  string Name;
  Gadget *theGadget;  //Find a way not to use pointers...
};

extern BasicGadget AllGadgets[]; 


#endif
