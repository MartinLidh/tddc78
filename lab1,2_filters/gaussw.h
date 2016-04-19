/*
  File: gausw.h

  Declaration of get_gauss_weights function.

 */

#ifndef _GAUSSW_H_
#define _GAUSSW_H_

/* Generate an array of weights for the gaussian filter. */
/* Input: n - number of weights to generate              */
/* Output: weights_out - array of weights. The element [0] */
/*  should be used for the central pixel, elements [1..n-1] */
/*  should be used for the pixels on a distance [1..n-1]  */  
void get_gauss_weights(int n, double* weights_out);

#endif
