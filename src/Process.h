/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2002  Erik Ouchterlony and Ragnar Ouchterlony

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

//TODO : Change this file to RobotIO.h and RobotIO.cc

#ifndef __PROCESS__
#define __PROCESS__

//#include <unistd.h>
#include <sys/types.h>
#include <fstream>

#include "Messagetypes.h"
#include "Structs.h"

class Robot;

class RobotIO {
 public:
  virtual ~RobotIO() {};
  virtual void start( const enum game_mode_t mode) = 0;
  virtual bool is_process_running() = 0; //NOTE : Can this one be implemented ?
  virtual void end()                = 0;

  virtual string get_robot_filename() = 0;  //NOTE : do we really need this one ?
  virtual ofstream* get_outstreamp()  = 0;
  virtual ifstream* get_instreamp()   = 0;
};

class Process : public RobotIO
{
public:
  
  Process(const string& filenm);
  ~Process();

  //  void set_name_and_robot(const string& filenm, Robot* rbt);
  
  void start(const enum game_mode_t mode);
  bool is_process_running();
  void check();
  void end();
  
  void send_signal();
  void set_signal_to_send(const bool do_send, const int sig);
  
  bool get_default_non_blocking_state();
  void set_non_blocking_state(const bool use_non_blocking);
  
  void delete_pipes();
  void kill_forcefully();
  

  //  void get_messages();
  //  void send_message(enum message_to_robot_type ...);

  string get_robot_filename() { return filename; }
  ofstream* get_outstreamp() { return outstreamp; }  
  ifstream* get_instreamp() { return instreamp; }  

  pid_t get_pid() { return pid; }
 

private:

  //  message_from_robot_type name2msg_from_robot_type(char*);
  //  bool check_state_for_message(const message_from_robot_type msg_t, 
  //                               const enum state_t state1,
  //                               const enum state_t state2 = NO_STATE);


  //Robot* my_robot;

  string filename;
  string plain_filename;  // Filename without path

  bool process_running;

  bool send_usr_signal;
  int signal_to_send;
  bool use_non_blocking;

  double cpu_next_limit;
  double cpu_warning_limit;
  double cpu_timeout;

  ifstream* instreamp;
  ofstream* outstreamp;

  int pipes[2];

  pid_t pid;    

  game_mode_t game_mode;
};

#endif // __PROCESS__
