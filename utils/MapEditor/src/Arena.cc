#include <iostream.h>
#include "Arena.h"
#include "AllGadgets.h"
#include <algorithm>

using namespace std;

void Arena::Write()
{  
  for(vector<GadgetDefinition*>::iterator i=GlobalGadgetDefinitions.begin();
      i!=GlobalGadgetDefinitions.end();
      i++)
    {
      (*i)->theGadget->Print();
    }
}

template<class T> struct deallocator    
//To dealloc the memory used by pointers in a vector...
{
  void operator()(T const p) {delete p;}
};

Arena::~Arena()
{
  for_each(GlobalGadgetDefinitions.begin(), 
	   GlobalGadgetDefinitions.end(),
	   deallocator<GadgetDefinition*>());
  GlobalGadgetDefinitions.clear();
}


