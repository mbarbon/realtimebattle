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

#ifndef __ENVIRONMENT_GADGET__
#define __ENVIRONMENT_GADGET__

#include <limits.h>
#include <float.h>
#include <vector>

#include "Gadget.h"
#include "Variable.h"
#include "Function.h"
#include "Arena.h"
#include "EventHandler.h"

class EnvironmentGadget : public Gadget
{
public:

  EnvironmentGadget( const char* name, Gadget* const p );

  static Gadget* new_environmentgadget( const char* name, Gadget* const p )
  {
    return new EnvironmentGadget( name, p );
  }


  enum EnvironmentVars
  {
    FRICTION = 0, AIRRESISTANCE, GRAVITY, VISIBILITY, TEMPERATURE,
    MATCHTIMELIMIT, TIMELIMIT, FIXEDLENGTH, TIMEOUT,
    LAST_ENVIRONMENTVAR
  };

  Gadget* create_instance( const string & s, const Gadget*  );

  void set_geometry_list( const vector<string> & );

private:
  
  vector<string> geometry_list;

  const static VariableDefinition variable_def[LAST_ENVIRONMENTVAR]; 

  void eval_function(const int fcn);
};


#endif // __ENVIRONMENT_GADGET__