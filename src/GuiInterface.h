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
#include <stack>

#include "GuiRequest.h"
#include "InfoClasses.h"

// ---------------------------------------------------------------------------
// class GuiInterface and it's associated classes
// ---------------------------------------------------------------------------
// All interactions between the gui and server should go through this class.
// This class needs to be threadsafe!
// ---------------------------------------------------------------------------

class GuiClientInterface
{
public:
  GuiClientInterface                        () {}
  virtual ~GuiClientInterface               () {}

  virtual const InfoBase* check_information () const = 0;
  virtual void apply_request                ( GuiRequest* req ) = 0;
  virtual void quit                         ( bool exit_program = false ) = 0;

private:
};

class GuiServerInterface
{
public:
  GuiServerInterface                     () {}
  virtual ~GuiServerInterface            () {}

  virtual void startup                   () = 0;
  virtual void shutdown                  () = 0;

  virtual void process_all_requests      () = 0;
  virtual const pthread_t* get_thread_p  () const = 0;

  // Gui functions
  virtual const string Name              () const = 0;
  virtual const string UsageMessage      () const = 0;
  virtual int Main                       ( GuiClientInterface* gi ) = 0;
private:
};

class GuiInterface : public GuiClientInterface, public GuiServerInterface
{
public:
  // Constructor & Destructor

  GuiInterface( const string&, pthread_mutex_t*, int, char** );
  ~GuiInterface();

  // Start up and Shut down the gui

  void startup                         ();
  void shutdown                        ();

  // Functions that supplies information to the Gui.

  const InfoBase* check_information    () const;

  // Functions that enables the gui to have some influence over the server.

  void apply_request                   ( GuiRequest* req )
  { request_stack.push( req ); }
  void quit                            ( bool exit_program = false );


  // Functions for the server.

  void process_all_requests            ();
  const pthread_t* get_thread_p        () const { return &thread; }

  // Gui functions
  const string Name                    () const { return (*func_Name)(); }
  const string UsageMessage            () const { return (*func_UsageMessage)(); }
  int Main                             ( GuiClientInterface* gi )
  { return (*func_Main)( gi ); }

private:

  // internal functions

  void* load_symbol                    ( const string& symname );

  // Communication to server
  stack<GuiRequest*> request_stack;

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
  int (*func_Main)( GuiClientInterface* );

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
int  GIMain( GuiClientInterface* );
// Exit from gui. This function should not needed to be set from gui.
void GIExit( int );

//Internal GI functions
void* GIMain_pre( void* arg );

#ifdef __cplusplus
}
#endif

#endif __GUIINTERFACE__
