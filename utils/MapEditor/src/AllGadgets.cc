#include "AllGadgets.h"


BasicGadget AllGadgets[] = 
{
  {"Weapon",    new WeaponGadget("", NULL)}, 
  {"Shot",      new ShotGadget("", NULL)}, 
  {"Explosion", new ExplosionGadget("", NULL)},
  {"Wall",      new WallGadget("", NULL)},
  {"", 0}
};


Gadget* createGadget(string GadgetType, const char* Name, Gadget* const p)
{
  for(int i = 0; AllGadgets[i].theGadget != 0; i++)
    {
      if(AllGadgets[i].Name == GadgetType)
	{
	  return AllGadgets[i].theGadget->NewInstance(Name, p);
	}
    }
  return NULL;
}

/* UnComment to make a test
//g++ -o test AllGadgets.cc Gadget.cc

#include <iostream.h>

void main()
{
  cout<<"Test...\n";
  AllGadgets[1].theGadget->NewInstance();
  AllGadgets[0].theGadget->NewInstance();
  AllGadgets[2].theGadget->NewInstance();
  for(int i = 0; i < 3; i++)
    delete  AllGadgets[i].theGadget;
  cout<<"End of test...\n";
}
*/
