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

#ifndef __SCRIPT_GADGET__
#define __SCRIPT_GADGET__

#include <vector>
#include <string>

#include "Gadget.h"
#include "Variable.h"

class Function;

struct ScriptLine
{
  string name;
  double value;
};


class Script : public Gadget
{
public:
  
  Script() {}
  ~Script() {}

  void run();
  void continue_script();
  

  

private:

  vector<ScriptLine> commands;
  
  Variable* compare_var;
  
  bool last_test_result;  // the result of the latest comparison
  

  int current_line_number;
  
  bool pausing;
};





#endif __SCRIPT_GADGET__
