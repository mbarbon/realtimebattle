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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#ifdef TIME_WITH_SYS_TIME 
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include <sys/resource.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>

#include "Process.h"
#include "Messagetypes.h"
#include "Structs.h"
#include "Robot.h"
#include "ArenaBase.h"
#include "ArenaController.h"
#include "Options.h"

void Error(const bool fatal, const String& error_msg, const String& function_name);

Process::Process(const String& filenm, Robot* rbt)
{
  filename = filenm;  
  my_robot = rbt;

  int nr;
  if( ( nr = filename.find( '/', 0, true) ) == -1 )
    plain_filename = filename;
  else
    plain_filename = get_segment(filename, nr+1, -1);

  process_running = false;

  send_usr_signal = false;
  signal_to_send = 0;
  
  use_non_blocking = get_default_non_blocking_state();

  instreamp = NULL;
  outstreamp = NULL;

  pipes[0] = -1;  
  pipes[1] = -1;
  pid = -1;
}

Process::~Process()
{
  if( is_process_running() ) kill_forcefully();
  delete_pipes();
}


void
Process::start(const enum game_mode_t mode)
{
  game_mode = mode;

  int pipe_in[2], pipe_out[2];
  if (pipe (pipe_in)) 
    Error(true, "Couldn't setup pipe_in for robot " + filename, "Process::start");

  if (pipe (pipe_out)) 
    Error(true, "Couldn't setup pipe_out for robot " + filename, "Process::start");

  if( (pid = fork()) < 0 )
    Error(true, "Couldn't fork childprocess for robot " + filename, "Process::start");

  if(pid == 0)   // Child process, to be the new robot
    {
      // Make pipe_out the standard input for the robot
      close(pipe_out[1]);
      dup2(pipe_out[0], STDIN_FILENO);

      // Make pipe_in the standard output
      dup2(pipe_in[1],STDOUT_FILENO);
      close(pipe_in[0]);

      // Make the pipes non-blocking

      if( use_non_blocking )
        {
          int pd_flags;
          if( (pd_flags = fcntl(pipe_out[0], F_GETFL, 0)) == -1 ) 
            Error(true, "Couldn't get pd_flags for pipe_out in robot " + filename, 
                  "Process::start, child");
          pd_flags |= O_NONBLOCK;
          if( fcntl(pipe_out[0], F_SETFL, pd_flags) == -1 ) 
            Error(true, "Couldn't change pd_flags for pipe_out in robot " + filename, 
                  "Process::start, child");
          
          
          if( (pd_flags = fcntl(pipe_in[1], F_GETFL, 0)) == -1 ) 
            Error(true, "Couldn't get pd_flags for pipe_in in robot " + filename, 
                  "Process::start, child");
          pd_flags |= O_NONBLOCK;
          if( fcntl(pipe_in[1], F_SETFL, pd_flags) == -1 ) 
            Error(true, "Couldn't change pd_flags for pipe_in in robot " + filename, 
                  "Process::start, child");
        }
      
      // Check file attributes

      struct stat filestat;
      if( 0 != stat( filename.chars(), &filestat ) ) 
        Error(true, "Couldn't get stats for robot " + filename, "Process::start, child");
      if( !S_ISREG( filestat.st_mode) )
        Error(true, "Robot file isn't regular, error for robot " + filename, 
              "Process::start, child");
      if( !(filestat.st_mode & S_IXOTH) )
        Error(true, "Robot file isn't executable for user, error for robot " + filename, 
              "Process::start, child");
      if( (filestat.st_mode & S_ISUID) )
        Error(true, "Set user ID is not allowed, error for robot " + filename, 
              "Process::start, child");

      // Lower priority by one
     
      int old;
      if( (old = getpriority (PRIO_PROCESS, 0)) == -1 )
        Error(true, "Couldn't get priority for robot " + filename, "Process::start, child");
      if( setpriority (PRIO_PROCESS, 0, old + 1) == -1)
        Error(true, "Couldn't set priority for robot " + filename, "Process::start, child");
      
      // Close all pipes not belonging to the robot
      
        Robot* robotp;

        ListIterator<Robot> li;
        for( the_arena.get_all_robots_in_sequence()->first(li); li.ok(); li++ )
          {
            robotp = li();
            if( robotp != my_robot ) robotp->get_process()->delete_pipes();
          }

      if( game_mode != DEBUG_MODE )
        {
          struct rlimit res_limit;

          //        // Deny file access
          
          //            if( getrlimit( RLIMIT_NOFILE, &res_limit ) == -1 )
          //              Error(true, "Couldn't get file limits for robot " + filename, 
          //                    "Process::start, child");
          
          //            //res_limit.rlim_cur = 7;   // Don't know why, but it is the lowest number that works
          //            if( setrlimit( RLIMIT_NOFILE, &res_limit ) == -1 )
          //              Error(true, "Couldn't limit file access for robot " + filename, 
          //                    "Process::start, child");
          

          // Forbid creation of child processes
          
#ifdef HAVE_RLIMIT_NPROC
          if( getrlimit( RLIMIT_NPROC, &res_limit ) == -1 )
            Error(true, "Couldn't get proc limits for robot " + filename, 
                  "Process::start, child");
          
          res_limit.rlim_cur = 0;
          if( setrlimit( RLIMIT_NPROC, &res_limit ) == -1 )
            Error(true, "Couldn't limit child processes for robot " + filename, 
                  "Process::start, child");
#endif
        }

      // Execute process. Should not return!
      if( execl(filename.chars(), filename.chars(), NULL) == -1 )
        Error(true, "Couldn't open robot " + filename, "Process::start, child");

      Error(true, "Robot didn't execute, SHOULD NEVER HAPPEN!, error for " + filename, 
            "Process::start, child");
    }
  else
    {
      close(pipe_out[0]);     // Close input side of pipe_out
      close(pipe_in[1]);      // Close output side of pipe_in  
      
      pipes[0] = pipe_out[1];
      pipes[1] = pipe_in[0];

      // Make the pipes non-blocking
      int pd_flags;
      if( (pd_flags = fcntl(pipe_in[0], F_GETFL, 0)) == -1 ) 
        Error(true, "Couldn't get pd_flags for pipe_in in robot " + filename, 
              "Process::start, parent");
      pd_flags |= O_NONBLOCK;
      if( fcntl(pipe_in[0], F_SETFL, pd_flags) == -1 ) 
        Error(true, "Couldn't change pd_flags for pipe_in in robot " + filename, 
              "Process::start, parent");
      if( (pd_flags = fcntl(pipe_out[1], F_GETFL, 0)) == -1 ) 
        Error(true, "Couldn't get pd_flags for pipe_out in robot " + filename, 
              "Process::start, parent");
      pd_flags |= O_NONBLOCK;
      if( fcntl(pipe_out[1], F_SETFL, pd_flags) == -1 ) 
        Error(true, "Couldn't change pd_flags for pipe_out in robot " + filename, 
              "Process::start, parent");

      outstreamp = new ofstream(pipe_out[1]);
      instreamp = new ifstream(pipe_in[0]);
    }

  // wait some time to let process start up

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 40000;  //  1/25 s 

  select(FD_SETSIZE, NULL, NULL, NULL, &timeout);
  

  process_running = true;
  cpu_next_limit = the_opts.get_d(OPTION_CPU_START_LIMIT);
  cpu_warning_limit = cpu_next_limit * the_opts.get_d(OPTION_CPU_WARNING_PERCENT);
  cpu_timeout = 0.0;

  my_robot->set_values_at_process_start_up();
}

