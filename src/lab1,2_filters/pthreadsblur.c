#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include "ppmio.h"
#include "blurfilter3.h"
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

//struct for holding parameters for creating threads
typedef struct{
  int xsize,ysize, radius, over, under;
  pixel* src,*dst, *tmp;
  double *w;
}thread_param;


//helper function to call when creating threads. "unpacking" parameters and calling the blur function.
void* startblur(void *params_struct){
  thread_param *params = (thread_param*) params_struct;
  blurfilter(params->xsize, params->ysize, params->src, params->radius, 
      params->w, params->over, params->under, params->dst,
      params->tmp);
  return NULL;
}

int main( int argc, char *argv[] )
{
  if(argc!=5)
    {
      printf("invalid number of args");
      return 1;
    }

  struct timespec stime, etime;
  int xpix, ypix, me, np, maxpix, workload, work2, i, recvcount, bufsize,radius,nthreads;
  pixel* picture,* blurredpic;

  sscanf(argv[3], "%d", &radius);
  sscanf(argv[4], "%d", &nthreads);

  thread_param params[nthreads];
  pixel *tmps[nthreads];

  int over[nthreads], under[nthreads],scounts[nthreads], displs[nthreads];

  pthread_t threads[nthreads];

  picture = (pixel *)malloc((MAX_PIXELS*3)*sizeof(pixel*)); 
  blurredpic = (pixel *)malloc((MAX_PIXELS*3)*sizeof(pixel*)); 

  double w[MAX_RAD];
  get_gauss_weights(radius, w);

	
  read_ppm(argv[1], &xpix, &ypix, &maxpix, (char*)picture);
  //same as mpi, calculate and divide work. Only whole rows.
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

  clock_gettime(CLOCK_REALTIME, &stime);
  //create the parameter struct and assign values.
  for(i = 0; i<nthreads; i++)
    {
      over[i] = min(displs[i]/xpix, radius)*-1;
      under[i] = min((xpix*ypix-displs[i]-scounts[i])/xpix, radius);
      tmps[i] = (pixel *)malloc(scounts[i]*sizeof(pixel));
      params[i].xsize = xpix;
      params[i].ysize= scounts[i]/xpix;
      params[i].src = &picture[displs[i]];
      params[i].radius=radius;
      params[i].w=w;
      params[i].over=over[i];
      params[i].under=under[i];
      params[i].dst=&blurredpic[displs[i]];
      params[i].tmp=tmps[i];
      if(pthread_create(&threads[i], NULL, startblur, &params[i]))
      {
	fprintf(stderr, "Error creating thread %d.\n", i);
	return 1;
      }
    }
  //join threads, to sync up and make sure they have all done their work.
  for(i = 0; i<nthreads; i++)
  {
    if(pthread_join(threads[i], NULL))
    {
      fprintf(stderr, "Error joining thread %d.\n", i);
      return 2;
    }
    free(tmps[i]);
  }
  //picture is now 100% done, write to file.
  write_ppm (argv[2], xpix, ypix, (char *)blurredpic);
  clock_gettime(CLOCK_REALTIME, &etime);
  printf("Filtering took: %2g secs\n", (etime.tv_sec  - stime.tv_sec) +
	 1e-9*(etime.tv_nsec  - stime.tv_nsec)) ;

  free(picture);
  free(blurredpic);


  return 0;


}

