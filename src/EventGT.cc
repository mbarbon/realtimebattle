#include "Event.h"
#include "EventGT.h"

    ///////////////////////////////////////////////////
   //                                               //
  //             RobotsUpdateEvent                  //
 //                                               //
///////////////////////////////////////////////////
  
void RobotsUpdateEvent::eval() const 
{
  //      my_arena->update_robots();
  NextEvent();
}

void RobotsUpdateEvent::NextEvent() const
{
  if((NoMoreThan != 0)&&((eval_time+refresh < NoMoreThan)))
    {//Add the next event if the game isn't finished...
      cout<<"Next in "<<refresh<<endl;
      Event * NextUpDate = new RobotsUpdateEvent(eval_time + refresh, refresh, NoMoreThan, my_arena, my_event_handler);
      my_event_handler->insert_event(NextUpDate);
    }
  else
    {
      //cout<<eval_time+refresh<<"<"<<NoMoreThan<<" : This was the last one...\n";
    }
}



    ///////////////////////////////////////////////////
   //                                               //
  //             ShotsUpdateEvent                  //
 //                                               //
///////////////////////////////////////////////////

void ShotsUpdateEvent::eval() const 
{ 
  //      my_arena->update_shots();
  NextEvent();
}
 
void ShotsUpdateEvent::NextEvent() const
{   
  if((NoMoreThan != 0)&&((eval_time+refresh < NoMoreThan)))
    { //Add the next event if the game isn't finished...
      Event * NextUpDate = new ShotsUpdateEvent(eval_time + refresh, refresh, NoMoreThan, my_arena, my_event_handler);
      my_event_handler->insert_event(NextUpDate);
    }
}


    ///////////////////////////////////////////////////
   //                                               //
  //             ShotsUpdateEvent                  //
 //                                               //
///////////////////////////////////////////////////
ContinueScriptEvent::eval() const
{
  //Can't remember what was coming here...
}

