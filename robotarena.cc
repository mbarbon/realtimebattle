#include <stdlib.h>
#include <time.h>
#include "Vector2D.h"
#include "robot.h"
#include "gui.h"

#define DEFAULT_WIDTH  600
#define DEFAULT_HEIGHT 480

GTimer klocka;

void print_help_message( void )
{
  cout << "   Usage: robotarena [options] robot1 [robot2] [robot3] ... " << endl << endl;
  cout << " Options: No options recognized" << endl << endl;
}

gint timeout_function(gpointer data)
{
  gulong musec = 100;
  double tid;

  tid = g_timer_elapsed(&klocka, &musec);
  //  tidssnitt = tid / ++rakna;
  //  insert_message( "RealTimeBattle", "Hej" );
  //  cout << "  Microsecs: " << musec;
  //  cout << "  R�kna: " << rakna << endl;
  //  g_timer_reset(&klocka);

  //  draw_objects();

  return true;
}

int main ( int argc, char * argv[] )
{
  int i,j;
  int nr_robots,nr_failed=0;
  gint timeout_tag;

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
  RobotOld * robots[nr_robots];
  for(j=i;j<argc;j++)
    {
      try
        {
          robots[j-i-nr_failed] = new RobotOld(argv[j]);
        }
      catch( RobotOld::InitError error )
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
    
  Gui gui( nr_robots, robotnames, 1000, 1000);
  
  g_timer_start(&klocka);

  timeout_tag = gtk_timeout_add( 100, GtkFunction(timeout_function), (gpointer) "");

  gtk_main();

  return EXIT_SUCCESS;
}
