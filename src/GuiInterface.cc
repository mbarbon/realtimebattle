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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <pthread.h>
#include <dlfcn.h>
#include <string>

#include "GuiInterface.h"
//#include "ArenaBase.h"
#include "ArenaController.h"
#include "Various.h"
#include "InformationDistributor.h"

extern class ArenaController the_arena_controller;
extern pthread_mutex_t the_mutex;

GuiInterface::GuiInterface( const string& name, int _id, int argc, char** argv )
{
  plain_name = name;
  library_name = "libRTBGui_" + name + ".so";

  dl_handle = dlopen( library_name.c_str(), DLOPEN_POLICY );
  if( dl_handle == NULL )
    Error( true, "Couldn't open gui " + name + ": " + string( dlerror() ),
           "GuiInterface::GuiInterface" );

  func_Name = (const string (*)())load_symbol( "GIName" );
  func_UsageMessage = (const string (*)())load_symbol( "GIUsageMessage" );
  func_Init = (bool (*)( int, char** ))load_symbol( "GIInit" );
  func_Main = (int (*)( GuiClientInterface* ))load_symbol( "GIMain" );
  func_Main_pre = (void* (*)( void* ))load_symbol( "GIMain_pre" );
  unique_id = (unsigned int*)load_symbol( "GI_unique_id" );

  if(!(*func_Init)( argc, argv ))
    Error( true, "Couldn't initialize gui " + name,
             "GuiInterface::GuiInterface" );

  information_reader_id = the_arena_controller.get_distributor()->add_reader();
}

// Loads a symbol into memory.
void*
GuiInterface::load_symbol( const string& symname )
{
  const char* error;
  void* symbol;
  symbol = dlsym( dl_handle, symname.c_str() );
  if( (error = dlerror()) != NULL )
    {
      symbol = dlsym( dl_handle, ("_" + symname).c_str() );
      if( (error = dlerror()) != NULL )
          Error( true, "Failed to find " + symname + " for gui " + plain_name +
                 ": " + string(error), "GuiInterface::load_symbol" );
    }
  return symbol;
}

GuiInterface::~GuiInterface()
{
  dlclose( dl_handle );
  //TODO: thread should be joined or whatever is appropriate
}

void
GuiInterface::startup()
{
  pthread_create(&thread, NULL, func_Main_pre, (void*)this);
}

void
GuiInterface::shutdown()
{
  void* ret_val;
  pthread_join( thread, &ret_val );
  // TODO: return something useful
}

//TODO: A Better way to end the gui.
void
GuiInterface::quit_program( int _success )
{
  apply_request( new QuitProgramRequest( _success ) );
}

// TODO: Make sure that the event is not deleted before the gui has used it!
const InfoBase*
GuiInterface::check_information() const
{
  pthread_mutex_lock( &the_mutex );
  const InfoBase* info =
    the_arena_controller.get_distributor()->get_information( information_reader_id );
  pthread_mutex_unlock( &the_mutex );
  return info;
}

void
GuiInterface::process_all_requests()
{
  while( !request_stack.empty() )
    {
      request_stack.top()->accept();
      delete request_stack.top();
      request_stack.pop();
    }
}

