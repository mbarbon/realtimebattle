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
// TODO : This class needs to be threadsafe!
// ---------------------------------------------------------------------------

class GuiClientInterface
{
public:
  GuiClientInterface                        () {}
  virtual ~GuiClientInterface               () {}

  virtual const InfoBase* check_information () const = 0;
  virtual void apply_request                ( GuiRequest* req ) = 0;
  virtual void quit_program                 ( int ) = 0;

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
  virtual const unsigned int get_unique_id() const = 0;
  virtual const pthread_t* get_thread_p  () const = 0;
  virtual const int get_reader_id        () const = 0;

  // Gui functions
  virtual const string Name              () const = 0;
  virtual const string UsageMessage      () const = 0;
  virtual int Main                       ( GuiClientInterface* gi ) = 0;

  virtual const bool operator==          ( const unsigned int& ) = 0;
  virtual const bool operator!=          ( const unsigned int& ) = 0;
  virtual const void net_command         ( const string& ) = 0;

private:
};

class GuiInterface : public GuiClientInterface, public GuiServerInterface
{
public:
  // Constructor & Destructor

  GuiInterface( const string&, int, int, char** );
  ~GuiInterface();

  // Start up and Shut down the gui

  void startup                         ();
  void shutdown                        ();

  // Functions that supplies information to the Gui.

  const InfoBase* check_information    () const;

  // Functions that enables the gui to have some influence over the server.

  void apply_request                   ( GuiRequest* req )
  { request_stack.push( req ); }
  void quit_program                    ( int );


  // Functions for the server.

  void process_all_requests            ();
  const unsigned int get_unique_id     () const { return *unique_id; }
  const pthread_t* get_thread_p        () const { return &thread; }
  const int get_reader_id              () const { return information_reader_id; };

  // Gui functions
  const string Name                    () const { return (*func_Name)(); }
  const string UsageMessage            () const { return (*func_UsageMessage)(); }
  int Main                             ( GuiClientInterface* gi )
  { return (*func_Main)( gi ); }

  const void net_command         ( const string& command) {
    func_Command(command);
  }
  const bool operator==                ( const unsigned int& id )
  { return *unique_id == id; }
  const bool operator!=                ( const unsigned int& id )
  { return *unique_id != id; }

private:

  // internal functions

  void* load_symbol                    ( const string& symname );

  // Communication to server
  stack<GuiRequest*> request_stack;

  // internal variables

  unsigned int* unique_id;
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
  void* (*func_Main_pre)( void* );
  void (*func_Command)( string );
};

#endif // __GUIINTERFACE__
