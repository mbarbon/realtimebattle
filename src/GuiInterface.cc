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

#include <pthread.h>
#include <dlfcn.h>
#include <string>

#include "GuiInterface.h"
#include "ArenaBase.h"
#include "ArenaController.h"
#include "Various.h"


GuiInterface::GuiInterface( const string& name, pthread_mutex_t* _mutex_p,
                            int argc, char** argv )
{
  mutex_p = _mutex_p;
  
  string libfile = "libRTBGui_" + name + ".so";
  dl_handle = dlopen( libfile.c_str(), DLOPEN_POLICY );
  if( dl_handle == NULL )
    Error( true, "Couldn't open gui " + name + ": " + string( dlerror() ),
           "GuiInterface::GuiInterface" );

  const char* error;
  // GIName
  func_Name = (const string (*)())dlsym(dl_handle, "GIName");
  if ((error = dlerror()) != NULL)
  {
    func_Name = (const string (*)())dlsym(dl_handle, "_GIName");
    if ((error = dlerror()) != NULL)
    {
      Error( true, "Failed to find GIName() for gui " + name + ": " + string(error),
             "GuiInterface::GuiInterface" );
    }
  }
  // GIUsageMessage
  func_UsageMessage = (const string (*)())dlsym(dl_handle, "GIUsageMessage");
  if ((error = dlerror()) != NULL)
  {
    func_UsageMessage = (const string (*)())dlsym(dl_handle, "_GIUsageMessage");
    if ((error = dlerror()) != NULL)
    {
      Error( true,
             "Failed to find GIUsageMessage() for gui " + name + ": " + string(error),
             "GuiInterface::GuiInterface" );
    }
  }
  // GIInit
  func_Init = (bool (*)( int, char** ))dlsym(dl_handle, "GIInit");
  if ((error = dlerror()) != NULL)
  {
    func_Init = (bool (*)( int, char** ))dlsym(dl_handle, "_GIInit");
    if ((error = dlerror()) != NULL)
    {
      Error( true, "Failed to find GIInit() for gui " + name + ": " + string(error),
             "GuiInterface::GuiInterface" );
    }
  }
  // GIMain
  func_Main = (int (*)( GuiInterface* ))dlsym(dl_handle, "GIMain");
  if ((error = dlerror()) != NULL)
  {
    func_Main = (int (*)( GuiInterface* ))dlsym(dl_handle, "_GIMain");
    if ((error = dlerror()) != NULL)
    {
      Error( true, "Failed to find GIMain() for gui " + name + ": " + string(error),
             "GuiInterface::GuiInterface" );
    }
  }

  if(!(*func_Init)( argc, argv ))
    Error( true, "Couldn't initialize gui " + name,
             "GuiInterface::GuiInterface" );
}

GuiInterface::~GuiInterface()
{
  dlclose( dl_handle );
  //TODO: thread should be joined or whatever is appropriate
}

void
GuiInterface::startup()
{
  pthread_create(&thread, NULL, GIMain_pre, (void*)this);
}

void
GuiInterface::shutdown()
{
  void* ret_val;
  pthread_join( thread, &ret_val );
  // TODO: return something useful
}

int
GuiInterface::get_game_nr()
{
  pthread_mutex_lock( mutex_p );
  int res = the_arena.get_game_nr();
  pthread_mutex_unlock( mutex_p );
  return res;
}

// PreMain function
void*
GIMain_pre( void* arg )
{
  int res = ((GuiInterface*) arg)->Main( (GuiInterface*) arg );
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
