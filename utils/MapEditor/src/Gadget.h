#ifndef __Gadget_h__
#define __Gadget_h__

#include <stdio.h>
#include <string>
#include <iostream.h>

#include "GadgetSet.h"

class Variable;
class Function;

struct VariableDefinition;
struct FunctionDefinition;

class Gadget
{
public:
  Gadget() : 
    info(this, 0, ""),
    parent(NULL)
    /*variables(NULL), functions(NULL)*/  
    {}

  Gadget( const char* Name, Gadget* const p ) : 
    info(this, last_id_used++, Name),
    parent(p)
    /*variables(NULL), functions(NULL)*/
    {
      cout<<"NbGadgets : "<<last_id_used<<endl;
    }

  static int last_id_used;

  ~Gadget();

  virtual int Read(FILE* /* fp */)
  {
    return 0;
  };

  virtual void Print();
  virtual Gadget* NewInstance( const char* Name, Gadget* const p ) 
    { return new Gadget(Name, p); }

  /*
    public:
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

  */

 protected:
  string info_string;
  GadgetInfo info;
  Gadget* parent;
  //GadgetSet my_gadgets;
  Variable* variables;
  //Function* functions;

};

class ExplosionGadget : public Gadget
{
 public:
  ExplosionGadget( const char* Name, Gadget* const p ) : Gadget(Name, p)
    {}
  Gadget* NewInstance( const char* Name, Gadget* const p ) 
    { 
      cout<<"Creating a new Explosion\n"; 
      return new ExplosionGadget(Name, p); 
    };
   int Read(FILE*);
protected:
   //float mySize;
};

#endif



