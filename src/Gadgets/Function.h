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

#ifndef __FUNCTION_GADGET__
#define __FUNCTION_GADGET__

struct FunctionDefinition
{
  char* name;
  bool enabled;
};


class Function : public Gadget
{
public:
  Function() {}
  Function(const char* name, Gadget* const p ) : Gadget(name, p) {}
  Function(const char* name, Gadget* const p, const int fcn ) 
    : Gadget(name, p), fcn_nr(fcn) {}

  ~Function() {}

  void operator() () { parent->eval_function(fcn_nr); }

  void set(const int fcn) { fcn_nr = fcn; }

private:

  int fcn_nr;

};


#endif __FUNCTION_GADGET__
