#ifndef __Gadget_h__
#define __Gadget_h__

#include <stdio.h>
#include <string>
#include <iostream.h>


class Gadget
{
public:
  Gadget() 
    : Next(0), myName("") {};
  Gadget(char* Name) 
    : Next(0), myName(Name) {};
  virtual int Read(FILE* /* fp */)
  {
    return 0;
  };
  virtual void Print();
  virtual Gadget* NewInstance() { return new(Gadget); };

  Gadget * Next;

  /*
    public:
    Gadget() : 
    info(this, 0, ""), 
    variables(NULL), functions(NULL) {}
    Gadget( const char* name, Gadget* const p ) : 
    info(this, last_id_used++, name), 
    parent(p), 
    variables(NULL), functions(NULL) {}
    ~Gadget();
    long int get_unique_id() const { return info.id; }
    const string& get_name() const { return info.name; }
    const GadgetInfo& get_info() const { return info; }
    Gadget* get_parent() const { return parent; }
    GadgetSet& get_my_gadgets() { return my_gadgets; }
    static void set_last_id_used() { last_id_used = 0; }
    bool eval_message(const string& msg);
    void eval_function(const int fcn) {}
    const Variable* get_variables() const { return variables; }
    protected:
    void init_variables( const VariableDefinition* var_def, const int last_var );
    void init_functions( const FunctionDefinition* fcn_def, const int last_fcn );
    GadgetInfo info;
    Gadget* parent;
    GadgetSet my_gadgets;
    Variable* variables;
    Function* functions;
    public:
    static int last_id_used;
  */

protected:
  string myName;
  string info_string;
};


class ShotGadget : public Gadget
{
 public:
   Gadget* NewInstance() { cout<<"Creating a new ShotGadget\n"; return NULL; };
 
};

class WeaponGadget : public Gadget
{
 public:
   Gadget* NewInstance() { cout<<"Creating a new Weapon\n"; return NULL; };
};

class ExplosionGadget : public Gadget
{
 public:
   Gadget* NewInstance() { cout<<"Creating a new Explosion\n"; return NULL; };
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
