#ifndef __CONS_H__
#define __CONS_H__

#include <stdarg.h>
#include <string>
#include <iostream>
#include <istream.h>
#include <unistd.h>

using namespace std;

/* NOTE : Do we really need all this */
/* TODO : Maybe let each function act as she want... */
/* TODO : Use the readline functions ... */
/* TODO : Use a StreamBuffer to make it more homogenious ... */

class Console;
extern Console C_Ignore;		/* never print RFC-style number prefix */
extern Console C_Comment; 		/* for human eyes only */
extern Console C_Version;       	/* version info */
extern Console C_Debug;		        /* debug info */
extern Console C_Log_Base;		/* 10, 11, 12 depending on log level */
extern Console C_OK;	        	/* success of requested operation */
extern Console C_Connection;    	/* new client */
extern Console C_Disconnected;  	/* client gone */
extern Console C_Rejected;		/* client rejected */
extern Console C_Fail;          	/* failure of requested operation */ 
extern Console C_MetaError;		/* failure of meta server */
extern Console C_Syntax;		/* syntax error or value out of range */
extern Console C_Bounce;		/* option no longer available */
extern Console C_GenFail; 		/* failure not caused by a requested operation */
extern Console C_Warning; 		/* something may be wrong */
extern Console C_Ready; 		/* waiting for input */

class Console { //: public std::ostream, public ConsoleStream {
 public:
  Console( string prompt, int fd = STDOUT_FILENO) :
    prompt(prompt)
    //std::ostream(this), ConsoleStream( prompt, fd )
    {};
  Console() : prompt(" # ")
    //ConsoleStream( string(" # "), STDOUT_FILENO )
    {};


  /* TODO : See what is useless */
  void handle_log(int level, string);
  void update_prompt();
  int dump(int i, string);
  void write(int i, string);
  void write(int i, char *message, ...);
  void write(string);
  void puts(int i, string);
  void rfconly(int i, string);
  void flush();
  void set_style( bool i );
  bool get_style( );
  void prompt_on( );
  void prompt_off( );
  void prompt_enter( );


 protected:
  string prompt;
  static bool prompt_is_showing;
  static bool show_prompt;
  static bool rfcstyle;
  static string input_prompt;
};

#endif




/*
  class ConsoleStream : public std::streambuf {
  public:
  ConsoleStream( string prompt, int fd = 0 ) : prompt( prompt ), fd( fd ) {
  setp(buffer, buffer+1);
  }
  ~ConsoleStream() {
  sync();
  }
  int sync() {
  if( pptr() > pbase() ) {
  tampon = tampon + buffer[0];
  setp( buffer, buffer + 1 );
  }
  return 0;
  }
  int overflow( int c ) {
  sync();
  
  if( c == '\n' )
  {
  cout<<prompt<<tampon<< std::endl ;
  tampon = "";
  }
  //else if( c == std::flush )
  //  {
  //TODO : We dont need the prompt all the time...
  //	cout<<prompt<<tampon<< std::flush ;
  //	tampon = "";
  //     }
  
  else
  {
  if( c != 0 ) {
  *pptr() = static_cast<char>( c );
  pbump( 1 );
  }
  }
  return c;
  }
  private:
  int fd;
  string prompt;
  char buffer[1]; // TODO : Do it with a bigger buffer and parse the buffer next... 
  string tampon;
  };
*/
