#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "ppmio.h"


typedef struct _pixel {
  unsigned char r,g,b;
} pixel;
typedef struct _workdata {
  int offstart, workstart,worksize;
} workdata;


int main( int argc, char *argv[] )
{
  if(argc<1)
    {
      printf("invalid number of args");
      return;
    }
  int radius = 1000;
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
  

  
  MPI_Aint offsets[1] = {0};
  MPI_Datatype oldtypes[1] = {MPI_INT};
  int blockcounts[1] = {3};

  MPI_Datatype workstruct;

  MPI_Type_struct(1, blockcounts,offsets, oldtypes,&workstruct);
  MPI_Type_commit(&workstruct);

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


  MPI_Bcast(scounts, np, MPI_INT, 0, com);
  MPI_Bcast(displs, np, MPI_INT, 0, com);
  recvbuff = (pixel *)malloc((scounts[me]+radius*xpix*2)*sizeof(pixel)); 
  printf("0: %d 1: %d 2: %d 3: %d\n", scounts[0], scounts[1],scounts[2], scounts[3]);  





  //recive overlapping data
  int needed = xpix*radius;
  for(i = 1; needed > 0; i++)
    {
      if(me-i >= 0)
	{
	  if(scounts[me-i] > needed){
	    MPI_Send
	    MPI_Recv(recvbuff, needed, MPI_INT, me-i, MPI_ANY_TAG, com, MPI_STATUS_IGNORE);
	    needed-=needed;
	    
	  }
	  else{
	    MPI_Send(&recvbuff[xpix*radius], scounts[me], MPI_INT, me-i, MPI_ANYTAG_TAG,com,MPI_STATUS_IGNORE);
	    MPI_Recv(&recvbuff[needed-scounts[me-i]], scounts[me-i],MPI_INT, me-i,MPI_ANY_TAG,com,MPI_STATUS_IGNORE);
	    needed-=scounts[me-i];
	  }
	}
    }
  needed=xpix*radius;
  for(i = 1; needed > 0; i++){
    if(me+i < np)
      {
	if(scounts[me+i] > needed){
	  MPI_Recv(&recvbuff[radius*xpix*2+scounts[me]-needed], needed, MPI_INT, me+i, MPI_ANY_TAG, com, MPI_STATUS_IGNORE);
	  needed-=needed;	    
	}
	else{
	  MPI_Recv(&recvbuff[radius*xpix*2+scounts[me]-needed], scounts[me+i],MPI_INT, me+i ,MPI_ANY_TAG,com,MPI_STATUS_IGNORE);
	  needed-=scounts[me+i];
	}
      }
  }
    

  
  /*	  
  //get overlap above area
  if(displs[i]-(xpix*radius)<=0)
  {
  scounts[i]+= displs[i];
  displs[i]=0;
  }else{
  scounts[i]+=(xpix*radius);
  displs[i]-=xpix*radius;
  }
  wdata[i].offstart = displs[i];
  //get overlap for lower area.
  if(displs[i]+(xpix*radius)+scounts[i] > xpix*ypix)
  {
  scounts[i]=xpix*ypix-displs[i];
  }
  else{
  scounts[i]+=xpix*radius;
  }

  } */

  MPI_Scatterv( picture, scounts, displs, MPI_CHAR, &recvbuff[xpix*radius], scounts[me], MPI_CHAR, 0, com); 

  printf("p %d got pixel %d, %d, %d\n", me, recvbuff[0].r,recvbuff[0].g,recvbuff[0].b);
  
  MPI_Gatherv(recvbuff, scounts[me], MPI_CHAR , picture, scounts, displs, MPI_CHAR, 0, com);
  
  if(me==0)
    {
      printf("root had pitcure with pixel %d\n", picture[0].r);
    }


  
 
  MPI_Finalize();
  free(picture);
  free(recvbuff);

 
  return 0;


}

