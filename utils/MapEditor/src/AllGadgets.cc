#include "AllGadgets.h"


BasicGadget AllGadgets[] = 
{
  {"Weapon",      new WeaponGadget("", NULL)}, 
  {"Shot",        new ShotGadget("", NULL)}, 
  {"Explosion",   new ExplosionGadget("", NULL)},
  {"Wall",        new WallGadget("", NULL)},
  {"Defense",     new DefenseGadget("", NULL)},
  {"Sensor",      new SensorGadget("", NULL)},
  {"Environment", new EnvironmentGadget("", NULL)},
  {"", 0}
};

