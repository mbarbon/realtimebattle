/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2001  Erik Ouchterlony and Ragnar Ouchterlony

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

#ifndef __WALL_GADGET__
#define __WALL_GADGET__

#include <limits.h>
#include <float.h>

#include "Gadget.h"
#include "Variable.h"
#include "Function.h"
#include "ShotGadget.h"
#include "../Arena.h"
#include "../EventHandler.h"
#include "Script.h"

class WallGadget : public Gadget
{
public:

  WallGadget( const char* name, Gadget* const p );

  static Gadget* new_wallgadget( const char* name, Gadget* const p )
  {
    return new WallGadget( name, p );
  }


  enum WallVars
  {
    COLOUR=0, SIZE, TEMPERATURE, LAST_WALLVAR
  };

  Gadget* create_instance( const string & s, const Gadget*  );

private:

  const static VariableDefinition variable_def[LAST_WALLVAR]; 

  void eval_function(const int fcn);
  
  Script*    ActionAtCollision;
  Script*    ActionWhenHit;
};


#endif // __WALL_GADGET__
