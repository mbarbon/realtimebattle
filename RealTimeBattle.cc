#include <stdlib.h>
#include <time.h>
#include "Arena.h"
#include "Vector2D.h"
#include "gui.h"

void 
print_help_message()
{
  cout << "   Usage: RealTimeBattle [options] robot1 [robot2] [robot3] ... " << endl << endl;
  cout << " Options: No options recognized" << endl << endl;
}

gint
update_function(gpointer the_arenap)
{  
  gint res;
  try
    {
      res = ((Arena*)the_arenap)->timeout_function();
    }
  catch ( Error the_error )
	 {
		the_error.print_message();
		exit( EXIT_FAILURE );
	 }

  return res;
}


int 
main ( int argc, char* argv[] )
{
  int i, j;
  int nr_robots;
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

  if(argc - i < 1) 
    {
    print_help_message();
    return EXIT_FAILURE;
  }

  nr_robots=argc-i;
  bool failed = false;
  char** robotnames;
  robotnames = new char*[nr_robots];
  for(j=i;j<argc;j++)
    {
      robotnames[j-i] = new char[strlen(argv[j]) + 1];
      strcpy(robotnames[j-i],argv[j]);
      // check if file argv[j] exists and is executable
      //        {
      //          error.print_message();
      //          nr_failed++;
      //        }
    }
  robotnames[nr_robots] = NULL;
  
  char** arenanames; 
  arenanames = new char*[5];
  arenanames[0] = new char[strlen("Arenas/Forest.arena")+1];
  strcpy(arenanames[0], "Arenas/Forest.arena");
  arenanames[1] = new char[strlen("Arenas/Rooms.arena")+1];
  strcpy(arenanames[1], "Arenas/Rooms.arena");
  arenanames[2] = new char[strlen("Arenas/Star.arena")+1];
  strcpy(arenanames[2], "Arenas/Star.arena");
  arenanames[3] = new char[strlen("Arenas/Circles.arena")+1];
  strcpy(arenanames[3], "Arenas/Circles.arena");
  arenanames[4] = NULL;

  if(failed)
    {
      cout << "No correct robots could be executed." << endl << endl;
      print_help_message();
      return EXIT_FAILURE;
    }
    
  Arena* the_arena = new Arena;
  try
    {
      the_arena->get_the_gui()->setup_control_window();
      the_arena->start_tournament( robotnames, arenanames, nr_robots, 2, 2);
    }
  catch ( Error the_error )
	 {
		the_error.print_message();
		return EXIT_FAILURE;
	 }

  for(int i=0; i<nr_robots; i++) delete [] robotnames[i];
  delete [] robotnames;
  
  timeout_tag = gtk_timeout_add( 40, GtkFunction(update_function), (gpointer) the_arena);

  gtk_main();

  return EXIT_SUCCESS;
}
