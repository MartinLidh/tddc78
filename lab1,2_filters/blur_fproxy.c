#include "blurfilter.h"

//extern void blurfilter_f_(int* xsize, int* ysize, pixel** src, int* radius, double** w);
extern void blurfilter_f_(int* xsize, int* ysize, pixel* src, int* radius, double* w);

void blurfilter(const int xsize, const int ysize, pixel* src, const int radius, const double *w){
  int xs,ys,r;
  pixel* s = src;
  double* wp = (double*)w;

  xs = xsize; ys = ysize; r = radius;

  blurfilter_f_(&xs, &ys, s, &r, wp);

}
