#include <iostream.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <glib.h>
#include <math.h>
#include "../messagetypes.h"

#define abs(x) ((x>0) ? (x) : -(x))

volatile double acceleration = 0.0;
volatile double robot_rotate = 0.0;
volatile double cannon_rotate = 0.0, radar_rotate = 0.0;
double tid, speed, radar_angle, cannon_angle, tid0, slumprotate = 1.0 ;
double maxspeed =10.0, robotmaxrotate, cannonmaxrotate, radarmaxrotate;
double robotstartenergy, robotmaxenergy, robotenergylevels, shotspeed, shotmaxenergy;
double  shotminenergy, shotenergyincreasespeed,timeout, rotend=0.0, broms=0.0;
int robots_left = 20, rotating;
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
          int init;
          cin >> init ;
          if (init == 1)
            {
              cout << "Name Thomas2" << endl;
              cout << "Colour 993322  777777" << endl;
            }
          break;
        case GAME_STARTS:
          //          cout << "Rotate 1 " << robot_rotate << endl;
          cout << "Sweep 6 " << cannonmaxrotate << " " << -M_PI/2.0 << " " << M_PI/2.0 << endl;
          acceleration = 0.4;
          cout << "Acceleration " << acceleration << endl;
          break;
        case GAME_FINISHES:
          break;

        case GAME_OPTION:
          int optnr;
          double value;
          cin >> optnr >> value ;
          switch(optnr)
            {
            case ROBOT_MAX_ROTATE:
              robotmaxrotate = value;
              break;
            case ROBOT_CANNON_MAX_ROTATE:
              cannonmaxrotate = value;
              break;
            case ROBOT_RADAR_MAX_ROTATE:
              radarmaxrotate = value ;
              break;
            case ROBOT_START_ENERGY:
              robotstartenergy = value;
              break;
            case ROBOT_MAX_ENERGY:
              robotmaxenergy = value;
              break;
            case ROBOT_ENERGY_LEVELS:
              robotenergylevels = value;
              break;
            case SHOT_SPEED:
              shotspeed = value;
              break;
            case SHOT_MIN_ENERGY:
              shotminenergy = value;
              break;
            case SHOT_MAX_ENERGY:
              shotmaxenergy = value;
              break;
            case SHOT_ENERGY_INCREASE_SPEED:
              shotenergyincreasespeed = value;
              break;
            case TIMEOUT:
              timeout = value;
              break;
            }
          break;
        case SAVE_DATA:
          cout << "SaveDataFinished" << endl;
          break;

        case INFO:
          cin >> tid >> speed  >> cannon_angle;
          break;
          
        case ROBOTS_LEFT:
          cin >> robots_left;
          break;

        case RADAR:
          {
            double dist;
            int object;

            cin >> dist >> object >> radar_angle;
            switch(object)
              {
              case ROBOT:
                broms=0.0;
                if (speed<maxspeed)
                  acceleration = 2.0;
                else
                  acceleration = 0.0;
                cout << "RotateAmount 1 " << robotmaxrotate << radar_angle << endl;
                cout << "Acceleration " << acceleration << endl;
                cout << "Sweep 6 " << cannonmaxrotate << " " << -M_PI/6.0 << " " << M_PI/6.0 << endl; 
                cout << "Shoot 2" << endl;
                cout << "Break " << broms << endl;
                break;
              case WALL:
                if (rotend < tid)
                  rotating = 0;
                if (speed < 0.2)
                  {
                    broms = 0.0;
                    if ((rotating == 0) && (dist <= 3.0))
                      {
                        cout << "RotateAmount 6 " << cannonmaxrotate << M_PI << endl;
                        rotating = 6;
                        rotend = tid + M_PI/cannonmaxrotate;
                        acceleration = 0.0;
                      }
                    else   
                      {
                        if (dist > 3.0)
                          {
                            if ((radar_angle > M_PI/20.0) && (rotating > 1))
                              {
                                cout << "RotateAmount 1 " << robotmaxrotate << radar_angle << endl;
                                cout << "RotateTo 6 " << -robotmaxrotate << 0.0 << endl;
                                rotating = 1;
                                rotend = tid + radar_angle/robotmaxrotate ;
                                acceleration = 0.0;
                              }
                            else
                              {
                                if (rotating == 0)
                                  {
                                    broms = 0.0;
                                    acceleration = 2.0;
                                    cout << "Sweep 6 " << cannonmaxrotate << " " << -M_PI/2.0 << " " << M_PI/2.0 << endl;
                                  }
                              }
                          }
                      }
                    cout << "Acceleration " << acceleration << endl;
                  }
                else
                  {
                  if ((dist < 3.0) && (abs(radar_angle) < M_PI/4.0) && (rotating != 1))
                    {
                      acceleration = 0.0;
                      broms = 1.0-abs(radar_angle);
                      cout << "Acceleration " << acceleration << endl;
                      cout << "Sweep 6 " << cannonmaxrotate << " " << -M_PI/6.0 << " " << M_PI/6.0 << endl; 
                    }
                  }
                cout << "Break " << broms << endl;
                
                break;
              case SHOT:
                break;
              case COOKIE:
                broms = 0.0;
                robot_rotate = 0.0;
                if (speed<maxspeed)
                  acceleration = 2.0;
                else
                  acceleration = 0.0;
                cout << "RotateAmount 1 " << robotmaxrotate << radar_angle << endl;
                rotend = tid + radar_angle/robotmaxrotate;
                rotating = 1;
                cout << "Acceleration " << acceleration << endl;
                cout << "Sweep 6 " << cannonmaxrotate << " " << -M_PI/6.0 << " " << M_PI/6.0 << endl; 
                cout << "Break " << broms << endl;
                break;

              case MINE:
                if (dist < 5.0)
                  cout << "Shoot " << shotminenergy << endl;
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
            double coll_angle;
            cin >> tmp >> coll_angle;
            switch(tmp)
              {
              case ROBOT: 
                break;
              case SHOT:
                break;
              case MINE: 
                cout << "Print Oh no! A mine!" << endl; 
                break;
              case COOKIE: 
                cout << "Print Cookie eaten!" << endl; 
                break;
              case WALL:
                break;
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
//robot_rotate = 0.53;

  timer = g_timer_new();
  g_timer_reset(timer);
  srand(time(0));
  //check_messages(SIGUSR1);
  signal(SIGUSR1, check_messages);

  cout << "RobotOption " << SEND_SIGNAL << " " << true << endl;

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


