

#ifndef __STRING__
#define __STRING__

#include <iostream.h>

enum string_double_t { STRING_NORMAL_FORM, STRING_EXP_FORM, STRING_FIXED_FORM };

class String
{
public:
  String();
  String(const String& str);
  String(const char* str);
  String(const char c);
  String(const int);
  String(const long int);
  String(const double, const int digits = 10, const string_double_t form = STRING_NORMAL_FORM);
  ~String();

  String& operator=(const String&);
  String& operator=(const char);
  String& operator=(const char*);
  friend String operator+(const String&, const String&);
  friend int operator==(const String&, const String&);
  friend int operator!=(const String&, const String&);
  String& operator+=(const String&);

  char operator[](int) const;

  friend ostream& operator<<(ostream&, const String&);  
  friend istream& operator>>(istream&, String&);  

  friend int str2int(const String&);
  friend long str2long(const String&);
  friend double str2dbl(const String&);
  friend long str2hex(const String&);
  friend String hex2str(const long);

  String& erase(const int pos, const int size = 1);
  String& insert(const String&, const int pos = 0);
  friend String get_segment(const String& str, const int start, const int end);

  int get_length() { return length; }
  const char* chars() { return array; }
  char* non_const_chars() { return array; }   //  Warning! This allows for changing the private member array.
  char* copy_chars();   // allocates an array of chars, remember to delete afterwards!

private:
  char* array;
  int length;
  int buf_size;

  //  String& copy(const String&);
  void allocate_array(const int size);
  void enlarge_array(const int size);

  class Range {};
};

#endif __STRING__
