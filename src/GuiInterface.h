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


#include "Options.h"
//#include "ArenaBase.h"
#include "Structs.h"

class ArenaController;
class String;
class Vector2D;


// ---------------------------------------------------------------------------
// class GuiInterface
// ---------------------------------------------------------------------------
// All interactions between the gui and server should go through this class.
// This class needs to be threadsafe!
// ---------------------------------------------------------------------------

class GuiInterface
{
public:
  struct message_t
  {
    message_t( const class String m, const class String f ) : message(m), from(f) {}
    class String message;
    class String from;
  };

  // Constructor & Destructor

  GuiInterface();
  ~GuiInterface();

  // Functions that supplies information to the Gui.

  // The lists shouldn't be supplied directly 
  List<Shape>* get_object_lists               (); 
  List<Robot>* get_all_robots_in_sequence     ();
  List<Robot>* get_all_robots_in_tournament   ();
  String get_current_arena_filename           ();
  int get_game_nr                             ();
  int get_games_per_sequence                  ();
  int get_sequence_nr                         ();
  int get_sequences_in_tournament             ();
  int get_robots_per_game                     ();
  int get_number_of_robots                    ();
  double get_total_time                       ();
  state_t get_state                           ();
  Vector2D* get_boundary                      ();
  enum game_mode_t get_game_mode              ();
  int get_debug_level                         ();
  int get_max_debug_level                     ();

  const double   get_opt_d                    ( option_double_t option );
  const long int get_opt_l                    ( option_long_t   option );
  const String   get_opt_s                    ( option_string_t option );

  // Functions that enables the gui to have some influence over the server.

  void quit                                   ();

  message_t get_message                       ();
  int set_debug_level                         ( const int new_level );
  void pause_game_toggle                      ();
  void step_paused_game                       ();

  void interrupt_tournament                   ();
  void end_game                               ();

  void start_tournament    ( const List<start_tournament_info_t>& robotfilename_list, 
                             const List<start_tournament_info_t>& arenafilename_list, 
                             const int robots_p_game, 
                             const int games_p_sequence, 
                             const int n_o_sequences );

  void set_opt_d                              ( option_double_t option, double   val );
  void set_opt_l                              ( option_long_t   option, long int val );
  void set_opt_s                              ( option_string_t option, String   val );

  // Functions for the server.

  pthread_t* get_thread_p                     ();

  void add_message                            ( String message, String from );

  //TODO: Perhaps an eventlist?


private:

  list<message_t> messages;

  pthread_t thread;
  pthread_mutex_t gi_mutex;
};

// ---------------------------------------------------------------------------
// These functions should be defined by the gui.
// ---------------------------------------------------------------------------

// The name of the gui should be returned as a const String
const String GIName();
// The usage string of the gui should be returned as a const String
const String GIUsageMessage();

// Initialization of gui
bool GIInit( int , char** );
// The main loop of the gui
int  GIMain( GuiInterface* );
// Exit from gui. This function is not needed to be set from gui.
void GIExit( int );


#endif __GUIINTERFACE__
