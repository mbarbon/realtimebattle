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

#ifndef __ALLGADGETS__
#define __ALLGADGETS__


#include "Gadget.h"
#include "GadgetSet.h"

#include "Function.h"
#include "Variable.h"
#include "Script.h"

#include "ArenaGadget.h"
#include "CommunicationGadget.h"
#include "DefenseGadget.h"
#include "EngineGadget.h"
#include "ExplosionGadget.h"
#include "RobotBodyGadget.h"
#include "ShotGadget.h"
#include "WeaponGadget.h"

#include "Messagetypes.h"

/** Class with static only members that contains various functions and
 *  information that deals with a gadget in some way and does not
 *  fit into the ordinary gadgetclasses.
 *  Do not create an object of this class.
 *  TODO: If we decide to use Namespaces this should be a namespace instead. */
class AllGadgets
{
public:
  /** Create a gadget that matches the gadget_type supplied.
   *  @see    gadget_t
   *  @param  type The type of Gadget to create, in form of a gadget_t.
   *  @param  name The name to be given to this gadget.
   *  @param  p    The parent of the gadget.
   *  @return A newly allocated Gadget-pointer of the specified type. */
  static Gadget* create_gadget_by_gadget_type ( const gadget_t    type,
                                                const char*       name,
                                                Gadget* const     p );
  /** Create a gadget that matches the string gadgettype supplied.
   *  @see    gadget_types
   *  @param  type The type of Gadget to create, in form of a string.
   *  @param  name The name to be given to this gadget.
   *  @param  p    The parent of the gadget.
   *  @return A newly allocated Gadget-pointer of the specified type. */
  static Gadget* create_gadget_by_stringtype  ( const string&     type,
                                                const char*       name,
                                                Gadget* const     p );
};

#endif  __ALLGADGETS__
