
#ifndef __VARIOUS__
#define __VARIOUS__

#include <gdk/gdktypes.h>

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
//#define abs(a) ((a) > 0 ? (a) : -(a))
//#define sgn(a) ((a) > 0 ? 1 : -1)

enum entry_datatype_t
{
  ENTRY_INT,
  ENTRY_DOUBLE,
  ENTRY_HEX,
  ENTRY_CHAR,
  ENTRY_BOOL
};

struct entry_t
{
  entry_t(entry_datatype_t d, bool s) :
    datatype(d), allow_sign(s) {}
  entry_datatype_t datatype;
  bool allow_sign;
};

int factorial(const int n);

int binomial(const int n, const int k);

GdkColor make_gdk_colour(const long col);

long gdk2hex_colour(const GdkColor& col);

void reorder_pointer_array(void** array, int size);

void entry_handler( GtkWidget * entry, entry_t * entry_info );

#endif __VARIOUS__
