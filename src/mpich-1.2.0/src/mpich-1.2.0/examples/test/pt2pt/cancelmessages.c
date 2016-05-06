/*
 * This file tests to see if short,eager,and rndv messages can all be 
 * successfully cancelled.  If they cannot be cancelled, then the 
 * program still must successfully complete.
 */

#include "mpi.h"
#include <stdio.h>

#if defined(NEEDS_STDLIB_PROTOTYPES)
#include "protofix.h"
#endif

int main( argc, argv )
int argc;
char **argv;
{

    double       sbuf[20000], rbuf[20000];
    int          rank;
    int          n, flag, size;
    int          err = 0;
    MPI_Status   status;
    MPI_Request  req;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    if (size < 2) {
	printf( "Cancel test requires at least 2 processes\n" );
	MPI_Abort( MPI_COMM_WORLD, 1 );
    }

    /* Short Message Test */
    n = 200;

    if (rank == 1) { /* begin if rank = 1 */
	MPI_Isend( &sbuf, n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &req );
	MPI_Cancel(&req); 
	MPI_Wait(&req, &status);
	MPI_Test_cancelled(&status, &flag);
	if (!flag) {
	    err++;
	    printf( "Cancelling a short message failed where it should succeed.\n" );
	}
	MPI_Request_free( &req );
    }  /* end if rank == 1 */

    MPI_Barrier(MPI_COMM_WORLD); 

    if (rank == 0) {  /* begin if rank == 0 */
	MPI_Recv( &rbuf, n, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD, &status);
    }  /* end if rank = 0 */
    else if (rank == 1) { /* begin if rank = 1 */
	MPI_Isend( &sbuf, n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &req );
	MPI_Cancel(&req); 
	MPI_Wait(&req, &status);
	MPI_Test_cancelled(&status, &flag);
	if (status.MPI_ERROR != MPI_SUCCESS) {
	    err++;
	    printf( "Cancel of a send returned an error in the status field.\n" );
	}
	  /* end if status.MPI_ERROR */
	if (!flag)
	    MPI_Request_free( &req ); 
    }  /* end if rank == 1 */

    MPI_Barrier(MPI_COMM_WORLD);

    /* Eager Message Test */
    n = 3000;

    if (rank == 1) { /* begin if rank = 1 */
	MPI_Isend( &sbuf, n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &req );
	MPI_Cancel(&req);
	MPI_Wait(&req, &status);
	MPI_Test_cancelled(&status, &flag);
	if (!flag) {
	    err++;
	    printf( "Cancelling a eager message failed where it should succeed.\n" );
	}
	MPI_Request_free( &req ); 
    }  /* end if rank == 1 */

    MPI_Barrier(MPI_COMM_WORLD); 

    if (rank == 0) {  /* begin if rank == 0 */
	MPI_Irecv(&rbuf, n, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD, &req );
	MPI_Wait( &req, &status);
    }  /* end if rank = 0 */
    else if (rank == 1) { /* begin if rank = 1 */
	MPI_Isend( &sbuf, n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &req );
	MPI_Cancel(&req);
	MPI_Wait(&req, &status);
	MPI_Test_cancelled(&status, &flag);
	if (status.MPI_ERROR != MPI_SUCCESS) {
	    err++;
	    printf( "Cancel of a send returned an error in the status field.\n" );
	}
	/* end if status.MPI_ERROR */
	if (!flag)
	    MPI_Request_free( &req );
    }  /* end if rank == 1 */

    MPI_Barrier(MPI_COMM_WORLD);

    /* Rndv Message Test */
    n = 20000;

    if (rank == 1) { /* begin if rank = 1 */
	MPI_Isend( &sbuf, n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &req );
	MPI_Cancel(&req);
	MPI_Wait(&req, &status);
	MPI_Test_cancelled(&status, &flag);
	if (!flag) {
	    err++;
	    printf( "Cancelling a eager message failed where it should succeed.\n" );
	}
	MPI_Request_free( &req ); 
    }  /* end if rank == 1 */

    MPI_Barrier(MPI_COMM_WORLD); 

    if (rank == 0) {  /* begin if rank == 0 */
	MPI_Irecv(rbuf, n, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD, &req );
	MPI_Wait( &req, &status); 
    }  /* end if rank = 0 */
    else if (rank == 1) { /* begin if rank = 1 */
	MPI_Isend( sbuf, n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &req );
	MPI_Cancel(&req);
	MPI_Wait(&req, &status);
	MPI_Test_cancelled(&status, &flag);
	if (status.MPI_ERROR != MPI_SUCCESS) {
	    err++;
	    printf( "Cancel of a send returned an error in the status field.\n" );
	}
	/* end if status.MPI_ERROR */
	if (!flag)
	    MPI_Request_free( &req );
    }  /* end if rank == 1 */

    MPI_Barrier(MPI_COMM_WORLD); 

    if (rank == 1) {  /* begin if rank = 1 */
	if (err) {
	    printf( "Test failed with %d errors.\n", err );
	}
	else {
	    printf( "Test passed\n" );
	}
    }

    MPI_Finalize( );

    return 0;
}
