
#ifndef __MESSAGETYPES__
#define __MESSAGETYPES__

enum message_to_robot_type 
{
  UNKNOWN_MESSAGE_TO_ROBOT = -1,
  INITIALIZE = 0,
  GAME_STARTS,
  EXIT_ROBOT,
  RADAR,
  WARNING
};

enum message_from_robot_type 
{
  UNKNOWN_MESSAGE_FROM_ROBOT = -1,
  ROTATE,
  ACCELERATE,
  SHOOT,
  NAME,
  COLOUR,
  PRINT,
};

enum argument_type
{
  NONE,
  STRING,
  DOUBLE,
  INT,
  HEX
};

enum warnings
{
  UNKNOWN_MESSAGE,
  PROCESS_TIME_LOW,
  ENERGY_LOW,
  VARIABLE_OUT_OF_RANGE
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
  {"GameStarts", 0, {NONE,   NONE,   NONE,   NONE}},
  {"ExitRobot",  0, {NONE,   NONE,   NONE,   NONE}},
  {"Radar",      2, {DOUBLE, INT,    NONE,   NONE}},
  {"Warning",    2, {INT,    STRING, NONE,   NONE}},
  {"",             0, {}}
};

static const Message message_from_robot[20] = 
{
  {"Rotate",       2, {INT,    DOUBLE}},     // first arg: what to rotate, second arg: amount
  {"Acceleration", 1, {DOUBLE}},
  {"Shoot",        0, {DOUBLE}},             // arg: amount of energy
  {"Name",         1, {STRING}},
  {"Colour",       2, {HEX, HEX}},
  {"Print",        1, {STRING}},
  {"",             0, {}}
};

#endif __MESSAGETYPES__
