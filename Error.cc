#include "Arena.h"
  
Error::Error(char* strp, char* funcp) 
{ 
  error_stringp = g_string_new(strp); 
  error_string2p = g_string_new(""); 
  c = '\0'; 
  function_stringp = g_string_new(funcp); 
}

Error::Error(char* strp, char* str2p, char* funcp) 
{ 
  error_stringp = g_string_new(strp); 
  error_string2p = g_string_new(str2p); 
  c = '\0'; 
  function_stringp = g_string_new(funcp); 
}

Error::Error(char* strp, char ch, char* funcp) 
{ 
  error_stringp = g_string_new(strp); 
  error_string2p = g_string_new(""); 
  c = ch; 
  function_stringp = g_string_new(funcp);
}
           
void 
Error::print_message() 
{ 
  if ( c != '\0' )
    cerr << "Error in " << function_stringp << ": " << error_stringp 
         << " " << c << endl; 
  else if (error_string2p != NULL) 
    cerr << "Error in " << function_stringp << ": " << error_stringp 
         << " " << error_string2p << endl;
  else
    cerr << "Error in " << function_stringp << ": " << error_stringp 
         << endl;         
}

