/*
  File: blurfilter2.h

  Declaration of pixel structure and blurfilter function.
    
 */

#ifndef _BLURFILTER2_H_
#define _BLURFILTER2_H_

/* NOTE: This structure must not be padded! */
typedef struct _pixel {
    unsigned char r,g,b;
} pixel;

void blurfilter(const int xsize, const int ysize, pixel* src, const int radius, const double *w);

#endif
