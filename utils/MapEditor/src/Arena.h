#ifndef __Arena_h__
#define __Arena_h__


#include <stdio.h>
#include <vector>
#include "Gadget.h"


class Arena
{
public:
  Arena() 
    : TheGadgets(0) {};
  ~Arena();
  void Read(char*);
protected:
  int Read(FILE*);
  int ReadHead(FILE*);
  void Write();

  vector<Gadget*> TheGadgets;

  //Gadget *TheGadgets;
};



#endif
