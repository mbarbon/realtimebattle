#include <iostream.h>
#include <unistd.h>
#include <stdlib.h>
#include "../messagetypes.h"

enum object_type { NOOBJECT, ROBOT, SHOT, WALL, COOKIE, MINE, EXPLOSION };
static const number_of_object_types = 6;

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

int 
main(int argc, char * argv[])
{
  double acceleration;

  char msg_name[81];
  char text[81];
  message_to_robot_type msg_t;

  for(;;sleep(1))
    {
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
              cout << "Rotate 7 0.7" << endl;
              acceleration = 1.05;
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
                    cout << "Print kill" << endl;
                    break;
                  case WALL:
                    if( dist < 0.5 )
                      acceleration = -0.45;
                    cout << "Acceleration " << acceleration << endl;
                    break;
                  case SHOT:
                    cout << "Print Help me!" << endl;
                    cout << "Print A shot is killing me!" << endl;
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
            case WARNING:
              cin.getline(text,80,'\n');
              cerr << text << endl;
              cerr << "Fire: Help, I got a yellow card!" << endl;
              break;
            case EXIT_ROBOT:
              cout << "Fire: Shutting down and leaving" << endl;
              exit(EXIT_SUCCESS);
              break;
            default:
              break;
            }
        }
    }

  return(EXIT_SUCCESS);
}
