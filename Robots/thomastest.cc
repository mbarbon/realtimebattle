#include <iostream.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include "../messagetypes.h"

volatile double acceleration = 0.0;
volatile double robot_rotate = 0.0;
volatile sig_atomic_t exit_robot = false;

message_to_robot_type
name2msg_to_robot_type(char* msg_name)
{
  for(int i=0; message_from_robot[i].msg[0] != '\0'; i++)
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

  srand(time(0));

  //  if(rand() < (RAND_MAX/40))
  //    {
  //      robot_rotate = - robot_rotate;
  //      cout << "Rotate 1 " << robot_rotate << endl;
  //    }

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
          cout << "Rotate 7 2" << endl;
          acceleration = 0.4;
          cout << "Acceleration " << acceleration << endl;
          break;
        case RADAR:
          {
            double dist, energy;
            int object;

            cin >> dist >> object;
            switch(object)
              {
              case ROBOT:
                energy = 20.0/(dist*dist+0.2);
                if (dist<3.0)
                  {
                    robot_rotate = 0.0;
                    acceleration = acceleration/2.0;
                  }
                else
                  {
                  acceleration = 2.0;
                  robot_rotate = 0.0;
                  }
                cout << "Acceleration " << acceleration << endl;
                cout << "Rotate 7 " << robot_rotate << endl;
                cout << "Shoot " << energy << endl;

                //cout << "Print kill" << endl;
                break;
              case WALL:
                {
                  double old_acc = acceleration;
                  double old_rot = robot_rotate;

                  acceleration = old_acc>?0.5;
                  robot_rotate = old_rot<?0.7;
                  if( dist < 1.0 )
                    {
                    acceleration = -0.5 ;
                    robot_rotate = 0.0;
                    }
                  else
                    {
                      if (dist<4.0)
                        {
                        acceleration = 0.0 ;
                        robot_rotate = 3.0 ;
                        }
                    }
                    cout << "Acceleration " << acceleration << endl;
                    cout << "Rotate 7 " << robot_rotate << endl;
                    
                }
                break;
              case SHOT:
                acceleration = 2.0;
                robot_rotate = 1.0;
                cout << "Acceleration " << acceleration << endl;
                cout << "Rotate 7 " << robot_rotate << endl;
                //cout << "Print Help me!" << endl;
                //cout << "Print A shot is approaching!" << endl;
                break;
              case COOKIE:
                cout << "Print Yummy! A cookie" << endl;
                break;
              case MINE:
                cout << "Print Avoid this mine" << endl;
                break;
              case EXPLOSION:
                cout << "Print Avoid! Explosion" << endl;
                break;
              default:
                break;
              }
            //                cout << "Print " << dist << " " << object << endl;
          }
          break;
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
              case MINE: cout << "Print Oh no! A mine!" << endl; break;
              case COOKIE: cout << "Print Cookie eaten!" << endl; break;
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
  acceleration = acceleration*1.001<?2.0;
  signal (sig, check_messages);
}

int 
main(int argc, char * argv[])
{
  robot_rotate = 0.53;

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


