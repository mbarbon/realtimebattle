#ifndef __CONS_H__
#define __CONS_H__

#include <stdarg.h>
#include <string>
#include <iostream.h>

#define C_IGNORE -1		/* never print RFC-style number prefix */
#define C_COMMENT 0 		/* for human eyes only */
#define C_VERSION 1		/* version info */
#define C_DEBUG	2		/* debug info */
#define C_LOG_BASE 10		/* 10, 11, 12 depending on log level */
#define C_OK 100		/* success of requested operation */
#define C_CONNECTION 101	/* new client */
#define C_DISCONNECTED 102	/* client gone */
#define C_REJECTED 103		/* client rejected */
#define C_FAIL 200		/* failure of requested operation */ 
#define C_METAERROR 201		/* failure of meta server */
#define C_SYNTAX 300		/* syntax error or value out of range */
#define C_BOUNCE 301		/* option no longer available */
#define C_GENFAIL 400		/* failure not caused by a requested operation */
#define C_WARNING 500		/* something may be wrong */
#define C_READY 999		/* waiting for input */

class Console {
 public:
  Console() : 
    prompt(" # "), prompt_is_showing(false), show_prompt(false)
    {};
  Console(string p) : 
    prompt(p), prompt_is_showing(false), show_prompt(false) 
    {};


  void handle_log(int level, string);
  void update_prompt();
  int dump(int i, string);
  void write(int i, string);
  void write(int i, char *message, ...);
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
  bool prompt_is_showing, show_prompt;
  static bool rfcstyle;

};

#endif
