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
  |             |     |Witch:Robot |       |               |
  ---------------     --------------       -----------------


class Event;

class GTEvent : public Event;
class RTEvent : public Event;

class NewMine/Cook. : public GTEvent;
class RobotsUpDate  : public GTEvent;

class QuitEvent : public GTEvent;

*/
#ifndef __EVENT_HANDLER__
#define __EVENT_HANDLER__

#include <queue>

#include "Event.h"
#include "Timer.h"

class EventHandler
{

 public:

  EventHandler();
  ~EventHandler();
  
  void main_loop();
  
  void insert_event( Event* ev );    //same as GT, but still here for compatibility
  void insert_RT_event (Event* ev);
  void insert_GT_event (Event* ev);
  
  double get_time();
  void pauseGame() ;  //Pause/unPause the game
  void quit();
  
  //private:
  
  //friend class QuitEvent;
  
  void finish() { finished = true; };
  
 private:
  priority_queue<const Event*> RT_event_queue;
  priority_queue<const Event*> GT_event_queue;

  
  Timer timer;
  double current_time;

  Timer pauseStarted;   //When did the pause will start
  double pausedTime;    //Time of pause since the begining of the game...
  bool paused;    //For the moment, this is the only way to alter the gametime in a normal game... so...

  bool finished;
};


#include "EventRT.h"
#include "EventGT.h"

#endif __EVENT_HANDLER__






