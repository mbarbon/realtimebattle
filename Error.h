
#ifndef __ERROR__
#define __ERROR__

#include <glib.h>

class Error
{
public:
  Error() {}
  Error(char* strp, char* funcp);
  Error(char* strp, char* str2p, char* funcp);
  Error(char* strp, char ch, char* funcp);
  ~Error();
  void print_message();
  
  friend class Arena;
  friend class ArenaObject;
private:

  GString* error_stringp;
  GString* error_string2p;
  GString* function_stringp;
  gchar c;
};

#endif __ERROR__
