/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2000  Erik Ouchterlony and Ragnar Ouchterlony

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

#ifndef __SIMPLE_PROCESS__
#define __SIMPLE_PROCESS__

#include <string>

//#include <unistd.h>
#include <sys/types.h>
#include <fstream>


class Robot;

class SimpleProcess
{
public:
  
  SimpleProcess(const string& filenm);
  ~SimpleProcess();

  //  void set_name_and_robot(const string& filenm, Robot* rbt);
  
  void start();
  bool is_running() {  return process_running; }
  
  void send_signal(const int sig);

  bool get_default_non_blocking_state();
  void set_non_blocking_state(const bool use_non_blocking);

  void delete_pipes();
  void kill_forcefully();


  pid_t get_pid() { return pid; }




  string filename;
  string plain_filename;  // Filename without path

  string rtb_tmp_dir;

  bool process_running;

  bool send_usr_signal;
  int signal_to_send;
  bool use_non_blocking;

  ifstream* ipipe_streamp;
  ofstream* opipe_streamp;

  int in_pipe;
  int out_pipe;

  pid_t pid;    

};

#endif __SIMPLE_PROCESS__
