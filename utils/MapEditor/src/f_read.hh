#include <stdio.h>
#include <string>

void SkipUnknownObject(FILE *);


int read_line(FILE *fp);            //Read one line and put it into a buffer if not a comment...
void skip_line(FILE *fp);           //Skip a line...
string read_info_string(FILE *fp);  //Read an info string
bool blank_line();
