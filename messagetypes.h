
#ifndef __MESSAGETYPES__
#define __MESSAGETYPES__

enum message_to_robot_type 
{
  UNKNOWN_MESSAGE_TO_ROBOT = -1,
  INITIALIZE = 0,
  GAME_STARTS,
  GAME_FINISHES,
  SAVE_DATA,
  BIN_DATA_TO,
  LOAD_DATA_FINISHED,
  EXIT_ROBOT,
  RADAR,
  INFO,
  ROBOTS_LEFT,
  COLLISION,
  DEAD,
  WARNING
};

enum message_from_robot_type 
{
  UNKNOWN_MESSAGE_FROM_ROBOT = -1,
  ROTATE,
  ACCELERATE,
  BREAK,
  SHOOT,
  NAME,
  COLOUR,
  PRINT,
  LOAD_DATA,
  BIN_DATA_FROM,
  SAVE_DATA_FINISHED
};

enum argument_type
{
  NONE,
  STRING,
  DOUBLE,
  INT,
  HEX,
  BINDATA
};

enum warning_type
{
  UNKNOWN_MESSAGE,
  PROCESS_TIME_LOW,
  ENERGY_LOW,
  VARIABLE_OUT_OF_RANGE,
  MESSAGE_SENT_IN_ILLEGAL_STATE
};

enum object_type 
{ 
  NOOBJECT = -1, 
  ROBOT, 
  SHOT, 
  WALL, 
  COOKIE, 
  MINE, 
  EXPLOSION 
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
  {"GameFinishes",0,{NONE,   NONE,   NONE,   NONE}},
  {"SaveData",   0, {NONE,   NONE,   NONE,   NONE}},
  {"BinDataTo",  2, {INT,    BINDATA,NONE,   NONE}},
  {"LoadDataFinished",0, {NONE,   NONE,   NONE,   NONE}},
  {"ExitRobot",  0, {NONE,   NONE,   NONE,   NONE}},
  {"Radar",      3, {DOUBLE, INT,    DOUBLE, NONE}},   // first arg: distance, second arg: object_type, third arg: radar_angle
  {"Info",       3, {DOUBLE, DOUBLE, DOUBLE, NONE}},   // first arg: time, second arg: speed, third arg: cannon_angle
  {"RobotsLeft", 1, {INT,    NONE,   NONE,   NONE}},   // arg: robots left
  {"Collision",  2, {INT,    DOUBLE, NONE,   NONE}},   // first arg: object_type, second arg: change in energy 
  {"Dead",       0, {NONE,   NONE,   NONE,   NONE}},   // Robot died  
  {"Warning",    2, {INT,    STRING, NONE,   NONE}},   // first arg: warning_type, second arg: string
  {"",           0, {}}
};

static const Message message_from_robot[20] = 
{
  {"Rotate",       2, {INT,    DOUBLE}},     // first arg: what to rotate: 1 Robot, 2 cannon, 4 radar and apropriate sums for combinations
                                             // second arg: angular velocity (rad/s)
  {"Acceleration", 1, {DOUBLE}},             // arg: acceleration (m/s^2)  
  {"Break",        1, {DOUBLE}},             // arg: break precentage, 0 = no break, 1 = full break 
  {"Shoot",        1, {DOUBLE}},             // arg: amount of energy
  {"Name",         1, {STRING}},             // arg: name
  {"Colour",       2, {HEX, HEX}},           // first arg: home colour, second arg: away colour
  {"Print",        1, {STRING}},             // arg: message to print   
  {"LoadData",     0, {}},
  {"BinDataFrom",  2, {INT, BINDATA}},       // first arg: number of bytes,  second arg: data
  {"SaveDataFinished",0, {}},
  {"",             0, {}}
};

#endif __MESSAGETYPES__
