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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "AllGadgets.h"
#include "EnvironmentGadget.h"
#include "Variable.h"
#include "RobotBodyGadget.h"
#include <string>
#include "String.h"

const VariableDefinition 
EnvironmentGadget::variable_def[EnvironmentGadget::LAST_ENVIRONMENTVAR] = 
{
  // Name,      type, value, min,max, inaccm, random, read, write
  {"Friction",    DOUBLE_V, 0, 0.0, DBL_MAX,0, false, true, true },    
  {"AirResistance", DOUBLE_V, 0, 0.0, DBL_MAX,0, false, true, true },
  {"Gravity",    DOUBLE_V, 0, 0.0, DBL_MAX,0, false, true, true },   
  {"Visibility", DOUBLE_V, 0, 0.0, DBL_MAX,0, false, true, true },   
  {"Temperature",   DOUBLE_V, 0, 0.0, DBL_MAX,0, false, true, true },
  
  {"MatchTimeLimit",   BOOL_V, false,0,0,0,false,true,true},
  {"TimeLimit",       BOOL_V, false,0,0,0,false,true,true},
  {"FixedLength",     BOOL_V, false,0,0,0,false,true,true},
  {"Timeout",       DOUBLE_V, 0, 0.0, DBL_MAX,0, false, true, true }

};


EnvironmentGadget::EnvironmentGadget( const char* name, Gadget* const p ) 
  : Gadget(name, p, GAD_ENVIRONMENT)
{
  init_variables(variable_def, LAST_ENVIRONMENTVAR);
}

Gadget* 
EnvironmentGadget::create_instance( const string & s, const Gadget* the_def )
{
  //TODO : make some tests !!!
  if( s == "AtBegining" || s == "MainLoop" || s == "EndCondition" ) {
    Script* build_as_script = (Script*) (the_def);
    Script* new_script = dynamic_cast<Script*> 
      (AllGadgets::create_gadget_by_stringtype( lowercase( gadget_types[ the_def->get_info().type ] ),
						s.c_str(), this));
    (*new_script) = *build_as_script;
    instance[s] = new_script;
  
    my_gadgets.add( instance[s]->get_info() );
    return instance[s];
  }
  return NULL;
}

void
EnvironmentGadget::set_geometry_list( const vector<string>& the_list )
{
  geometry_list = the_list;
}

void
EnvironmentGadget::eval_function( const int fcn )
{
  
}
