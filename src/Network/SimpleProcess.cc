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

#include <sys/resource.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <string>

#include "SimpleProcess.h"

SimpleProcess::SimpleProcess(const string& filenm)
{
  filename = filenm;  

  string::size_type nr;
  if( ( nr = filename.rfind('/')  ) == string::npos )
    plain_filename = filename;
  else
    plain_filename = filename.substr(nr+1, string::npos);

  process_running = false;

  send_usr_signal = false;
  signal_to_send = 0;
  
  use_non_blocking = get_default_non_blocking_state();

  cerr << "use_non_blocking: " << use_non_blocking << endl;

  ipipe_streamp = NULL;
  opipe_streamp = NULL;

  in_pipe = out_pipe = -1;  
  pid = -1;
}

SimpleProcess::~SimpleProcess()
{
  if( is_process_running() ) kill_forcefully();
  delete_pipes();
}


void
SimpleProcess::start()
{
  int pipe_in[2], pipe_out[2];
  if (pipe (pipe_in)) 
    {
      cerr << "Error in SimpleProcess::start: Couldn't setup pipe_in for robot " 
           << filename << endl;
      exit(EXIT_FAILURE);
    }


  if (pipe (pipe_out)) 
    {
      cerr << "Error in SimpleProcess::start: Couldn't setup pipe_out for robot " 
           << filename << endl;
      exit(EXIT_FAILURE);
    }


  if( (pid = fork()) < 0 )
    {
      cerr << "Error in SimpleProcess::start: Couldn't fork childprocess for robot " 
           << filename << endl;
      exit(EXIT_FAILURE);
    }


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
            {
              cerr << "Error in SimpleProcess::start, child: " 
                   << "Couldn't get pd_flags for pipe_out in robot " << filename << endl;
              exit(EXIT_FAILURE);
            }
     
          pd_flags |= O_NONBLOCK;
          if( fcntl(pipe_out[0], F_SETFL, pd_flags) == -1 ) 
            {
              cerr << "Error in SimpleProcess::start, child: " 
                   << "Couldn't change pd_flags for pipe_out in robot " << filename << endl;
              exit(EXIT_FAILURE);
            }
          
          if( (pd_flags = fcntl(pipe_in[1], F_GETFL, 0)) == -1 ) 
            {
              cerr << "Error in SimpleProcess::start, child: " 
                   << "Couldn't get pd_flags for pipe_in in robot " << filename << endl;
              exit(EXIT_FAILURE);
            }

          pd_flags |= O_NONBLOCK;
          if( fcntl(pipe_in[1], F_SETFL, pd_flags) == -1 ) 
            {
              cerr << "Error in SimpleProcess::start, child: " 
                   << "Couldn't change pd_flags for pipe_in in robot " << filename << endl; 
              exit(EXIT_FAILURE);
            }

        }
      
      // Check file attributes

      struct stat filestat;
      if( 0 != stat( filename.c_str(), &filestat ) )
        {
          cerr << "Error in SimpleProcess::start, child: "  
               << "Couldn't get stats for robot " << filename << endl;
          exit(EXIT_FAILURE);
        }
      if( !S_ISREG( filestat.st_mode) )
        {
          cerr << "Error in SimpleProcess::start, child: " 
               << "Robot file isn't regular, error for robot " << filename << endl;
          exit(EXIT_FAILURE);
        }
      if( !(filestat.st_mode & S_IXOTH) )
        {
          cerr << "Error in SimpleProcess::start, child: " 
               << "Robot file isn't executable for user, error for robot " << filename << endl;
          exit(EXIT_FAILURE);
        }
      if( (filestat.st_mode & S_ISUID) )
        {
          cerr << "Error in SimpleProcess::start, child: " 
               << "Set user ID is not allowed, error for robot " << filename << endl;
          exit(EXIT_FAILURE);
        }

      

      // Execute process. Should not return!
      if( execl(filename.c_str(), filename.c_str(), NULL) == -1 )
        cerr << "Error in SimpleProcess::start, child:" 
             << "Couldn't open robot " << filename << endl;

      cerr << "Error in SimpleProcess::start, child:" 
           << "Robot didn't execute, SHOULD NEVER HAPPEN!, error for " << filename << endl;
      
      exit(EXIT_FAILURE);
      
    }
  else
    {
      close(pipe_out[0]);     // Close input side of pipe_out
      close(pipe_in[1]);      // Close output side of pipe_in  
      
      out_pipe = pipe_out[1];
      in_pipe = pipe_in[0];

      // Make the pipes non-blocking
//        int pd_flags;
//        if( (pd_flags = fcntl(pipe_in[0], F_GETFL, 0)) == -1 ) 
//          {
//            cerr << "Error in SimpleProcess::start, parent: "
//                 << "Couldn't get pd_flags for pipe_in in robot " << filename << endl;
//            exit(EXIT_FAILURE);
//          }

//        pd_flags |= O_NONBLOCK;
//        if( fcntl(pipe_in[0], F_SETFL, pd_flags) == -1 ) 
//          {
//            cerr << "Error in SimpleProcess::start, parent: "
//                 << "Couldn't change pd_flags for pipe_in in robot " << filename << endl; 
//            exit(EXIT_FAILURE);
//          }

//        if( (pd_flags = fcntl(pipe_out[1], F_GETFL, 0)) == -1 )
//          {
//            cerr << "Error in SimpleProcess::start, parent: "
//                 << "Couldn't get pd_flags for pipe_out in robot " << filename << endl; 
//            exit(EXIT_FAILURE);
//          }

//        pd_flags |= O_NONBLOCK;
//        if( fcntl(pipe_out[1], F_SETFL, pd_flags) == -1 ) 
//          {
//            cerr << "Error in SimpleProcess::start, parent: "
//                 << "Couldn't change pd_flags for pipe_out in robot " << filename << endl; 
//            exit(EXIT_FAILURE);
//          }

      opipe_streamp = new ofstream( out_pipe );
      ipipe_streamp = new ifstream( in_pipe );
    }

  process_running = true;
}

