#ifndef __Gadget_h__
#define __Gadget_h__

#include <stdio.h>

class Gadget
{
public:
  Gadget(char* Name) 
    : Next(0) {strcpy(myName, Name);};
  virtual int Read(FILE* /* fp */)
  {
    return 0;
  };
  virtual void Print();
  Gadget * Next;
protected:
  char myName[50]; //Maybe use a string...?
};

class Wall : public Gadget
{
public:
  Wall(char* Name) 
    : Gadget(Name), mySize(0) {};
  int Read(FILE*);
  void Print();
protected:
  float mySize;
};

#endif
