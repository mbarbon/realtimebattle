#include <iostream.h>
#include <unistd.h>
#include <stdlib.h>
#include "../messagetypes.h"

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
  char msg_name[81];
  char text[81];
  message_to_robot_type msg_t;

  cout << "Print Starting" << endl;
  cout << "Print Start ing" << endl;
  cout << "Print I_am_ready" << endl;
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
              cerr << "Fire: Got message: INITIALIZE" << endl;
              cout << "Name Fire" << endl;
              break;
            case GAME_STARTS:
              cerr << "Fire: Got message: GAME_STARTS" << endl;
              cout << "Print I_am_ready" << endl;
              cout << "Print I_am_ready" << endl;
              break;
            case RADAR:
              //              cerr << "Fire: Got message: RADAR" << endl;
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
