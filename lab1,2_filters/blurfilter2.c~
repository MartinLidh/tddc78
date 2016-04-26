/*
  File: blurfilter.c

  Implementation of blurfilter function.
    
 */
#include <stdio.h>
#include "blurfilter.h"
#include "ppmio.h"


pixel* pix(pixel* image, const int xx, const int yy, const int xsize)
{
  register int off = xsize*yy + xx;

#ifdef DBG
  if(off >= MAX_PIXELS) {
    fprintf(stderr, "\n Terribly wrong: %d %d %d\n",xx,yy,xsize);
  }
#endif
  return (image + off);
}

void blurfilter(const int xsize, const int ysize, pixel* src, const int radius, const double *w){
  int x,y,x2,y2, wi;
  double r,g,b,n, wc;
  pixel dst[MAX_PIXELS];


  for (y=0; y<ysize; y++) {
    for (x=0; x<xsize; x++) {
      r = w[0] * pix(src, x, y, xsize)->r;
      g = w[0] * pix(src, x, y, xsize)->g;
      b = w[0] * pix(src, x, y, xsize)->b;
      n = w[0];
      for ( wi=1; wi <= radius; wi++) {
	wc = w[wi];
	x2 = x - wi;
	if(x2 >= 0) {
	  r += wc * pix(src, x2, y, xsize)->r;
	  g += wc * pix(src, x2, y, xsize)->g;
	  b += wc * pix(src, x2, y, xsize)->b;
	  n += wc;
	}
	x2 = x + wi;
	if(x2 < xsize) {
	  r += wc * pix(src, x2, y, xsize)->r;
	  g += wc * pix(src, x2, y, xsize)->g;
	  b += wc * pix(src, x2, y, xsize)->b;
	  n += wc;
	}
      }
      pix(dst,x,y, xsize)->r = r/n;
      pix(dst,x,y, xsize)->g = g/n;
      pix(dst,x,y, xsize)->b = b/n;
    }
  }

  for (y=0; y<ysize; y++) {
    for (x=0; x<xsize; x++) {
      r = w[0] * pix(dst, x, y, xsize)->r;
      g = w[0] * pix(dst, x, y, xsize)->g;
      b = w[0] * pix(dst, x, y, xsize)->b;
      n = w[0];
      for ( wi=1; wi <= radius; wi++) {
	wc = w[wi];
	y2 = y - wi;
	if(y2 >= 0) {
	  r += wc * pix(dst, x, y2, xsize)->r;
	  g += wc * pix(dst, x, y2, xsize)->g;
	  b += wc * pix(dst, x, y2, xsize)->b;
	  n += wc;
	}
	y2 = y + wi;
	if(y2 < ysize) {
	  r += wc * pix(dst, x, y2, xsize)->r;
	  g += wc * pix(dst, x, y2, xsize)->g;
	  b += wc * pix(dst, x, y2, xsize)->b;
	  n += wc;
	}
      }
      pix(src,x,y, xsize)->r = r/n;
      pix(src,x,y, xsize)->g = g/n;
      pix(src,x,y, xsize)->b = b/n;
    }
  }

}



