#ifndef __Arena_h__
#define __Arena_h__


#include <stdio.h>
#include <vector>
#include "AllGadgets.h"


class Arena
{
public:
  Arena()  {};
  ~Arena();
  void Read(char*);
protected:
  int Read(FILE*);
  int ReadHead(FILE*);
  void Write();

  vector<GadgetDefinition*> GlobalGadgetDefinitions;

  //Gadget *TheGadgets;
};



#endif
