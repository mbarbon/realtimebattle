#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <string>

#include "Console.h"


bool Console::rfcstyle = false;

/************************************************************************
Function to handle log messages.
This must match the log_callback_fn typedef signature.
************************************************************************/
void Console::handle_log(int level, string message)
{
  if(rfcstyle) {
    write(C_LOG_BASE+level, message);
  } else {
    char Level[16];
    sprintf(Level, "%d", level);
    write(C_LOG_BASE+level, string(Level) + ": " +  message);
  }
}

/************************************************************************
Print the prompt if it is not the last thing printed.
************************************************************************/
void Console::update_prompt()
{
  if (prompt_is_showing || !show_prompt)
    return;
  
  dump(C_READY, prompt);
  flush();
  prompt_is_showing = true;
}

/************************************************************************
Write to console without line-break, don't print prompt.
************************************************************************/
int Console::dump(int i, string buf)
{
  if(prompt_is_showing) {
    cout<<endl;
  }
  if ((rfcstyle) && (i >= 0)) {
    cout<<i<<" "<<buf;
  } else {
    cout<<buf;
  }
  prompt_is_showing = false;
  return buf.length();
}

/************************************************************************
Write to console and add line-break, and show prompt if required.
************************************************************************/
void Console::write(int i, char* message, ...)
{
  char buf[512];
  va_list args;
  
  va_start(args, message);
  vsnprintf(buf, sizeof(buf), message, args);
  va_end(args);

  puts(i, buf);
}


void Console::write(int i, string buf)
{
  puts(i, buf);
}

/************************************************************************
Write to console and add line-break, and show prompt if required.
Same as write, but without the format string stuff.
The real reason for this is because __attribute__ complained
with write(C_COMMENT,"") of "warning: zero-length format string";
this allows puts(C_COMMENT,"");
************************************************************************/
void Console::puts(int i, string str)
{
  if(prompt_is_showing) {
    cout<<"\n";
  }
  if ((rfcstyle) && (i >= 0)) {
    cout<<i<<" "<<str<<endl;
  } else {
    cout<<str<<endl;
  }
  prompt_is_showing = false;
  update_prompt();
}

/************************************************************************
Ensure timely update. 
************************************************************************/
void Console::flush()
{
  cout.flush();
}

/************************************************************************
Set style.
************************************************************************/
void Console::set_style( bool i )
{
  rfcstyle = i;
  if (rfcstyle) 
    puts(C_OK, "Ok. RFC-style set.");
  else
    puts(C_OK, "Ok. Standard style set.");
}

/************************************************************************
Returns rfc-style.
************************************************************************/
bool Console::get_style()
{
  return rfcstyle;
}

/************************************************************************
Make sure a prompt is printed, and re-printed after every message.
************************************************************************/
void Console::prompt_on()
{
  static int first = true;
  if (first) {
    puts(C_COMMENT, "");
    puts(C_COMMENT, "For introductory help, type 'help'.");
    first = false;
  }
  show_prompt=true;
  update_prompt();
}

/************************************************************************
Do not print a prompt after log messages.
************************************************************************/
void Console::prompt_off()
{
  show_prompt=false;
}

/************************************************************************
User pressed enter: will need a new prompt 
************************************************************************/
void Console::prompt_enter()
{
  prompt_is_showing = false;
}

