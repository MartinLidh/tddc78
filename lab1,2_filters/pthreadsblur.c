#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "ppmio.h"
#include "blurfilter2.h"
#include <string.h>


#define MAX_RAD 1000
#define MAX_X 1.33
#define Pi 3.14159

void get_gauss_weights(int n, double* weights_out) {
  double x;
  int i;

  for( i = 0; i < n+1; i++) {
    x = (double)i * MAX_X/n;
    weights_out[i] = exp(-x*x * Pi);
  }
}

int min(int x, int y)
{
  return y ^ ((x ^ y) & -(x < y));
}


typedef struct{
  const int xsize,ysize, radius, over, under;
  pixel* src,*dst;
  const double *w;
}thread_param;

void startblur(void *params_struct){
  thread_param *params = (thread_param*) params_struct;
  void blurfilter(params->xsize, params->ysize, pixel* src, const int radius, const double *w, const int over,const int under);
}

int main( int argc, char *argv[] )
{
  if(argc!=4)
    {
      printf("invalid number of args");
      return;
    }
    
  int xpix, ypix, me, np, maxpix, workload, work2, i, recvcount, bufsize,radius,nthreads;
  pixel* picture,* blurredpic;

  sscanf(argv[3], "%d", &radius);
  sscanf(argv[4], "%d", &nthreads);
  

  int over[nthreads], under[nthreads],scounts[nthreads], displs[nthreads];
  
  pthread_t threads[nthreads];

  picture = (pixel *)malloc((MAX_PIXELS*3)*sizeof(pixel*)); 
  blurredpic = (pixel *)malloc((MAX_PIXELS*3)*sizeof(pixel*)); 

  double w[MAX_RAD];
  get_gauss_weights(radius, w);

	
  read_ppm(argv[1], &xpix, &ypix, &maxpix, (char*)picture);
    
  workload = (ypix/nthreads)*xpix;
  work2= ypix%nthreads;
  int offset = 0;
  for(i=0; i<nthreads; i++)
    {
      displs[i]=offset;
      if(i<work2)
	{
	  scounts[i] = workload+xpix;	
	}
      else{
	scounts[i] = workload;
      }
      offset += scounts[i];
    }


  for(i = 0; i<nthreads; i++)
    {
      over[i] = min(displs[i]/xpix, radius)*-1;
      under[i] = min((xpix*ypix-displs[i]-scounts[i])/xpix, radius);
      thread_param params = { .xsize = xpix, .ysize= scounts[i]/xpix, .src = &picture[displs[i]], .radius=radius,
			      .w=w, .over=over[i], .under=under[i], .dst=blurredpic}

      
      
      
    }

  //blur pixels
    
  
  if(me==0)
    { 
      write_ppm (argv[2], xpix, ypix, (char *)picture);
     
    }
  

#ifdef DEBUG
  char filename [30];
  sprintf(filename, "../img_node2_%d.ppm", me);
  write_ppm(filename, xpix, (scounts[me]+xpix*radius*2)/xpix, (char *)recvbuff);
#endif

  MPI_Finalize();
  free(picture);
  free(recvbuff);

 
  return 0;


}

