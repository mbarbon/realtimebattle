#include <iostream.h>
#include "Arena.h"
#include "Gadget.h"

void Arena::Write()
{
  Gadget *O;
  O = TheGadgets;
  while(O != NULL)
    {
      O->Print();
      O = O->Next;
    }
}


Arena::~Arena()
{
  Gadget *O;
  while(TheGadgets)
    {
      O = TheGadgets;
      TheGadgets = TheGadgets->Next;
      delete O;
    }
}


