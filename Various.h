
#ifndef __VARIOUS__
#define __VARIOUS__

#include <gdk/gdktypes.h>

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
//#define abs(a) ((a) > 0 ? (a) : -(a))
//#define sgn(a) ((a) > 0 ? 1 : -1)


int factorial(const int n);

int binomial(const int n, const int k);

GdkColor make_gdk_colour(const long col);

void reorder_pointer_array(void** array, int size);

#endif __VARIOUS__
