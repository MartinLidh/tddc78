#include <mpi.h>
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

int main( int argc, char *argv[] )
{
  if(argc<1)
    {
      printf("invalid number of args");
      return;
    }
  int radius = 100;
  int xpix, ypix, me, np;
  int maxpix;
  pixel* picture;
  pixel * recvbuff;
  picture = (pixel *)malloc((MAX_PIXELS*3)*sizeof(pixel*)); 


  MPI_Comm com = MPI_COMM_WORLD;
  MPI_Init( &argc, &argv );
  MPI_Comm_size( com, &np );
  MPI_Comm_rank( com, &me );
  int scounts[np];
  int displs[np];
  int workload;
  int work2;
  int i;

  MPI_Request sendReqs[np];
  MPI_Status sendStats[np];




  MPI_Aint offsets[1] = {0};
  MPI_Datatype oldtypes[1] = {MPI_CHAR};
  int blockcounts[1] = {3};

  

  MPI_Datatype MPI_PIXEL;

  MPI_Type_struct(1, blockcounts,offsets, oldtypes,&MPI_PIXEL);
  MPI_Type_commit(&MPI_PIXEL);

  if(me==0)
    {	
      read_ppm(argv[1], &xpix, &ypix, &maxpix, (char*)picture);
    
      workload = (ypix/np)*xpix;
      work2= ypix%np;
      int offset = 0;
      for(i=0; i<np; i++)
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
    }
  int recvcount = scounts[me];

  MPI_Bcast(&ypix, 1, MPI_INT, 0, com);
  MPI_Bcast(&xpix, 1, MPI_INT, 0, com);
  MPI_Bcast(scounts, np, MPI_INT, 0, com);
  MPI_Bcast(displs, np, MPI_INT, 0, com);


  int over, under;

  over = min(displs[me]/xpix, radius)*-1;
  under = min((xpix*ypix-displs[me]-scounts[me])/xpix, radius);


  recvbuff = (pixel *)malloc((scounts[me]+(radius*xpix*2))*sizeof(pixel)); 
  printf("0: %d 1: %d 2: %d 3: %d\n", scounts[0], scounts[1],scounts[2], scounts[3]);  

  MPI_Scatterv( picture, scounts, displs, MPI_PIXEL, &recvbuff[xpix*radius], scounts[me], MPI_PIXEL, 0, com);  
  double w[MAX_RAD];
  get_gauss_weights(radius, w);
  blurfilterX(xpix, scounts[me]/xpix, &recvbuff[xpix*radius], radius, w );
 

 
  printf("xpix %d, ypix %d \n", xpix, ypix);
  
  pixel * buf = (pixel *)malloc((np*scounts[0]*2)*sizeof(pixel));
  int bufsize = (np*scounts[0]*2)*sizeof(pixel) + 2 * MPI_BSEND_OVERHEAD;
  MPI_Buffer_attach(buf, bufsize);


  //send overlapping data
  int needed = xpix*radius;

  int reqIndex = 0;
  for(i = 1; needed > 0; i++)
    {
      if(me-i >= 0)
	{
	  if(scounts[me-i] > needed){		    
	    printf("Proc with id %d is sending to %d and number of pixs is:%d \n",me,me-i, needed);
	    MPI_Ibsend(&recvbuff[xpix*radius], needed, MPI_PIXEL, me-i,1337, com, &sendReqs[reqIndex++] );
	    needed-=needed;  
	  }
	  else{
	    printf("Proc with id %d is sending to %d and number of pixs is:%d \n",me,me-i, needed);

	    MPI_Ibsend(&recvbuff[xpix*radius], scounts[me], MPI_PIXEL, me-i, 1337,com, &sendReqs[reqIndex++]);
	    needed-=scounts[me-i];
	  }
	}
      else
	break;
    }
  needed=xpix*radius;
  for(i = 1; needed > 0; i++){
    if(me+i < np)
      {
	if(scounts[me+i] > needed){
	  printf("Proc with id %d is sending to %d and number of pixs is: %d\n",me,me+i, needed);
	  MPI_Ibsend(&recvbuff[xpix*radius+scounts[me]-needed], needed, MPI_PIXEL, me+i, 1337,com, &sendReqs[reqIndex++]);
	  needed-=needed;
	}
	else{
	  printf("Proc with id %d is sending to %d and number of pixs is:%d \n",me,me-i, needed);

	  MPI_Ibsend(&recvbuff[xpix*radius], scounts[me], MPI_PIXEL, me+i, 1337,com, &sendReqs[reqIndex++]);
	  needed-=scounts[me+i];
	}
      }else break;
	  
  }
  
  
  //recive overlapping data
  needed = xpix*radius;
  
  for(i = 1; needed > 0; i++)
    {
      if(me-i >= 0)
	{	  
	  if(scounts[me-i] > needed){
	    printf("Proc with id %d is receiving from %d now. Number of pixs: %d\n",me,me-i, needed);
	    MPI_Recv(recvbuff, needed, MPI_PIXEL, me-i, MPI_ANY_TAG, com, MPI_STATUS_IGNORE);
	    needed-=needed;
	  }
	  else{
	    printf("Proc with id %d is sending to %d and number of pixs is:%d \n",me,me-i, needed);

	    MPI_Recv(&recvbuff[needed-scounts[me-i]], scounts[me-i],MPI_PIXEL, me-i, MPI_ANY_TAG,com, MPI_STATUS_IGNORE);
	    needed-=scounts[me-i];
	  }
	}else break;
    }
  needed=xpix*radius;
  for(i = 1; needed > 0; i++){
    if(me+i < np)
      {
	if(scounts[me+i] > needed){
	  printf("Proc with id %d is receiving from %d now. Number of pixs: %d\n",me,me+i, needed);
	  MPI_Recv(&recvbuff[(radius*xpix*2)+scounts[me]-needed], needed, MPI_PIXEL, me+i, MPI_ANY_TAG, com, MPI_STATUS_IGNORE);
	  needed-=needed;	    
	}
	else{
	  printf("Proc with id %d is sending to %d and number of pixs is:%d \n",me,me+i, needed);

	  MPI_Recv(&recvbuff[radius*xpix*2+scounts[me]-needed], scounts[me+i],MPI_PIXEL, me+i ,MPI_ANY_TAG,com,MPI_STATUS_IGNORE);
	  needed-=scounts[me+i];
	}
      }else break;
  }



  //blur pixels
  

  blurfilterY(xpix, scounts[me]/xpix, &recvbuff[xpix*radius], radius, w ,over, under);
  printf("0: %d 1: %d 2: %d 3: %d\n", displs[0], displs[1],displs[2], displs[3]);  


  MPI_Gatherv(&recvbuff[radius*xpix], scounts[me], MPI_PIXEL , picture, scounts, displs, MPI_PIXEL, 0, com);
  /*
  if(me == 1){
    MPI_Send(&recvbuff[xpix*radius], scounts[me], MPI_PIXEL, 0, 1337,com);
  }
  if(me == 0){
    MPI_Recv(&picture[scounts[me]], scounts[me],MPI_PIXEL, 1 ,MPI_ANY_TAG,com,MPI_STATUS_IGNORE);
    memcpy(picture, &recvbuff[xpix*radius], scounts[me]*sizeof(pixel));
    printf("pixelsize:  %lu \n", sizeof(pixel));
    }
  */
  //printf("p %d got pixel %d, %d, %d\n", me, recvbuff[xpix*radius].r,recvbuff[xpix*radius].g,recvbuff[xpix*radius].b);
  
  
  if(me==0)
    { 
      write_ppm (argv[2], xpix, ypix, (char *)picture);
      //  write_ppm("../img_node_0.ppm", xpix, (scounts[me]+xpix*radius)/xpix, (char *)recvbuff);
    }
  /*
  if(me==np)
    {
      char filename [30];
      sprintf(filename, "../img_node_%d.ppm", me);
      write_ppm(filename, xpix, (scounts[me]+xpix*radius)/xpix, (char *)recvbuff);
      }
  
  */
  if(1)
    {
      char filename [30];
      sprintf(filename, "../img_node2_%d.ppm", me);
      write_ppm(filename, xpix, (scounts[me]+xpix*radius*2)/xpix, (char *)recvbuff);
    }

  
  MPI_Finalize();
  free(picture);
  free(recvbuff);

 
  return 0;


}

