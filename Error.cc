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

Error::~Error()
{
  delete error_stringp;
  delete error_string2p;
  delete function_stringp;
}
           
void 
Error::print_message() 
{ 
  if ( c != '\0' )
    cerr << "Error in " << function_stringp->str << ": " << error_stringp->str 
         << " " << c << endl; 
  else if (error_string2p != NULL) 
    cerr << "Error in " << function_stringp->str << ": " << error_stringp->str 
         << " " << error_string2p->str << endl;
  else
    cerr << "Error in " << function_stringp->str << ": " << error_stringp->str
         << endl;         
}

