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

/*Class Diagram :
    -------------------             ------------------
    |   Event         |             |  EventHandler  |
    -------------------             ------------------
    |eval()           | n         1 |main_loop()     |
    |operator<        |-------------|insert_event()  |
    -------------------             |get_time()      |
    |eval_time:double |             |quit()          |
    |my_events_handler|             ------------------
    -------------------             |                |
             |                      ------------------
	     |
        ____/_\___________________________________________________
	|                                                        |
  ----------------                                          ---------------
  |GameTimeEvent |                                          |RealTimeEvent|
  ----------------                                          ---------------
  |              |                                          |             |
  ----------------                                          ---------------
  |              |                                          |             |
  ----------------                                          ---------------
          |                                                         |
       __/_\________________________________________                |
       |                                           |         ______/_\_____
  ----------------                         --------------    |CheckGUI    |
  |Recursive     |                         | UniqueExec |    -------------- 
  ----------------                         --------------    |            |
  |Add_Next()    |                         |            |    --------------
  ----------------                         --------------    |            |
  |Refresh : int |                         |            |    --------------
  ----------------                         --------------
          |                                        |
          |                                        |
       __/_\_________________                      |
       |                    |                      |
  _____|_________     ______|_______       _______/_\_______ 
  |NewMine/Cook.|     |RobotUpDate |       |  QuitEvent    |
  ---------------     --------------       -----------------
  |             |     |            |       |               |
  ---------------     --------------       -----------------
  |             |     |Which:Robot |       |               |
  ---------------     --------------       -----------------


class Event;

class GTEvent : public Event;
class RTEvent : public Event;

class NewMine/Cook. : public GTEvent;
class RobotsUpDate  : public GTEvent;

class QuitEvent : public GTEvent;

*/

//TODO : Need some Wait Event classes to run a function only when an event is generated !!!

#ifndef __EVENT_HANDLER__
#define __EVENT_HANDLER__

#include <queue>

#include "Event.h"
#include "Timer.h"

class EventHandler;
extern EventHandler the_eventhandler;

class Tournament;

class EventHandler
{
 public:

  EventHandler();
  ~EventHandler();
  
  void main_loop();
  
  void insert_RT_event (Event* ev, bool absolute_time=false);
  void insert_GT_event (Event* ev, bool absolute_time=false);
  
  double get_time() { return current_time; }
  double get_game_time() { return current_game_time; }

  void pause_game() ;  //Pause/unPause the game
  void set_game_speed( const double speed );

  void quit();  
  void finish_game();
  
  void set_tournament( Tournament* const t );

 private:
  
  typedef const Event* ep;
  typedef priority_queue<ep, vector<ep>, lt_event>  EventQueue;

  EventQueue RT_event_queue;
  EventQueue GT_event_queue;

  
  Timer timer;
  double current_time;
  double current_game_time;

  double game_speed_before_pause;

  bool finished;

  Tournament* the_tournament;

  int nb_RT_event;
  int nb_GT_event;
};

#endif // __EVENT_HANDLER__