bool
SimpleProcess::is_process_running()
{
  return process_running;
}

void
SimpleProcess::send_signal()
{
  if( send_usr_signal )
    kill(pid, signal_to_send);
}

void
SimpleProcess::set_signal_to_send(const bool do_send, const int sig)
{
  send_usr_signal = do_send;
  signal_to_send = sig;
  if( do_send ) send_signal();
}

void
SimpleProcess::kill_forcefully()
{
  kill(pid, SIGKILL);
  delete_pipes();
  process_running = false;
}

void
SimpleProcess::delete_pipes()
{
  if( ipipe_streamp != NULL ) delete ipipe_streamp;
  ipipe_streamp = NULL;
  if( opipe_streamp != NULL ) delete opipe_streamp;
  opipe_streamp = NULL;
  if( in_pipe != -1 ) 
    {
      close(in_pipe);
      in_pipe = -1;
    }
  if( out_pipe != -1 ) 
    {
      close(out_pipe);
      out_pipe = -1;
    }
  
}

// If non_blocking is _not_ used, a file, OPTION_TMP_RTB_DIR/"robotname"
// , is created.
void
SimpleProcess::set_non_blocking_state(const bool non_bl)
{
  if( non_bl == use_non_blocking ) return;

  string tmp_filename = rtb_tmp_dir + "/" + plain_filename;

  //  create_tmp_rtb_dir();

  if( non_bl )
    remove( tmp_filename.c_str() );
  else
    {
      int fd = open(tmp_filename.c_str(), 
                    O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH );

      if( fd != -1 )  close( fd );
    }
  
 
  use_non_blocking = non_bl;

  //  restart_process

  kill_forcefully();
  start();
}

bool
SimpleProcess::get_default_non_blocking_state()
{
  string tmp_filename = rtb_tmp_dir + "/" + plain_filename;
  
  int fd;
  if( ( fd = open(tmp_filename.c_str(), O_RDONLY) ) != -1 )
    {
      close(fd);
      return false;
    }
  
  return true;
}
