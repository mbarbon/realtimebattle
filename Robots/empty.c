#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//#include "../messagetypes.h"

int main(int argc, char * argv[])
{
  write(STDOUT_FILENO, "RobotOption 0 0\n", 16); 
  write(STDOUT_FILENO, "Name Empty\n", 11);
  write(STDOUT_FILENO, "Colour aaffaa ee2299\n", 21);

  printf("Print printf worked!");
  fprintf(stdout, "Print fprintf worked!");

  sleep(500);

  return(EXIT_SUCCESS);
}
