#include "AllGadgets.h"


BasicGadget AllGadgets[] = 
{
  {"Weapon",    new WeaponGadget("", NULL)}, 
  {"Shot",      new ShotGadget("", NULL)}, 
  {"Explosion", new ExplosionGadget("", NULL)},
  {"Wall",      new WallGadget("", NULL)},
  {"", 0}
};

