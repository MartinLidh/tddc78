/*
  File gaussw.c

  Implementation of get_gauss_weights function.

 */
#include <math.h>

#define MAX_X 1.33
#define Pi 3.14159

/* Generate an array of weights for the gaussian filter. */
/* Input: n - number of weights to generate              */
/* Output: weights_out - array of weights. The element [0] */
/*  should be used for the central pixel, elements [1..n] */
/*  should be used for the pixels on a distance [1..n]  */  
void get_gauss_weights(int n, double* weights_out) {
  double x;
  int i;

  for( i = 0; i < n+1; i++) {
	x = (double)i * MAX_X/n;
	weights_out[i] = exp(-x*x * Pi);
  }
}
