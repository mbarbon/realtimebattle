#ifndef __AllGadgets_hh__
#define __AllGadgets_hh__

#include "Gadget.h"
#include <string>



struct BasicGadget
{
  string Name;
  Gadget *theGadget;  //Find a way not to use pointers...
};

extern BasicGadget AllGadgets[]; 


#endif
