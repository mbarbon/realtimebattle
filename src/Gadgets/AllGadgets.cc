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

#include <map>

#include "AllGadgets.h"
#include "String.h"

typedef Gadget* (*NewGadgetFunction)( const char*, Gadget* const );

map< string, NewGadgetFunction > new_gadget_functions;

void
init_new_gadget_functions()
{
  new_gadget_functions.clear();

  new_gadget_functions[ lowercase( gadget_types[GAD_COMMUNICATION] ) ] =
    CommunicationGadget::new_communicationgadget;
  new_gadget_functions[ lowercase( gadget_types[GAD_DEFENSE] ) ] =
    DefenseGadget::new_defensegadget;
  new_gadget_functions[ lowercase( gadget_types[GAD_ENGINE] ) ] =
    EngineGadget::new_enginegadget;
  new_gadget_functions[ lowercase( gadget_types[GAD_EXPLOSION] ) ] =
    ExplosionGadget::new_explosiongadget;
  new_gadget_functions[ lowercase( gadget_types[GAD_FUNCTION] ) ] =
    Function::new_function;
  new_gadget_functions[ lowercase( gadget_types[GAD_ROBOTBODY] ) ] =
    RobotBodyGadget::new_robotbodygadget;
  new_gadget_functions[ lowercase( gadget_types[GAD_SCRIPT] ) ] =
    Script::new_script;
  new_gadget_functions[ lowercase( gadget_types[GAD_SHOT] ) ] =
    ShotGadget::new_shotgadget;
  new_gadget_functions[ lowercase( gadget_types[GAD_VARIABLE] ) ] =
    Variable::new_variable;
  new_gadget_functions[ lowercase( gadget_types[GAD_WEAPON] ) ] =
    WeaponGadget::new_weapongadget;
}

Gadget*
AllGadgets::create_gadget_by_gadget_type( const gadget_t    type,
                                          const char*       name,
                                          Gadget* const     p )
{
  return create_gadget_by_stringtype( gadget_types[type], name, p );
}

Gadget*
AllGadgets::create_gadget_by_stringtype( const string&     type,
                                         const char*       name,
                                         Gadget* const     p )
{
  if( new_gadget_functions.empty() )
    init_new_gadget_functions();

  map< string, NewGadgetFunction >::const_iterator ci;
  if( (ci = new_gadget_functions.find( lowercase( type ) ))
      != new_gadget_functions.end() )
    return (*(ci->second))( name, p );

  return 0;
}
