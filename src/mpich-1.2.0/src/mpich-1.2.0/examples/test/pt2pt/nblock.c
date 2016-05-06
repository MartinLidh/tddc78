#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#if defined(NEEDS_STDLIB_PROTOTYPES)
#include "protofix.h"
#endif

/*
   Test to make sure that nonblocking routines actually work.  This
   stresses them by sending large numbers of requests and receiving them
   piecemeal.
 */
int main( int argc, char **argv )
{
    int count, tag, nsend, myid, np, rcnt, scnt, i, j, *buf;
    MPI_Status status;
    MPI_Request *rsend, *rrecv;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &myid );
    MPI_Comm_size( MPI_COMM_WORLD, &np );

    nsend = 3 * np;
    rsend = (MPI_Request *) malloc ( nsend * sizeof(MPI_Request) );
    rrecv = (MPI_Request *) malloc ( nsend * sizeof(MPI_Request) );
    if (!rsend || !rrecv) {
	fprintf( stderr, "Failed to allocate space for requests\n" );
	MPI_Abort( MPI_COMM_WORLD, 1 );
    }

    for (count = 1; count < 10000; count *= 2) {
	buf   = (int *)malloc( count * sizeof(int) );
	/* We'll send/recv from everyone */
	scnt = 0;
	rcnt = 0;
	for (j=0; j<3; j++) {
	    tag = j;
	    for (i=0; i<np; i++) {
		if (i != myid) 
		    MPI_Isend( buf, count, MPI_INT, i, tag, MPI_COMM_WORLD, 
			       &rsend[scnt++] );
	    }
	    for (i=0; i<np; i++) {
		if (i != myid) 
		    MPI_Irecv( buf, count, MPI_INT, i, tag, MPI_COMM_WORLD, 
			       &rrecv[rcnt++] );
	    }
	}
	/* In general, it would be better to use MPI_Waitall, but this should
	   work as well */
	for (i=0; i<rcnt; i++) {
	    MPI_Wait( &rrecv[i], &status );
	}
	for (i=0; i<scnt; i++) {
	    MPI_Wait( &rsend[i], &status );
	}
	free( buf );

	MPI_Barrier( MPI_COMM_WORLD );
	if (myid == 0 && (count % 64) == 0) {
	    printf( "All processes completed for count = %ld ints of data\n", 
		    (long)count );
	    fflush(stdout);
	}
    }
    MPI_Finalize();
    return 0;
}


