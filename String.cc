#include <strings.h>
#include <iostream.h>
#include <strstream.h>
#include <iomanip.h>
#include <stdlib.h>

#include "String.h"

String::String()
{
  buf_size = 0;
  length = 0;
  allocate_array(0);
  array[0] = '\0';
}

String::String(const char* str)
{
  buf_size = 0;
  *this = str;
}

String::String(const String& str)
{
  buf_size = 0;
  *this = str;
}

String::String(const int n)
{
  buf_size = 0;
  length = 15;
  allocate_array(15);
  array[15] = '\0';
  int pos = 14;

  int k = ( n>0 ? n : -n );

  if( k == 0 ) 
    array[pos--] = '0';
  else 
    for(; k > 0; k/=10) array[pos--] = '0' + k%10;

  if( n < 0 ) 
    {
      array[0] = '-';
      erase(1, pos);
    }
  else
    erase(0, pos+1);
}

String::String(const double x, const int digits, const bool exp_form)
{
  buf_size = 0;
  strstream ss;
  ss << (exp_form ? setiosflags(ios::scientific) : setiosflags(ios::fixed)) << setprecision(digits) << x << endl;
  ss >> *this;
}

String::~String()
{
  delete [] array;
}

void
String::allocate_array(const int size)
{
  if( buf_size > 0 ) delete [] array;
  buf_size = (size / 8 + 1) * 8;
  array = new char[buf_size];
}

void
String::enlarge_array(const int size)
{
  if( buf_size <= size )
    {
      char* temp = array;
      buf_size = (size / 8 + 1) * 8;
      array = new char[buf_size];
      strcpy(array, temp);
      delete [] temp;
    }
}

String&
String::operator=(const String& str)
{
  length = str.length;
  allocate_array(length);
  strcpy(array, str.array);
  return *this;
}

String&
String::operator=(const char* str)
{
  length = strlen(str);
  allocate_array(length);
  strcpy(array, str);
  return *this;
}

String&
String::operator=(const char c)
{
  length = 1;
  allocate_array(length);
  array[0] = c;
  array[1] = '\0';
  return *this;
}

String&
String::operator+=(const String& str)
{
  int newlength = length + str.length;
  enlarge_array(newlength);
  strcpy(&array[length], str.array);
  length = newlength;

  return *this;
}

String
operator+(const String& str1, const String& str2)
{
  String sum(str1);
  sum += str2;

  return sum;
}

int
operator==(const String& str1, const String& str2)
{
  return strcmp(str1.array, str2.array) == 0;
}

char
String::operator[](int pos) const
{
  if( pos < 0 || pos > length ) throw Range();

  return array[pos];
}

String&
String::erase(const int pos, const int size)
{
  if( pos < 0 || pos + size > length || size <= 0 ) throw Range();

  strcpy(&array[pos], &array[pos+size]); // ???
  length -= size;
  
  return *this;
}

String&
String::insert(const String& str, const int pos)
{
  if( pos < 0 || pos > length ) throw Range();

  char temp[length - pos + 1];
  strcpy(temp, &array[pos]);
      
  length += str.length;
  enlarge_array(length);
      
  strcpy(&array[pos], str.array);
  strcpy(&array[pos+str.length], temp);
      
  return *this;
}

int
str2int(const String& str)
{
  return atoi(str.array);
}

double
str2dbl(const String& str)
{
  return atof(str.array);
}

long
str2hex(const String& str)
{
  int pos = 0;
  long sign = 1;
  char c;
  long number = 0;

  if( str.array[pos] == '-' )
    {
      pos++;
      sign = -1;
    }

  for( ; pos < str.length; pos++)
    {
      c = str.array[pos];
      if( c >= '0' && c <= '9' ) 
        number=number*16+(c-'0');
      else if( c >='a' && c <= 'f' ) 
        number=number*16+10+(c-'a');
      else if( c >='A' && c <= 'F' ) 
        number=number*16+10+(c-'A');
      else break;
    }

  return sign*number;
}

String
hex2str(long n)
{
  String str;
  str.length = 15;
  str.allocate_array(15);
  str.array[15] = '\0';
  int pos = 14;

  int k = ( n>0 ? n : -n );

  if( k == 0 ) 
    str.array[pos--] = '0';
  else 
    for(; k > 0; k/=16) 
      {
        if( k%16 < 10 ) 
          str.array[pos--] = '0' + k%16;
        else
          str.array[pos--] = 'a' - 10 + k%16;
      }

  if( n < 0 ) 
    {
      str.array[0] = '-';
      str.erase(1, pos);
    }
  else
    str.erase(0, pos+1);  

  return str;
}

char*
String::copy_chars()
{ 
  char* str = new char[length+1];
  
  strcpy(str, array);
  
  return str;
}

istream&
operator>>(istream& is, String& str)
{
  char buf[100];
  is.get(buf, 100, '\n');

  str = buf;

  return( is );
}

ostream&
operator<<(ostream& os, const String& str)
{
  os << str.array;
  return( os );
}