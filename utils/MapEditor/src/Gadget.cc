#include <iostream.h>
#include "Gadget.h"


int Gadget::last_id_used = 0;

Gadget::~Gadget()
{
  /*
    if( variables != NULL )
    delete [] variables;
    
    if( functions != NULL )
    delete [] functions;
  */
}

void Gadget::Print()
{
  cout<<"Here is an object\n";
}

