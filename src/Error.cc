#include <iostream.h>
#include <stdio.h>
#include "Error.h"
  
Error::Error(const String& str, const String& func) 
  : error_string(str), error_string2(""), function_string(func), c(' ') {}


Error::Error(const String& str1, const String& str2, const String& func) 
  : error_string(str1), error_string2(str2), function_string(func), c(' ') {}

Error::Error(const String& str, char ch, const String& func) 
  : error_string(str), error_string2(""), function_string(func), c(ch) {}

void 
Error::print_message() 
{ 
  cerr << "Error in " << function_string << ": " << error_string << " " 
       << error_string2 << " " << c << endl;

  perror("Errno message");
}

