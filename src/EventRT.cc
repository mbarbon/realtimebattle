#include "EventRT.h"


    ///////////////////////////////////////////////////
   //                                               //
  //             CheckGUIEvent                     //
 //                                               //
///////////////////////////////////////////////////


void CheckGUIEvent::eval() const 
{
  // Note: might have to supply process_all_options() with eventhandler in the future.
  gui_p->process_all_options();
  NextEvent();
}

void CheckGUIEvent::NextEvent() const
{
  if((NoMoreThan != 0)&&((eval_time+refresh < NoMoreThan)))
    { //Add the next event if the game isn't finished...
      Event * NextUpDate = new CheckGUIEvent(eval_time + refresh, refresh,
                                             NoMoreThan, my_event_handler);
      my_event_handler->insert_RT_event(NextUpDate);
    }
}

