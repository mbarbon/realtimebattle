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

#ifndef __GUIFUNCTIONS_H__
#define __GUIFUNCTIONS_H__

#include "GuiInterface.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------
// These functions should be called from the server.
// All except GIExit should be defined within the gui.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// The name of the gui should be returned as a const string
// ---------------------------------------------------------------------------
const string GIName();

// ---------------------------------------------------------------------------
// The usage string of the gui should be returned as a const string
// ---------------------------------------------------------------------------
const string GIUsageMessage();

// ---------------------------------------------------------------------------
// Initialization of gui
// ---------------------------------------------------------------------------
bool GIInit( int , char** );

// ---------------------------------------------------------------------------
// The main loop of the gui
// ---------------------------------------------------------------------------
int  GIMain( GuiClientInterface* );

// ---------------------------------------------------------------------------
// GIExit should be called from the gui to stop itself.
// This function should not be set from gui.
// ---------------------------------------------------------------------------
void GIExit( int );


// ---------------------------------------------------------------------------
// Internal things not to be changed from the gui.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// This variable is being set to the unique id assigned to this gui.
// ---------------------------------------------------------------------------
#include <pthread.h>
#include <stack>

extern list<unsigned int> GI_exit_list;
extern pthread_mutex_t the_mutex;
unsigned int GI_unique_id;

// ---------------------------------------------------------------------------
// GIMain_pre function is the first function to be called when the
// new thread starts.
// ---------------------------------------------------------------------------
void*
GIMain_pre( void* arg )
{
  int res = ((GuiInterface*) arg)->Main( (GuiClientInterface*) arg );
  GIExit( res );
  return NULL;
}


// ---------------------------------------------------------------------------
// GIExit function
// ---------------------------------------------------------------------------
// TODO: GIExit should maybe do something more
void
GIExit( int returncode )
{
  int* returncode_p = new int(returncode);
  pthread_mutex_lock( &the_mutex );
  GI_exit_list.push_back(GI_unique_id);
  pthread_mutex_unlock( &the_mutex );
  pthread_exit(returncode_p);
}

#ifdef __cplusplus
}
#endif

#endif __GUIFUNCTIONS_H__
