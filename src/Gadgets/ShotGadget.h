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

#ifndef __SHOT_GADGET__
#define __SHOT_GADGET__

#include <limits.h>
#include <float.h>

#include "Gadget.h"
#include "Variable.h"
#include "Function.h"


class ShotGadget : public Gadget
{
public:

  ShotGadget( const char* name, Gadget* const p );

  static Gadget* new_shotgadget( const char* name, Gadget* const p )
  {
    return new ShotGadget( name, p );
  }

  enum ShotVars
  {
    DAMAGE=0, SPEED, SIZE, COSTPERDAMAGE, COSTPERSPEED,
    MAX_DAMAGE, MIN_DAMAGE, WEAPONHEATPERDAMAGE, TEMPERATURE,
    MAXTIME, TARGETSEEKING, MAXANGULARVELOCITY, COLOUR,
    PRICE, MASS, LAST_SHOTVAR
  };

  const ShotGadget& operator= (const ShotGadget& v) ;

private:

  const static VariableDefinition variable_def[LAST_SHOTVAR]; 



  enum ShotFcns
  {
    LAST_SHOTFCN
  };

  const static FunctionDefinition function_def[LAST_SHOTFCN];


  void eval_function(const int fcn);
  
  // ExplosionGadget* explosion_when_colliding;
  // ExplosionGadget* explosion_when_hit;
  // ExplosionGadget* explosion_at_max_time;

};


#endif // __SHOT_GADGET__
