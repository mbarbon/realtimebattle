#include <iostream.h>
#include "Arena.h"
#include "Gadget.h"
#include <algorithm>

using namespace std;

void Arena::Write()
{  
  for(vector<Gadget*>::iterator i=TheGadgets.begin(); i!=TheGadgets.end(); i++)
    {
      (*i)->Print();
    }
}

template<class T> struct deallocator    
//To dealloc the memory used by pointers in a vector...
{
  void operator()(T const p) {delete p;}
};

Arena::~Arena()
{
  for_each(TheGadgets.begin(), TheGadgets.end(),deallocator<Gadget*>());
  TheGadgets.clear();
}


