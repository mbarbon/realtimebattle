#include <iostream.h>
#include "Gadget.h"

void Gadget::Print()
{
  cout<<"Here is an object\n";
}

void Wall::Print()
{
  cout<<"\nHello, I'm a wall\n";
  cout<<"My name is "<<myName<<endl;
  cout<<"My size is "<<mySize<<endl;
  cout<<"Good Bye"<<endl;
}
