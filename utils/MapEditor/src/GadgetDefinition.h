#ifndef __Gadget_definition_hh__
#define __Gadget_definition_hh__

#include <string>

class Gadget;

struct GadgetDefinition
{
  string Name;
  string Type;
  Gadget *theGadget;
};


Gadget*
createGadget(string GadgetType, const char* Name);

GadgetDefinition* 
createGadgetDef(string /* GadgetType */, const char*, Gadget* const);
GadgetDefinition*
findGadgetDefinitionFor(string /* GadgetName */, Gadget* );

#endif
