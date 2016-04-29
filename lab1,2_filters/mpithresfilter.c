#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ppmio.h"

#define uint unsigned int 
typedef struct _pixel {
    unsigned char r,g,b;
} pixel;

void thresfilter(const int xsize, const int ysize, pixel* src, uint average){
  
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


int main (int argc, char ** argv) {
  int xsize, ysize, colmax, workload, work2, i, offset, np, me;
  pixel src[MAX_PIXELS], recvbuff[MAX_PIXELS];

  struct timespec stime, etime, rootstime, rootetime;

  /* Take care of the arguments */

  if (argc != 3) {
    fprintf(stderr, "Usage: %s infile outfile\n", argv[0]);
    exit(1);
  }
    
  MPI_Comm com = MPI_COMM_WORLD;
  MPI_Init( &argc, &argv );
  MPI_Comm_size( com, &np );
  MPI_Comm_rank( com, &me );

  int scounts[np], displs[np];


  MPI_Aint offsets[1] = {0};
  MPI_Datatype oldtypes[1] = {MPI_CHAR};
  int blockcounts[1] = {3};
  MPI_Datatype MPI_PIXEL;

  MPI_Type_struct(1, blockcounts,offsets, oldtypes,&MPI_PIXEL);
  MPI_Type_commit(&MPI_PIXEL);



  clock_gettime(CLOCK_REALTIME, &rootstime);

  if(me==0)
    {		      
      read_ppm(argv[1], &xsize, &ysize, &colmax, (char*)src);
      workload = (ysize/np)*xsize;
      work2= ysize%np;
      int offset = 0;
      for(i=0; i<np; i++)
	{
	  displs[i]=offset;
	  if(i<work2)
	    {
	      scounts[i] = workload+xsize;	
	    }
	  else{
	    scounts[i] = workload;
	  }
	  offset += scounts[i];
	}
      if (colmax > 255) {
	fprintf(stderr, "Too large maximum color-component value\n");
	exit(1);
      }

    }

  clock_gettime(CLOCK_REALTIME, &rootetime);

  clock_gettime(CLOCK_REALTIME, &stime);

  MPI_Bcast(&ysize, 1, MPI_INT, 0, com);
  MPI_Bcast(&xsize, 1, MPI_INT, 0, com);
  MPI_Bcast(scounts, np, MPI_INT, 0, com);
  MPI_Bcast(displs, np, MPI_INT, 0, com);
  // printf("Processor %d has scounts: %d and displs %d \n", me, scounts[me], displs[me]);


  
  MPI_Scatterv( src, scounts, displs, MPI_PIXEL, recvbuff, scounts[me], MPI_PIXEL, 0, com);  

  
  uint sum;
  for(i = 0, sum = 0; i < scounts[me]; i++) {
    sum += (uint)recvbuff[i].r + (uint)recvbuff[i].g + (uint)recvbuff[i].b;
  }
  sum /= scounts[me];

  uint globalsum;

  //printf("Processor %d has sum %d\n", me, sum);
  MPI_Reduce(&sum, &globalsum, 1, MPI_UNSIGNED , MPI_SUM, 0, com); 
  

    
    

  if(me==0)
    { 
      globalsum /= np;
      // printf("Processor %d has reduced sum %d\n", me, globalsum);
      
      // 
     
    }
  MPI_Bcast(&globalsum, 1, MPI_UNSIGNED, 0, com);

  thresfilter(xsize, scounts[me]/xsize, recvbuff, globalsum);
  
  MPI_Gatherv(recvbuff, scounts[me], MPI_PIXEL , src, scounts, displs, MPI_PIXEL, 0, com);


  if(me == 0){
    write_ppm (argv[2], xsize, ysize, (char *)src);
    printf("Root reading and calculating work took: %2g secs\n", (rootetime.tv_sec  - rootstime.tv_sec) +
	   1e-9*(rootetime.tv_nsec  - rootstime.tv_nsec)) ;
  }

  clock_gettime(CLOCK_REALTIME, &etime);

  
  printf("Filtering for process %d took: %2g secs\n", me ,(etime.tv_sec  - stime.tv_sec) +
	 1e-9*(etime.tv_nsec  - stime.tv_nsec)) ;


  MPI_Finalize();
  






}
