#include <iostream.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include "../messagetypes.h"

static const number_of_object_types = 6;

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

  if(rand() < (RAND_MAX/40))
    {
      robot_rotate = - robot_rotate;
      cout << "Rotate 1 " << robot_rotate << endl;
    }

  cin.clear();
  while( !cin.eof() )
    {
      cin >> msg_name;
      msg_t = name2msg_to_robot_type(msg_name);
      switch(msg_t)
        {
        case INITIALIZE:
          cout << "Name Fire" << endl;
          cout << "Colour 2877ea 33ca11" << endl;
          break;
        case GAME_STARTS:
          cout << "Print I am ready" << endl;
          cout << "Rotate 1 " << robot_rotate << endl;
          cout << "Rotate 6 1.5" << endl;
          acceleration = 0.54;
          cout << "Acceleration " << acceleration << endl;
          break;
        case RADAR:
          {
            double dist;
            int object;

            cin >> dist >> object;
            switch(object)
              {
              case ROBOT:
                cout << "Shoot 2" << endl;
                //cout << "Print kill" << endl;
                break;
              case WALL:
                {
                  double old_acc = acceleration;
                  if( dist < 0.6 )
                    acceleration = 0.0;
                  else
                    acceleration = 1.0;

                  if( old_acc != acceleration )
                    cout << "Acceleration " << acceleration << endl;
                }
                break;
              case SHOT:
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
                //case ROBOT: cout << "Print Collided with robot" << endl; break;
                //case SHOT: cout << "Print Hit by a shot" << endl; break;
              case MINE: cout << "Print Oh no! A mine!" << endl; break;
              case COOKIE: cout << "Print Cookie eaten!" << endl; break;
                //case WALL: cout << "Print Bounced on a wall" << endl; break;
              }
            //if( enegry_diff != 0 ) cout << "Print energy changed by " << enegry_diff << endl;
          }
          break;
        case WARNING:
          cin.getline(text,80,'\n');
          cerr << text << endl;
          cerr << "Fire: Help, I got a yellow card!" << endl;
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


