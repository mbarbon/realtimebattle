#include "EventRT.h"


    ///////////////////////////////////////////////////
   //                                               //
  //             CheckGUIEvent                     //
 //                                               //
///////////////////////////////////////////////////


void CheckGUIEvent::eval() const 
{
  //This is your go Ragnar :)
  cout<<"Evaluating the GUI Event\n";
  NextEvent();
}

void CheckGUIEvent::NextEvent() const
{
  if((NoMoreThan != 0)&&((eval_time+refresh < NoMoreThan)))
    { //Add the next event if the game isn't finished...
      Event * NextUpDate = new CheckGUIEvent(eval_time + refresh, refresh, NoMoreThan, my_event_handler);
      my_event_handler->insert_RT_event(NextUpDate);
    }
}
