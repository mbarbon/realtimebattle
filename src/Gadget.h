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

#ifndef __GADGET__
#define __GADGET__

#include <string>
#include <list>

#include "GadgetSet.h"

class Variable;
class Function;

class Gadget
{
public:

  Gadget( const char* name, Gadget* const p ) 
    : info(this, last_id_used++, name), parent(p) {}

  ~Gadget() {}

  long int get_unique_id() { return info.id; }
  string& get_name() { return info.name; }

  const GadgetInfo& get_info() { return info; }

  static void set_last_id_used() { last_id_used = 0; }
  
  void eval_function(const int fcn) {}

protected:

  GadgetInfo info;


  Gadget* parent;

  // Pointer to the gadget in the GadgetDefinition hierarchy which defined
  // this gadget
  //  Gadget* defining_gadget;

  GadgetSet my_gadgets;

public:
  static int last_id_used;
};

#endif __GADGET__
