#include <iostream.h>
#include "Arena.h"
#include "Gadget.h"

void Arena::Write()
{
  for(vector<Gadget*>::iterator i = TheGadgets.begin(); i != TheGadgets.end(); i++)
    {
      (*i)->Print();
    }
}


Arena::~Arena()
{
  for(vector<Gadget*>::iterator i = TheGadgets.begin(); i != TheGadgets.end(); i++)
    {
      delete (*i);
    }
}


