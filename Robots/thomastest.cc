#include <iostream.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <glib.h>
#include "../messagetypes.h"

#define abs(x) ((x>0) ? (x) : (-x))

volatile double acceleration = 0.0;
volatile double robot_rotate = 0.0;
volatile double cannon_rotate = 0.0, radar_rotate = 0.0;
double tid, speed, radar_angle, cannon_angle, tid0, slumprotate = 1.0 ;
double maxspeed = 10.0;
bool sweep = false, align_cro= true, align_cra = true, enemy = false;

volatile sig_atomic_t exit_robot = false;
static GTimer* timer;

message_to_robot_type
name2msg_to_robot_type(char* msg_name)
{
  for(int i=0; message_to_robot[i].msg[0] != '\0'; i++)
    {
      if( strcmp(message_to_robot[i].msg, msg_name) == 0 )
        return (message_to_robot_type)i;
    }
  return UNKNOWN_MESSAGE_TO_ROBOT;
}

void
check_messages(int sig)
{
  exit_robot = false;

  char msg_name[81];
  char text[81];
  message_to_robot_type msg_t;

  gulong microsecs;
  g_timer_elapsed(timer, &microsecs);
  srand(microsecs);

  cin.clear();
  while( !cin.eof() )
    {
      cin >> msg_name;
      msg_t = name2msg_to_robot_type(msg_name);
      switch(msg_t)
        {
        case INITIALIZE:
          cout << "Name Thomas" << endl;
          cout << "Colour dede11 de5500" << endl;
          break;
        case GAME_STARTS:
          cout << "Print I am ready" << endl;
          //          cout << "Rotate 1 " << robot_rotate << endl;
          cout << "Rotate 1 2" << endl;
          acceleration = 0.4;
          cout << "Acceleration " << acceleration << endl;
          break;
        case GAME_FINISHES:
          cout << "Print The End" << endl;
          break;

        case SAVE_DATA:
          cout << "SaveDataFinished" << endl;
          break;

        case INFO:
          cin >> tid >> speed  >> cannon_angle;
          if(rand() < (RAND_MAX/100))
            {
              slumprotate = 0.0;
              while (abs(slumprotate)<0.5)
                slumprotate = ((double)(rand())/(double)RAND_MAX-0.5)*2.0;
              // cout << "Print Thomas: slumptal = " << slumprotate << endl;
            }
          break;
          
        case RADAR:
          {
            double dist, energy;
            int object;

            cin >> dist >> object >> radar_angle;
            switch(object)
              {
              case ROBOT:
                enemy = true;
                energy = 20.0/(dist*dist+0.2)+0.5;
                robot_rotate = 0.0;
                if (dist<3.0)
                  {
                    if ((speed>1.0)||((dist<1.0)&&(speed>0.01)))
                      {
                        cout << "Break 1" << endl;
                        acceleration = 0.0;
                      }
                    else
                      if (speed<1.0)
                        {
                          acceleration = 0.0;
                        }
                  }
                else
                  {
                    if (speed<maxspeed)
                      acceleration = 2.0;
                    else
                      acceleration = 0.0;
                  }
                cout << "Rotate 1 " << robot_rotate << endl;
                cout << "Shoot " << energy << endl;
                cout << "Acceleration " << acceleration << endl;

                //cout << "Print kill" << endl;
                break;
              case WALL:
                {
                  double old_acc = acceleration;
                  double old_rot = robot_rotate;
                  if (enemy)
                    {
                      enemy = false;
                      slumprotate = -slumprotate;
                    }
                  if( dist < 1.0 )
                    {
                      if (speed > 0.1)
                        {
                          acceleration = 0.0;
                          robot_rotate = 0.0;
                          cout << "Break 1" << endl;
                        }
                      else
                        {
                          robot_rotate = 5.0*slumprotate;
                          acceleration = 0.1;
                        }
                    }
                  else
                    {
                      if (dist<4.0)
                        {
                          if (speed > 2.0)
                            {
                              acceleration = 0.0 ;
                              robot_rotate = 2.0*slumprotate ;
                              cout << "Break 0.7" << endl;
                            }
                          else
                            {
                              acceleration = 0.1;
                              robot_rotate = 1.0*slumprotate;
                            }
                        }
                      else
                        {
                          robot_rotate = 0.3*slumprotate;
                          if (speed<maxspeed)
                            acceleration = 2.0;
                          else
                            acceleration = 0.0;
                        }
                        
                    }
                    cout << "Rotate 1 " << robot_rotate << endl;
                    cout << "Acceleration " << acceleration << endl;
                    
                }
                break;
              case SHOT:
                if (speed < 1.0)
                  {
                    acceleration = 2.0;
                    robot_rotate = 1.0*slumprotate;
                  }
                else
                  robot_rotate = 3.0*slumprotate;
                cout << "Acceleration " << acceleration << endl;
                cout << "Rotate 1 " << robot_rotate << endl;
                //cout << "Print Help me!" << endl;
                //                cout << "Print A shot is approaching!" << endl;
                break;
              case COOKIE:
                robot_rotate = 0.0;
                if (speed<3.0)
                  acceleration = 2.0;
                else
                  acceleration = 0.0;
                cout << "Rotate 1 " << robot_rotate << endl;
                cout << "Acceleration " << acceleration << endl;
                
                //                cout << "Print Yummy! A cookie" << endl;
                break;

              case MINE:
                acceleration = 0.0;
                robot_rotate = 2.0*slumprotate;
                if (speed > 0.4)
                  cout << "Break 1" << endl;
                cout << "Shoot 0.1" << endl;
                cout << "Acceleration " << acceleration << endl;
                cout << "Rotate 1 " << robot_rotate << endl;
                
                //cout << "Print Avoid this mine" << endl;
                break;

              case EXPLOSION:
                cout << "Print Avoid! Explosion" << endl;
                break;
              }
          break;
          }

        case COLLISION:
          {
            int tmp;
            double enegry_diff;
            cin >> tmp >> enegry_diff;
            switch(tmp)
              {
              case ROBOT: 
                acceleration = -0.5;
                cout << "Acceleration " << acceleration << endl; 
                break;
              case SHOT:
                acceleration = 2.0;
                cout << "Acceleration " << acceleration << endl; 
                break;
              case MINE: 
                //cout << "Print Oh no! A mine!" << endl; 
                break;
              case COOKIE: 
                //cout << "Print Cookie eaten!" << endl; 
                break;
              case WALL:
                acceleration = 0.0;
                cout << "Acceleration " << acceleration << endl; 
                break;
              }
            //if( enegry_diff != 0 ) cout << "Print energy changed by " << enegry_diff << endl;
          }
          break;
        case WARNING:
          cin.getline(text,80,'\n');
          cerr << text << endl;
          cerr << "Thomas: Help, I got a yellow card!" << endl;
          break;
        case EXIT_ROBOT:
          cout << "Print Shutting down and leaving" << endl;
          exit_robot = true;
          break;
        default:
          break;
        }
    }
  signal (sig, check_messages);
}

int 
main(int argc, char * argv[])
{
//robot_rotate = 0.53;

  timer = g_timer_new();
  g_timer_reset(timer);
  srand(time(0));
  check_messages(SIGUSR1);

  for(;;sleep(1))
    {
      if( exit_robot ) 
        {
          //          cerr << "Fire: Exiting normally!" << endl;
          return(EXIT_SUCCESS);
        }
    }
  return(EXIT_SUCCESS);
}


