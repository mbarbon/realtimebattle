#include <stdlib.h>
#include <time.h>
#include "Vector2D.h"
#include "robot.h"

extern "C"
{
#include "gui.h"
}

#define DEFAULT_WIDTH  600
#define DEFAULT_HEIGHT 480

void print_help_message( void )
{
  cout << "   Usage: robotarena [options] robot1 [robot2] [robot3] ... " << endl << endl;
  cout << " Options: No options recognized" << endl << endl;
}

int main ( int argc, char * argv[] )
{
  int i,j;
  int nr_robots,nr_failed=0;

  gtk_init (&argc, &argv);

  srand(time(0));
  
  for(i=1; i < argc && argv[i][0] == '-';i++)
    {
      for(j=1; argv[i][j] !='\0'; j++)
        {
          switch( argv[i][j] )
            {
            default:
              cout << "Cannot recognize option -" << argv[i][j] << endl;
              break;
            }
        }
    }
  if(argc - i < 1) {
    print_help_message();
    return EXIT_FAILURE;
  }

  nr_robots=argc-i;
  Robot * robots[nr_robots];
  for(j=i;j<argc;j++)
    {
      try
        {
          robots[j-i-nr_failed] = new Robot(argv[j]);
        }
      catch( Robot::InitError error )
        {
          error.print_message();
          nr_failed++;
        }
    }
  nr_robots -= nr_failed;
  if(nr_robots == 0)
    {
      cout << "No correct robots could be executed." << endl << endl;
      print_help_message();
    }

  char * robotnames[nr_robots];
  for(i=0;i<nr_robots;i++)
    {
      char tempname[50];
      robots[i]->get_robotname(tempname);
      robotnames[i] = new char[strlen(tempname) + 1];
      strcpy(robotnames[i],tempname);
    }
    
  control_win( nr_robots, robotnames );
  arena_win();
  gtk_main();

  return EXIT_SUCCESS;
}
