
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
};

struct entry_t
{
  entry_t(entry_datatype_t d, double mn, double mx) :
    datatype(d), min_value(mn), max_value(mx) {}
  entry_datatype_t datatype;
  double min_value;
  double max_value;
};

int factorial(const int n);

int binomial(const int n, const int k);

GdkColor make_gdk_colour(const long col);

void reorder_pointer_array(void** array, int size);

void entry_handler( GtkWidget * entry, entry_t * entry_info );

#endif __VARIOUS__
