/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998  Erik Ouchterlony and Ragnar Ouchterlony

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

#include <iostream.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include "Messagetypes.h"

volatile double acceleration = 0.0;
volatile double robot_rotate = 0.0;
volatile sig_atomic_t exit_robot = false;

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

  if(rand() < (RAND_MAX/100))
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
          int init;
          cin >> init;
          if(init == 1)
            {
              cout << "Name Rotate and Fire" << endl;
              cout << "Colour 2877ea 33ca11" << endl;
            }
          break;
        case GAME_STARTS:
          cout << "Rotate 1 " << robot_rotate << endl;
          cout << "Rotate 6 1.5" << endl;
          acceleration = 0.54;
          cout << "Accelerate " << acceleration << endl;
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
                    cout << "Accelerate " << acceleration << endl;
                }
                break;
              case SHOT:
                //cout << "Print Help me!" << endl;
                //cout << "Print A shot is approaching!" << endl;
                break;
              case COOKIE:
                //                cout << "Print Yummy! A cookie" << endl;
                break;
              case MINE:
                //cout << "Print Avoid this mine" << endl;
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
          cout << "Print Help, I got a yellow card!  " << text << endl;
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

  signal(SIGUSR1, check_messages);
  cout << "RobotOption " << SEND_SIGNAL << " " << true << endl;

  srand(time(0));

  for(;;sleep(1))
    {
      if( exit_robot ) 
        {
          //          cerr << "rotate_and_fire: Exiting normally!" << endl;
          return(EXIT_SUCCESS);
        }
    }
  return(EXIT_SUCCESS);
}


