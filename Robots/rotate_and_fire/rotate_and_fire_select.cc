
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rotate_and_fire.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>


// This version of rotate_and_fire uses 'select' to check 
// for new messages. 

int 
main(int argc, char * argv[])
{
  class RotateAndFire raf_obj("Rotate&Fire-select", "66ccdd");


  // STDIN should not block and signals are not used

  raf_obj.robot_option( USE_NON_BLOCKING, true );
  //  raf_obj.robot_option( SIGNAL, SIGUSR1 );

  // Make a set of file descriptor to check for.
  // In this case only STDIN
  
  fd_set file_descriptor_set;
  FD_ZERO( &file_descriptor_set );
  FD_SET( STDIN_FILENO, &file_descriptor_set );  



  while( !raf_obj.is_quitting() )
    {
      raf_obj.check_messages();

      // Here you can do what you like, when finished ...



      // ... wait for the next message
      select( FD_SETSIZE, &file_descriptor_set, NULL, NULL, NULL );
    }

  return( EXIT_SUCCESS );
}