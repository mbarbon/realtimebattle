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

#ifndef __GUIINTERFACE__
#define __GUIINTERFACE__

#include <pthread.h>
#include <list>
#include <string>

#include "InfoClasses.h"

// ---------------------------------------------------------------------------
// class GuiInterface
// ---------------------------------------------------------------------------
// All interactions between the gui and server should go through this class.
// This class needs to be threadsafe!
// ---------------------------------------------------------------------------

class GuiInterface
{
public:
  // Constructor & Destructor

  GuiInterface( const string&, pthread_mutex_t*, int, char** );
  ~GuiInterface();

  // Start up and Shut down the gui

  void startup                         ();
  void shutdown                        ();

  // Functions that supplies information to the Gui.

  // Don't know what functions is needed here yet!

  // Functions that enables the gui to have some influence over the server.

  void quit                            ( bool exit_program = false );

  const InfoBase* get_information      ();

  // Functions for the server.

  pthread_t* get_thread_p              ();

  // Gui functions
  const string Name                    () const { return (*func_Name)(); }
  const string UsageMessage            () const { return (*func_UsageMessage)(); }
  int Main                             ( GuiInterface* gi )
  { return (*func_Main)( gi ); }

private:

  // internal functions

  void* load_symbol                    ( const string& symname );

  // internal variables

  long int unique_id;
  int information_reader_id;

  string plain_name;
  string library_name;

  pthread_t thread;

  // dl_handle
  void* dl_handle;

  // Gui functions stored
  const string (*func_Name)();
  const string (*func_UsageMessage)();
  bool (*func_Init)( int, char** );
  int (*func_Main)( GuiInterface* );

  // Mutex
  pthread_mutex_t* mutex_p;
};

// ---------------------------------------------------------------------------
// These functions should be defined by the gui.
// ---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// The name of the gui should be returned as a const string
const string GIName();
// The usage string of the gui should be returned as a const string
const string GIUsageMessage();

// Initialization of gui
bool GIInit( int , char** );
// The main loop of the gui
int  GIMain( GuiInterface* );
// Exit from gui. This function should not needed to be set from gui.
void GIExit( int );

//Internal GI functions
void* GIMain_pre( void* arg );

#ifdef __cplusplus
}
#endif

#endif __GUIINTERFACE__
