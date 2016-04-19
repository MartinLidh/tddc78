 MPI_Bcast(&ypix, 1, MPI_INT, 0, com);
  MPI_Bcast(&xpix, 1, MPI_INT, 0, com);

  MPI_Datatype col,coltype;
  
  MPI_Type_vector(ypix, 1, xpix, MPI_INT, &col);
  MPI_Type_commit(&col);

  MPI_Type_create_resized(col, 0, 1*sizeof(int), &coltype);
  MPI_Type_commit(&coltype);
  
  if(me == 0){
    workload = xpix/np;
    work2 = xpix%np;
    int offset = 0;
    for(i=0; i<np; i++)
      {
	displs[i]=offset;
	if(i<work2)
	  {
	    scounts[i] = workload+1;	
	  }
	else{
	  scounts[i] = workload;
	}
	offset += scounts[i];
      }

  }

  
  MPI_Bcast(scounts, np, MPI_INT, 0, com);
  MPI_Bcast(displs, np, MPI_INT, 0, com); 
  printf("procc %d got  scounts and displs  %d %d\n", me,scounts[me], displs[me]);
      
  
  MPI_Scatterv( picture, scounts, displs, coltype, recvbuff, scounts[me], coltype, 0, com); 

  
  /*
  printf("p %d got pixel %d, %d, %d\n", me, recvbuff[0].r,recvbuff[0].g,recvbuff[0].b);

  MPI_Gatherv(recvbuff, scounts[me], coltype , picture, scounts, displs, coltype, 0, com);
  
  if(me==0)
    {
      printf("root had pitcure with pixel %d\n", picture[0].r);
    }
  */
  
