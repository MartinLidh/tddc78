#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include "ppmio.h"
#include <string.h>


#define uint unsigned int 
typedef struct _pixel {
    unsigned char r,g,b;
} pixel;

#define MAX_RAD 1000
#define MAX_X 1.33
#define Pi 3.14159

void thresfilter(const uint xsize, const uint ysize, pixel* src, uint average){

  uint  i, psum, nump;
  nump = xsize * ysize;

  for(i = 0; i < nump; i++) {
    psum = (uint)src[i].r + (uint)src[i].g + (uint)src[i].b;
    if(average > psum) {
      src[i].r = src[i].g = src[i].b = 0;
    }
    else {
      src[i].r = src[i].g = src[i].b = 255;
    }
  }
}

int min(int x, int y)
{
  return y ^ ((x ^ y) & -(x < y));
}


typedef struct{
  uint xsize, ysize, *localsum, *globalavg;
  pixel* src;
  pthread_rwlock_t *lock;
  pthread_barrier_t *barrier;
}thread_param;

void* startthres(void *params_struct){
  thread_param *params = (thread_param*) params_struct;
  uint size = params->xsize*params->ysize;
  uint avg;
  for(i = 0, params->localsum = 0; i < size; i++) {
    params->localsum += (uint)params->src[i].r + (uint)params->src[i].g + (uint)params->src[i].b;
  }

  pthread_barrier_wait(params->barrier);
  pthread_rwlock_rdlock(params->lock);
  thresfilter(params->xsize, params->ysize, params->src, *globalsum);
  pthread_rwlock_unlock(params->lock);

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
  int xpix, ypix, me, np, maxpix, workload, work2, i, nthreads, globalsum;
  pixel* picture,* blurredpic;
  pthread_rwlock_t lock;
  pthread_rwlock_init(&lock, NULL);
  pthread_rwlock_wrlock(&lock);

  pthread_barrier_t barrier;
  pthread_barrier_init(&barrier, nthreads+1);

  sscanf(argv[3], "%d", &radius);
  sscanf(argv[4], "%d", &nthreads);

  thread_param params[nthreads];
  int localsums[nthreads];

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

  clock_gettime(CLOCK_REALTIME, &stime);

  for(i = 0; i<nthreads; i++)
    {
      over[i] = min(displs[i]/xpix, radius)*-1;
      under[i] = min((xpix*ypix-displs[i]-scounts[i])/xpix, radius);
      params[i].xsize = xpix;
      params[i].ysize= scounts[i]/xpix;
      params[i].src = &picture[displs[i]];
      params[i].localsum = &localsums[i];
      params[i].globalsum = &globalsum;
      params[i].lock = &lock;
      params[i].barrier = &barrier;
      if(pthread_create(&threads[i], NULL, startblur, &params[i]))
      {
	fprintf(stderr, "Error creating thread %d.\n", i);
	return 1;
      }
    }

  pthread_barrier_wait(&barrier);
  for(i = 0; i<nthreads; i++)
  {
    globalsum += localsums[i];
  }

  globalsum /= maxpix;
  pthread_rwlock_unlock(&lock);

  for(i = 0; i<nthreads; i++)
  {
    if(pthread_join(threads[i], NULL))
    {
      fprintf(stderr, "Error joining thread %d.\n", i);
      return 2;
    }
  }

  write_ppm (argv[2], xpix, ypix, (char *)blurredpic);
  clock_gettime(CLOCK_REALTIME, &etime);
  printf("Filtering took: %2g secs\n", (etime.tv_sec  - stime.tv_sec) +
	 1e-9*(etime.tv_nsec  - stime.tv_nsec)) ;

  free(picture);
  free(blurredpic);


  return 0;


}

