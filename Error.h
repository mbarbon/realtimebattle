
#ifndef __ERROR__
#define __ERROR__

#include "String.h"

class Error
{
public:
  Error() {}
  Error(const String& str, const String& func);
  Error(const String& str1, const String& str2, const String& func);
  Error(const String& str, char ch, const String& func);
  ~Error() {}
  void print_message();
  
  friend class Arena;
  friend class ArenaObject;
private:

  String error_string;
  String error_string2;
  String function_string;
  char c;
};

#endif __ERROR__
