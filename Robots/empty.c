#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../messagetypes.h"

#define FALSE 0
#define TRUE 1

#define MAX_BUFFER_LENGTH 400

#define STDIN 0
#define STDOUT 1

int main(int argc, char * argv[])
{
  char readbuffer[MAX_BUFFER_LENGTH];
  int len_read;

  write(STDOUT,"Name Empty ",strlen("Name Empty "));

  return(EXIT_SUCCESS);
}
