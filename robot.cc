#include <iostream.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "robot.h"
#include "messagetypes.h"
//#include "pipesetup.h"

#define ROBOT_CODE_LENGTH 11

#define ROBOT_FF_TO_ADD   ".to"
#define ROBOT_FF_FROM_ADD ".from"

#define MESSAGE_MAX_BUFFER_LENGTH 400

#define DONOT_WAIT_FOR_MESSAGE 0
#define WAIT_FOR_MESSAGE 1

Robot::Robot(const char * rfn)
{
  char * temp,* robotdir;
  char temp2[401];

  robotname = NULL;

  active=true;

  if(NULL == (robotdir=getenv("RA_ROBOTDIR")))
    {
      robotdir = new char[2];
      robotdir[0]='\0';
    }

  robotfilename = new char[strlen(rfn) + strlen(robotdir) + 1];
  strcpy(robotfilename,rfn);

  robotfilename_without_path = new char[strlen(rfn) + 1];
  temp = new char[strlen(rfn) + strlen(robotdir) + 1];
  strcpy(temp2, rfn);
  if( NULL != (temp = strrchr(temp2,'/')))
    strcpy(robotfilename_without_path,&temp[1]);
  else
    {
      strcpy(robotfilename_without_path,rfn);
      strcpy(temp2,robotdir);
      strcat(temp2,robotfilename_without_path);
      strcpy(robotfilename,temp2);
    }

  // Create Pipes

  if (pipe (pipe_from))
    throw InitError("Couldn't setup pipe_from for robot ",robotfilename_without_path);
  if (pipe (pipe_to))
    throw InitError("Couldn't setup pipe_to for robot ",robotfilename_without_path);

  // Fork child process

  if( (robot_pid = fork()) < 0 )
    throw InitError("Couldn't fork childprocess for robot ",robotfilename_without_path);

  if(robot_pid == 0)
    {
      // Input for robot
      dup2(pipe_to[0],0);
      close(pipe_to[1]);

      // Output for robot
      dup2(pipe_from[1],1);
      close(pipe_from[0]);
      
      // Execute robot
      execl(robotfilename, robotfilename, NULL);
      throw InitError("Couldn't open robot ",robotfilename_without_path); 
    }
  else
    {
      char readbuffer[MESSAGE_MAX_BUFFER_LENGTH+1];
      // Close input side of pipe_to
      close(pipe_to[0]);
      // Close output side of pipe_from
      close(pipe_from[1]);

      get_message(readbuffer);
      if(strncmp(readbuffer,MES_FROM_NAME,4)==0)
        {
          robotname = new char[strlen( &readbuffer[strlen( MES_FROM_NAME ) + 1] )];
          strcpy( robotname,&readbuffer[strlen( MES_FROM_NAME ) + 1] );
          cout << "Robotname: " << robotname << endl;
        }
    }
  delete[] temp;
}

Robot::~Robot()
{
  delete[] robotname;
  delete[] robotfilename;
  delete[] robotfilename_without_path;
}

char * Robot::int_to_str( char * strnumber, int number )
{
  char tempstrnumber[20];
  int remainder,tempnumber=number;
  int pos=0;

  tempstrnumber[19]='\0';
  do
    {
      pos++;
      remainder = tempnumber % 10;
      tempstrnumber[19-pos] = remainder + '0';
    } while( ( tempnumber /= 10 ) > 0 );
  strcpy(strnumber,&tempstrnumber[19-pos]);
  return(strnumber);
}

char * Robot::get_robotname( char * name )
{
  strcpy( name,robotname );
  return( name );
}

// Message functions

char * Robot::get_message( char * readbuffer )
{
  int len_read;
  len_read = read(pipe_from[0],readbuffer,MESSAGE_MAX_BUFFER_LENGTH - 1);
  readbuffer[len_read]='\0';
  return(readbuffer);
}

void Robot::send_message( const char * message )
{
  write(pipe_to[1],message,strlen(message));
}

