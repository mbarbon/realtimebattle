
#ifndef __MESSAGETYPES__
#define __MESSAGETYPES__


/* Messagetypes from Robot */

#define MES_FROM_NAME                   "Name"
#define MES_FROM_TURNLEFT               "TurnLeft"
#define MES_FROM_TURNRIGHT              "TurnRight"
#define MES_FROM_ACCELERATE             "Accelerate"
#define MES_FROM_TURN_CANNON_LEFT       "TurnCannonLeft"
#define MES_FROM_TURN_CANNON_RIGHT      "TurnCannonRight"
#define MES_FROM_CANNON_LOCK            "CannonLock"
#define MES_FROM_SHOOT                  "Shoot"
#define MES_FROM_RADAR_ANGULAR_VELOCITY "RadarAngularVelocity"

/* Messagetypes to Robot */

#define MES_TO_START_TORUNAMENT         "StartTournament"
#define MES_TO_START_GAME               "StartGame"
#define MES_TO_END_GAME                 "EndGame"
#define MES_TO_NUMBER_OF_ROBOTS         "NumberOfRobots"
#define MES_TO_RADAR                    "Radar"
#define MES_TO_COLLITION_ROBOT          "CollitionRobot"
#define MES_TO_COLLITION_WALL           "CollitionWall"
#define MES_TO_COLLITION_MINE           "CollitionMine"
#define MES_TO_HIT                      "Hit"
#define MES_TO_DEAD                     "Dead"
#define MES_TO_EXIT_ROBOT               "ExitRobot"


extern "C++" {

enum message_to_robot_type 
{
  INITIALIZE = 0,              
  EXIT_ROBOT,
  RADAR,
  WARNING
};

enum message_from_robot_type 
{
  UNKNOWN = -1,
  ROTATE,
  NAME,
  OUTPUT,
};

enum argument_type
{
  NONE,
  STRING,
  DOUBLE,
  INT
};

enum warnings
{
  UNKNOWN_MESSAGE,
  PROCESS_TIME_LOW,
  ENERGY_LOW
};


struct Message
{
  char* msg;
  int number_of_args;
  argument_type arg_type[4];
};

static const Message message_to_robot[20] = 
{
  {"Initialize", 0, {NONE,   NONE,   NONE,   NONE}},
  {"ExitRobot",  0, {NONE,   NONE,   NONE,   NONE}},
  {"Radar",      2, {DOUBLE, INT,    NONE,   NONE}},
  {"Warning",    2, {INT,    STRING, NONE,   NONE}}
};

static const Message message_from_robot[20] = 
{
  {"Rotate",     2, {INT,    DOUBLE}},
  {"Name",       1, {STRING}},
  {"Output",     1, {STRING}},
  {"",           0, {}}
};

} // extern "C++"
#endif __MESSAGETYPES__
