/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef __GADGET__
#define __GADGET__

#include <string>
#include <list>
#include <map>

#include "GadgetSet.h"

class Variable;
class Function;

struct VariableDefinition;
struct FunctionDefinition;

/*
  Gadget is a base class. The classes derives from it are used to store
  information about the characteristics of different "game" objects (like
  robots, weapons, sensors , etc). The Gadgets interact with the
  environment via Functions and Variables and can have a set of child
  gadgets.  
*/

class Gadget
{
public:
  Gadget() : 
    info(this, 0, "", GAD_NONE), 
    variables(NULL), functions(NULL) {}

  Gadget( const char* name, Gadget* const p, gadget_t type ) : 
    info(this, last_id_used++, name, type), 
    parent(p), 
    variables(NULL), functions(NULL) {}

  virtual ~Gadget();

  long int get_unique_id() const { return info.id; }
  const string& get_name() const { return info.name; }

  const GadgetInfo& get_info() const { return info; }

  Gadget* get_parent() const { return parent; }
  GadgetSet& get_my_gadgets() { return my_gadgets; }

  static void set_last_id_used() { last_id_used = 0; }
  
  bool eval_message(const string& msg);

  void eval_function(const int fcn) {}

  const Variable* get_variables() const { return variables; }
  const Function* get_functions() const { return functions; }

  virtual Gadget* create_instance( const string& s, const Gadget* ) { return NULL; }

  Gadget* find_by_name( const string& gadget_name ); 

protected:

  void init_variables( const VariableDefinition* var_def, const int last_var );
  void init_functions( const FunctionDefinition* fcn_def, const int last_fcn );

  GadgetInfo info;


  Gadget* parent;

  // Pointer to the gadget in the GadgetDefinition hierarchy which defined
  // this gadget
  //  Gadget* defining_gadget;

  GadgetSet my_gadgets;

  Variable* variables;  //This is variable definitions
  Function* functions;  //This is function definitions

  map<string, Gadget*> instance; //I don't know what it's gonna be use

public:
  static int last_id_used;
};

#endif // __GADGET__
