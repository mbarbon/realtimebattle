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

#ifndef __MESSAGETYPES__
#define __MESSAGETYPES__

enum message_to_robot_type 
{
  UNKNOWN_MESSAGE_TO_ROBOT = -1,
  INITIALIZE,
  YOUR_NAME,
  YOUR_COLOUR,
  BIN_DATA_TO,
  ASCII_DATA_TO,
  LOAD_DATA_FINISHED,
  GAME_OPTION,
  GAME_STARTS,
  RADAR,
  INFO,
  ROBOT_INFO,
  ROTATION_REACHED,
  ENERGY,
  ROBOTS_LEFT,
  COLLISION,
  WARNING,
  DEAD,
  GAME_FINISHES,
  SAVE_DATA,
  EXIT_ROBOT
};

enum message_from_robot_type 
{
  UNKNOWN_MESSAGE_FROM_ROBOT = -1,
  ROBOT_OPTION,
  NAME,
  COLOUR,
  LOAD_DATA,
  ROTATE,
  ROTATE_TO,
  ROTATE_AMOUNT,
  SWEEP,
  ACCELERATE,
  BRAKE,
  BREAK,
  SHOOT,
  PRINT,
  DEBUG,
  BIN_DATA_FROM,
  ASCII_DATA_FROM,
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
  //  ENERGY_LOW,
  //VARIABLE_OUT_OF_RANGE,
  MESSAGE_SENT_IN_ILLEGAL_STATE,
  UNKNOWN_OPTION
};

enum game_option_type
{
  ROBOT_MAX_ROTATE,
  ROBOT_CANNON_MAX_ROTATE,
  ROBOT_RADAR_MAX_ROTATE,

  ROBOT_MAX_ACCELERATION,
  ROBOT_MIN_ACCELERATION,

  ROBOT_START_ENERGY,
  ROBOT_MAX_ENERGY,
  ROBOT_ENERGY_LEVELS,

  SHOT_SPEED,
  SHOT_MIN_ENERGY,
  SHOT_MAX_ENERGY,
  SHOT_ENERGY_INCREASE_SPEED,

  TIMEOUT,

  DEBUG_LEVEL            // 0 - no debug, 5 - highest debug level
};

enum robot_option_type
{
  SEND_SIGNAL,              // 0 - true, 1 - false   
  SEND_ROTATION_REACHED     // 0 - no messages, 1 - messages when RotateTo and RotateAmount finished, 
                            // 2 - messages also when sweep direction is changed 
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

static const int number_of_object_types = 6;

struct Message
{
  char* msg;
  int number_of_args;
  enum argument_type arg_type[4];
};

static const struct Message message_to_robot[25] = 
{
  {"Initialize", 1, {INT,    NONE,   NONE,   NONE}},   // arg: if 1 this is the first sequence for the robot, send name and colour!!
  {"YourName",   1, {STRING, NONE,   NONE,   NONE}},   // arg: previous name, send new name only if you don't like it
  {"YourColour", 1, {HEX,    NONE,   NONE,   NONE}},   // arg: previous colour
  {"BinData",    2, {INT,    BINDATA,NONE,   NONE}},
  {"AsciiData",  1, {STRING, NONE,   NONE,   NONE}},
  {"LoadDataFinished",0, {NONE,NONE, NONE,   NONE}},
  {"GameOption", 2, {INT,    DOUBLE, NONE,   NONE}},   // arg 1: OPTION_NR,  arg 2:  value 
  {"GameStarts", 0, {NONE,   NONE,   NONE,   NONE}},
  {"Radar",      3, {DOUBLE, INT,    DOUBLE, NONE}},   // first arg: distance, second arg: object_type, third arg: radar_angle
  {"Info",       3, {DOUBLE, DOUBLE, DOUBLE, NONE}},   // first arg: time, second arg: speed, third arg: cannon_angle
  {"RobotInfo",  2, {DOUBLE, INT,    NONE,   NONE}},   // first arg: Other robots energylevel, second arg: enemy - 0, teammate - 1
  {"RotationReached",1,{INT, NONE,  NONE,   NONE}},    // first arg: what has finished rotation (see Rotate below)
  {"Energy",     1, {DOUBLE, NONE,   NONE,   NONE}},   // arg: energylevel
  {"RobotsLeft", 1, {INT,    NONE,   NONE,   NONE}},   // arg: robots left
  {"Collision",  2, {INT,    DOUBLE, NONE,   NONE}},   // first arg: object_type, second arg: collision angle
  {"Warning",    2, {INT,    STRING, NONE,   NONE}},   // first arg: warning_type, second arg: string
  {"Dead",       0, {NONE,   NONE,   NONE,   NONE}},   // Robot died  
  {"GameFinishes",0,{NONE,   NONE,   NONE,   NONE}},
  {"SaveData",   0, {NONE,   NONE,   NONE,   NONE}},

  {"ExitRobot",  0, {NONE,   NONE,   NONE,   NONE}},

  {"",           0, {}}
};

static const struct Message message_from_robot[25] = 
{
  {"RobotOption",  2, {INT, INT}},        // arg 1: OPTION_NR,  arg 2:  value 
  {"Name",         1, {STRING}},             // arg: name
  {"Colour",       2, {HEX, HEX}},           // first arg: home colour, second arg: away colour
  {"LoadData",     1, {INT}},                // arg: type of data, 0 - binary, 1 - ascii
  {"Rotate",       2, {INT, DOUBLE}},     // first arg: what to rotate: 1 Robot, 2 cannon, 4 radar and apropriate sums for combinations                                                             // second arg: angular velocity (rad/s)
  {"RotateTo",     3, {INT, DOUBLE, DOUBLE}},     // first and second arg: as in 'Rotate', third arg: angle to move to
  {"RotateAmount", 3, {INT, DOUBLE, DOUBLE}},     // first and second arg: as in 'Rotate', third arg: angle to rotate
  {"Sweep",        4, {INT, DOUBLE, DOUBLE, DOUBLE}}, // first and second arg: as in 'Rotate', but only cannon and radar
                                                      // third arg: left_angle, fourth arg: right_angle  (relative robot)
  {"Accelerate", 1, {DOUBLE}},             // arg: accelerate (m/s^2)  
  {"Brake",        1, {DOUBLE}},             // arg: brake precentage, 0 = no brake, 1 = full brake 
  {"Break",        1, {DOUBLE}},             // Wrong spelling, included for compabillity reasons.
  {"Shoot",        1, {DOUBLE}},             // arg: amount of energy
  {"Print",        1, {STRING}},             // arg: message to print   
  {"Debug",        1, {STRING}},             // arg: debug message
  {"BinData",      2, {INT, BINDATA}},    // first arg: number of bytes,  second arg: data
  {"AsciiData",    1, {STRING}},          // arg: data
  {"SaveDataFinished",0, {}},
  {"",             0, {}}
};

#endif __MESSAGETYPES__
