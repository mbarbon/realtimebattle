/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2000  Erik Ouchterlony and Ragnar Ouchterlony

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

#include "CommunicationGadget.h"
#include "Variable.h"

const VariableDefinition
CommunicationGadget::variable_def[CommunicationGadget::LAST_COMMUNICATIONVAR] =
{
  {"Bandwidth", INT_V, 0, 0, INT_MAX, 0, false, true, false},
  {"Range", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false},
  {"Directional", BOOL_V, false, 0, 0, false, true, false},
  {"LeftAngle", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, 0, false, true, false},
  {"RightAngle", DOUBLE_V, 0, -DBL_MAX, DBL_MAX, 0, false, true, false},
  {"AreaLimit", DOUBLE_V, 0, 0, DBL_MAX, 0, false, true, false}
};

CommunicationGadget::CommunicationGadget( const char* name, Gadget* const p )
  : Gadget(name, p, GAD_COMMUNICATION)
{
  init_variables( variable_def, LAST_COMMUNICATIONVAR );
  // Is there any need for this to be here?
  //init_functions( function_def, LAST_COMMUNICATIONFCN ); 
}

// Sorry, no functions to be evaluated.
void
CommunicationGadget::eval_function( const int fcn )
{
}
