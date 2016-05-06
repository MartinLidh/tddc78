#include "mpi.h"
#include <stdio.h>

#define BUFLEN 512

void main(argc,argv)
int argc;
char *argv[];
{
    int myid, numprocs, next, namelen, i, numloops = 5;
    char buffer[BUFLEN], processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Get_processor_name(processor_name,&namelen);

    fprintf(stderr,"Process %d on %s\n", myid, processor_name);
    strcpy(buffer,"hello there");
    MPI_Barrier(MPI_COMM_WORLD);

    next = (myid + 1) % numprocs;

    if (myid == 0)
    {
	MPI_Send(buffer, strlen(buffer)+1, MPI_CHAR, next, 99, MPI_COMM_WORLD);

	for (i = 0; i < numloops - 1; i++) {
	    MPI_Recv(buffer, BUFLEN, MPI_CHAR, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD,
		     &status);
	    MPI_Send(buffer, strlen(buffer)+1, MPI_CHAR, next, 99, MPI_COMM_WORLD);
	}

	MPI_Recv(buffer, BUFLEN, MPI_CHAR, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD,
		 &status);
    }
    else
    {
	for (i = 0; i < numloops; i++) {
	    MPI_Recv(buffer, BUFLEN, MPI_CHAR, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD,
		     &status);
	    MPI_Send(buffer, strlen(buffer)+1, MPI_CHAR, next, 99, MPI_COMM_WORLD);
	}
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}
