#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define MAX_REQ 10000

#define DEFAULT_REQ 100
#define DEFAULT_LEN 10000
#define DEFAULT_LOOP 10

int main( int argc, char **argv )
{
    int rank, size, loop, max_loop = DEFAULT_LOOP, max_req = DEFAULT_REQ;
    int buf_len = DEFAULT_LEN;
    int i, j, errs = 0, toterrs;
    MPI_Request r;
    MPI_Status  status;
    int *(b[MAX_REQ]);
    MPI_Datatype dtype;

    MPI_Init( &argc, &argv );

    dtype = MPI_INT;

    /* Check command line args (allow usage even with one processor */
    argv++;
    argc--;
    while (argc--) {
	if (strcmp( "-loop" , *argv ) == 0) {
	    argv++; argc--;
	    max_loop = atoi( *argv );
	}
	else if (strcmp( "-req", *argv ) == 0) {
	    argv++; argc--;
	    max_req = atoi( *argv );
	}
	else if (strcmp( "-len", *argv ) == 0) {
	    argv++; argc--;
	    buf_len = atoi( *argv );
	}
	else {
	    fprintf( stderr, 
		     "Usage: reqfree [ -loop n ] [ -req n ] [ -len n ]\n" );
	    MPI_Abort( MPI_COMM_WORLD, 1 );
	}
	argv++;
    }
    
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    if (size != 2) {
	fprintf( stderr, "This program requires two processes\n" );
	MPI_Abort( MPI_COMM_WORLD, 1 );
    }

    /* Assume only processor 0 has the command line */
    MPI_Bcast( &max_loop, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &max_req, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &buf_len, 1, MPI_INT, 0, MPI_COMM_WORLD );

    /* Allocate buffers */
    for (i=0; i<max_req; i++) {
	b[i] = (int *) malloc(buf_len * sizeof(int) );
	if (!b[i]) {
	    fprintf( stderr, "Could not allocate %dth block of %d ints\n", 
		     i, buf_len );
	    MPI_Abort( MPI_COMM_WORLD, 2 );
	}
	if (rank != 0) break;
	for (j=0; j<buf_len; j++) {
	    b[i][j] = i * buf_len + j;
	}
    }

    /* Loop several times to capture resource leaks */
    for (loop=0; loop<max_loop; loop++) {
	if (rank == 0) {
	    for (i=0; i<max_req; i++) {
		MPI_Isend( b[i], buf_len, dtype, 1, 0, MPI_COMM_WORLD, &r );
		MPI_Request_free( &r ); 
	    }
	    MPI_Barrier( MPI_COMM_WORLD );
	    MPI_Barrier( MPI_COMM_WORLD );
	}
	else {
	    MPI_Barrier( MPI_COMM_WORLD );
	    for (i=0; i<max_req; i++) {
		MPI_Recv( b[0], buf_len, dtype, 0, 0, MPI_COMM_WORLD, 
			  &status );
		for (j=0; j<buf_len; j++) {
		    if (b[0][j] != i * buf_len + j) {
			errs++;
			fprintf( stdout, 
				 "at %d in %dth message, got %d expected %d\n",
				 j, i, b[0][j], i * buf_len + j );
			break;
		    }
		}
	    }
	    MPI_Barrier( MPI_COMM_WORLD );
	}
    }

    MPI_Allreduce( &errs, &toterrs, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD );
    if (rank == 0) {
	if (toterrs == 0) printf( "No errors\n" );
	else              printf( "Found %d errors\n", toterrs );
    }

    MPI_Finalize( );
    return 0;
}

