#include "thresfilter.h"

extern void thresfilter_f_(int* xsize, int* ysize, pixel* src);

void thresfilter(const int xsize, const int ysize, pixel* src){
  int xs,ys;
  pixel* s = src;

  xs = xsize; ys = ysize;

  thresfilter_f_(&xs, &ys, s);

}
