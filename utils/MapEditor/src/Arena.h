#ifndef __Arena_h__
#define __Arena_h__


#include <stdio.h>
#include <vector>
#include <gtk/gtk.h>
#include "AllGadgets.h"


class Arena
{
public:
  Arena()  {};
  ~Arena();
  void Read(char*, GtkWidget* );
protected:
  int Read(FILE*, GtkWidget*);
  int ReadHead(FILE*);
  void Write();

  vector<GadgetDefinition*> GlobalGadgetDefinitions;

  //Gadget *TheGadgets;
};



#endif
