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

#include <pthread.h>

#include "GuiInterface.h"
#include "ArenaBase.h"
#include "ArenaController.h"

GuiInterface::GuiInterface()
{
  pthread_create(&thread, NULL, GIMain_pre, (void*)this);
  pthread_mutex_init( &gi_mutex, NULL );
}

GuiInterface::~GuiInterface()
{
  pthread_mutex_destroy( &gi_mutex );
  //TODO: thread should be joined or whatever is appropriate
}

int
GuiInterface::get_game_nr()
{
  pthread_mutex_lock( &gi_mutex );
  int res = the_arena.get_game_nr();
  pthread_mutex_unlock( &gi_mutex );
  return res;
}

// PreMain function
void*
GIMain_pre( void* arg )
{
  int res = GIMain( (GuiInterface*) arg );
  GIExit( res );
  return NULL;
}

// TODO: GIExit should maybe do something more
void
GIExit( int returncode )
{
  int* returncode_p = new int(returncode);
  pthread_exit(returncode_p);
}