bool
Process::is_process_running()
{
  return process_running;
}

void
Process::check()
{
  String procfilename = "/proc/" + String(pid) + "/stat";

  if( is_process_running() )
    {
      ifstream procfile(procfilename.chars());
      if( !procfile ) 
        {
          process_running = false;
          return;
        }

      char buf[16];

      for(int i=0; i<13; i++)
        procfile >> buf;
      
      int jiffies;
      
      procfile >> jiffies;

      double current_cpu = (double)jiffies / 100.0;

      

      // TODO: Rework cpu_usage limitation !

      double time_survived_in_sequence = 0; // WARNING: wrong!
      double tot_time = the_arena.get_total_time() + time_survived_in_sequence;
      if( !my_robot->is_alive() ) tot_time = time_survived_in_sequence;

      if( current_cpu > cpu_next_limit )
        {
          if( tot_time >= cpu_timeout )
            {
              // add time
              cpu_warning_limit = cpu_next_limit + 
                the_opts.get_d(OPTION_CPU_EXTRA) * the_opts.get_d(OPTION_CPU_WARNING_PERCENT);
              cpu_next_limit += the_opts.get_d(OPTION_CPU_EXTRA);
              cpu_timeout = tot_time + the_opts.get_d(OPTION_CPU_PERIOD);
            }
          else
            {
              // cpu limit exceeded, robot disqualified
              my_robot->die();
              // add time for next game
              cpu_warning_limit = cpu_next_limit + 
                the_opts.get_d(OPTION_CPU_EXTRA) * the_opts.get_d(OPTION_CPU_WARNING_PERCENT);
              cpu_next_limit += the_opts.get_d(OPTION_CPU_EXTRA);
              cpu_timeout = tot_time + the_opts.get_d(OPTION_CPU_PERIOD);
            }
        }
      else if( current_cpu > cpu_warning_limit && tot_time < cpu_timeout )
        {
          my_robot->send_message( WARNING, PROCESS_TIME_LOW, String(cpu_next_limit - current_cpu).chars());
          cpu_warning_limit = cpu_next_limit;
        }
    }
}


