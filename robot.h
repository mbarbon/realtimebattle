#include <unistd.h>
#include <strings.h>
#include <stdio.h>
#include "pos.h"
#include "messagetypes.h"

#define FIFO_TO_ROBOT_FILE_ADD   ".to"
#define FIFO_FROM_ROBOT_FILE_ADD ".from"

class Robot
{
public:
  Robot(const char *);
  ~Robot();
  char * get_message( char * readbuffer );
  void send_message( const char * message );
  char * int_to_str( char * strnumber, int number );
  char * get_robotname( char * name );

private:
  Position pos;
  char * robotfilename;
  char * robotname;
  char * robotcode;
  char * robotfilename_without_path;

  int pipe_from[2];
  int pipe_to[2];
  pid_t robot_pid;

  bool active;

  double robot_direction;
  double cannon_direction;
  double speed;

private:
  class InitError
  {
  public:
    InitError() {}
    InitError(char * msgp) { messagep = msgp; strcpy(argumentp,""); }
    InitError(char * msgp, char * argp) { messagep = msgp; argumentp = argp; }
    ~InitError() {}

    void print_message() { cerr << messagep << argumentp << endl; }

  private:
    char * messagep;
    char * argumentp;
  };
};
