#ifndef __Gadget_h__
#define __Gadget_h__

#include <stdio.h>
#include <string>
#include <iostream.h>
#include <vector>

#include <gtk/gtk.h>


#include "GadgetSet.h"
#include "GadgetDefinition.h"

class Variable;
class Function;

struct VariableDefinition;
struct FunctionDefinition;


class Gadget
{
public:
  Gadget(string n) : name(n) {}
  friend GtkCTreeNode* AddToTree(GtkWidget*, GtkCTreeNode*, Gadget*);
  Gadget()  
    //variables(NULL), functions(NULL)  
    {}

  Gadget( const char* Name, Gadget* const p ) : 
    name(Name)
    //variables(NULL), functions(NULL)
    {
      //cout<<"NbGadgets : "<<last_id_used<<endl;
    }

  static int last_id_used;

  ~Gadget();

  int Read(FILE* /* fp */, GtkWidget* /* ctree */, GtkCTreeNode*);
  
  virtual vector<GadgetDefinition*>* Gadget_def() {return NULL;};
  virtual Gadget* Copy(Gadget*) {return NULL;};
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
  string name;
  string info_string;

  Variable* variables;
  //Function* functions;
};

GtkCTreeNode* AddToTree(GtkWidget*, GtkCTreeNode*, Gadget*);


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
protected:
   //float mySize;
};


class DefenseGadget : public Gadget
{
 public:
  DefenseGadget( const char* Name, Gadget* const p ) : Gadget(Name, p)
    {}
  Gadget* NewInstance( const char* Name, Gadget* const p)
    {
      return new DefenseGadget(Name, p);
    }
};


class SensorGadget : public Gadget
{
 public:
  SensorGadget( const char* Name, Gadget* const p ) : Gadget(Name, p)
    {}
  Gadget* NewInstance( const char* Name, Gadget* const p)
    {
      return new SensorGadget(Name, p);
    }
};

class EnvironmentGadget : public Gadget
{
 public:
  EnvironmentGadget( const char* Name, Gadget* const p ) : Gadget(Name, p)
    {}
  Gadget* NewInstance( const char* Name, Gadget* const p)
    {
      return new EnvironmentGadget(Name, p);
    }
};

#endif







