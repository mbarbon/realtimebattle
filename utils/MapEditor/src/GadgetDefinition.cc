#include "GadgetDefinition.h"
#include "AllGadgets.h"

GadgetDefinition* 
createGadgetDef(string GadgetType, const char* Name, Gadget* const p)
{
  Gadget* G = NULL;
  GadgetDefinition *ToReturn = NULL;
  for(int i = 0; AllGadgets[i].theGadget != 0; i++)
    {
      if(AllGadgets[i].Name == GadgetType)
	{
	  G = AllGadgets[i].theGadget->NewInstance(Name, p);
	  break;
	}
    }
  if(G)
    {
      ToReturn = new GadgetDefinition;
      ToReturn->Name      = string(Name);
      ToReturn->Type      = GadgetType;
      ToReturn->theGadget = G;
    }
  return ToReturn;
}

GadgetDefinition*
findGadgetDefinitionFor(string GadgetName, Gadget* theGadget)
{
  //Try to find in the courant gadget_def or in the parental gadget_def
  cout<<"I try to find a definition for "<<GadgetName<<endl;
  vector<GadgetDefinition*> *Gadget_def = theGadget->Gadget_def();
  //TODO : Find the gadget definition...
  vector<GadgetDefinition*>::iterator i;
  for(i = Gadget_def->begin(); i != Gadget_def->end(); i++)
    {
      if((*i)->Name == GadgetName)
	{
	  return (*i);
	}
    }
  return NULL;
}
