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

#ifndef __SCRIPT_GADGET__
#define __SCRIPT_GADGET__

#include <list>
#include <vector>
#include <stack>

#include "Gadget.h"
#include "Value.h"

class Variable;



class Script : public Gadget
{
public:
  
  Script( const char* name, Gadget* const p ) : Gadget(name,p, GAD_SCRIPT) {}
  ~Script() {}

  static Gadget* new_script( const char* name, Gadget* const p )
  {
    return new Script( name, p );
  }

  void run();
  void continue_script();

  const Script& operator= (const Script& v) ;  

  void add_script_lines( vector<string>& strings ) 
    {
      add_script_line( strings[0] );  //NOTE : Hmmmm
    };
  void add_script_line( string& s )
    { //Make it better for futur uses
      ScriptLine new_line;
      new_line.line_nr = 0;
      new_line.function = ACTION;
      new_line.is_variable = false;
      new_line.str_p = new string( s );
      commands.push_back(new_line);
    }

  enum ScriptFunction
  {
    PUSH, POP, COPY, STORE, ACTION,
    ADD, SUB, MULT, DIV, INV, NEG,
    SIN, COS, TAN, ATAN, ASIN, ACOS,
    LT, GT, LE, GE, EQ, NE,
    THEN, ELSE, GOTO, WAIT
  };
  

  struct ScriptLine
  {
    int line_nr;
    ScriptFunction function;
    bool is_variable;  // is the argument a variable?
    Value val;
    Variable* var_p;
    string* str_p;  // For actions

    Value& get_value();
  };

private:

  list<ScriptLine> commands;
  
  list<ScriptLine>::iterator current_line;
  
  stack<Value> val_stack;

  bool pausing;
};





#endif // __SCRIPT_GADGET__
