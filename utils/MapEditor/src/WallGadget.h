#ifndef __WALL_GADGET_HH__
#define __WALL_GADGET_HH__

#include "Gadget.h"

class WallGadget : public Gadget
{
public:
  WallGadget(const char* Name, Gadget* const);
  Gadget* NewInstance (const char* Name, Gadget* const p)
    {
      return new WallGadget(Name, p);
    }
  int Read(FILE*);
  void Print()
    { cout<<"A WallGadget\n";
    }
protected:
  //float mySize;
};


#endif  __WALL_GADGET_HH__