void
Process::end()
{
  my_robot->send_message(EXIT_ROBOT);
  send_signal();
}

void
Process::send_signal()
{
  if( send_usr_signal )
    kill(pid, signal_to_send);
}

void
Process::set_signal_to_send(const bool do_send, const int sig)
{
  send_usr_signal = do_send;
  signal_to_send = sig;
  if( do_send ) send_signal();
}

void
Process::kill_forcefully()
{
  kill(pid, SIGKILL);
  delete_pipes();
  process_running = false;
}

void
Process::delete_pipes()
{
  if( instreamp != NULL ) delete instreamp;
  instreamp = NULL;
  if( outstreamp != NULL ) delete outstreamp;
  outstreamp = NULL;
  if( pipes[0] != -1 ) 
    {
      close(pipes[0]);
      pipes[0] = -1;
    }
  if( pipes[1] != -1 ) 
    {
      close(pipes[1]);
      pipes[1] = -1;
    }
  
}

// If non_blocking is _not_ used, a file, OPTION_TMP_RTB_DIR/"robotname"
// , is created.
void
Process::set_non_blocking_state(const bool non_bl)
{
  if( non_bl == use_non_blocking ) return;

  String tmp_filename = the_opts.get_s( OPTION_TMP_RTB_DIR ) +
    "/" + plain_filename;

  create_tmp_rtb_dir();

  if( non_bl )
    remove( tmp_filename.chars() );
  else
    {
      int fd = open(tmp_filename.chars(), 
                    O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH );

      if( fd != -1 )  close( fd );
    }
  
 
  use_non_blocking = non_bl;

  //  restart_process

  kill_forcefully();
  start(game_mode);
}

bool
Process::get_default_non_blocking_state()
{
  String tmp_filename = the_opts.get_s( OPTION_TMP_RTB_DIR ) +
    "/" + plain_filename;
  
  int fd;
  if( ( fd = open(tmp_filename.chars(), O_RDONLY) ) != -1 )
    {
      close(fd);
      return false;
    }
  
  return true;
}