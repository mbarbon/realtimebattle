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

#include <string>

#include "GuiFunctions.h"
#include "IntlDefs.h"

const string
GIName()
{
  static const string name(_("No graphics at all"));
  return name;
}

const string
GIUsageMessage()
{
  static const string usage(_("No options"));
  return usage;
}

bool
GIInit( int argc, char** argv )
{
  return true;
}

int
GIMain( GuiClientInterface* _gi_p )
{
  return true;
}
