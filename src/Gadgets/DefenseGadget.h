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

#ifndef __DEFENSE_GADGET__
#define __DEFENSE_GADGET__

#include <limits.h>
#include <float.h>

#include "Gadget.h"
#include "Variable.h"
#include "Function.h"


class DefenseGadget : public Gadget
{
public:

  DefenseGadget( const char* name, Gadget* const p );

  enum DefenseVars
  {
    COLLISIONPROTECTION=0, HEATPROTECTION, SHOTPROTECTION, FRONTANGLE,
    MOUNTTIME, UNMOUNTTIME, MOUNTCOST, UNMOUNTCOST, USCOSTPERTIME,
    ATTACHABLE, COLOUR, PRICE, MASS, LAST_DEFENSEVAR
  };


private:

  const static VariableDefinition variable_def[LAST_DEFENSEVAR]; 



  enum DefenseFcns
  {
    LAST_DEFENSEFCN
  };

  const static FunctionDefinition function_def[LAST_DEFENSEFCN];


  void eval_function(const int fcn);
};


#endif __DEFENSE_GADGET__
