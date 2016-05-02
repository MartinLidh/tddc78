/*
  File: blurfilter3.h

  Declaration of pixel structure and blurfilter function.
    
 */

#ifndef _BLURFILTER3_H_
#define _BLURFILTER3_H_

/* NOTE: This structure must not be padded! */
typedef struct _pixel {
    unsigned char r,g,b;
} pixel;
void blurfilter(const int xsize, const int ysize, pixel* src, const int radius, const double *w, const int over, const int under, pixel* dst, pixel *temp);

#endif
