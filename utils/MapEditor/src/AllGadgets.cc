#include "AllGadgets.h"


BasicGadget AllGadgets[] = 
{
  {"Weapon", new WeaponGadget()}, 
  {"Shot", new ShotGadget()}, 
  {"Explosion", new ExplosionGadget()},
};


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
