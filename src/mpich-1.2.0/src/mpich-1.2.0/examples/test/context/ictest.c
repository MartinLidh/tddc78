/* ictest.c */
#include <stdio.h>
#include "mpi.h"
#include "test.h"

int main( int argc, char **argv )
{
  int size, rank, key, his_key, lrank;
  MPI_Comm myComm;
  MPI_Comm myFirstComm;
  MPI_Comm mySecondComm;
  int errors = 0, sum_errors;
  MPI_Status status;
  
  /* Initialization */
  MPI_Init ( &argc, &argv );
  MPI_Comm_rank ( MPI_COMM_WORLD, &rank);
  MPI_Comm_size ( MPI_COMM_WORLD, &size);

  /* Only works for 2 or more processes */
  if (size >= 2) {
    MPI_Comm merge1, merge2, merge3, merge4;

    /* Generate membership key in the range [0,1] */
    key = rank % 2;
    
    MPI_Comm_split ( MPI_COMM_WORLD, key, rank, &myComm );
    MPI_Intercomm_create (myComm, 0, MPI_COMM_WORLD, (key+1)%2, 1, 
                          &myFirstComm );
    MPI_Comm_dup ( myFirstComm, &mySecondComm );
    MPI_Comm_rank( mySecondComm, &lrank );
    his_key = -1;
    
    if (lrank == 0) {
      MPI_Sendrecv (&key,     1, MPI_INT, 0, 0,
                    &his_key, 1, MPI_INT, 0, 0, mySecondComm, &status);
      if (key != (his_key+1)%2)
        errors++;
    }
    
    if (errors)
      printf("[%d] Failed!\n",rank);

    MPI_Intercomm_merge ( mySecondComm, key, &merge1 );
    MPI_Intercomm_merge ( mySecondComm, (key+1)%2, &merge2 );
    MPI_Intercomm_merge ( mySecondComm, 0, &merge3 );
    MPI_Intercomm_merge ( mySecondComm, 1, &merge4 );

    /* Free communicators */
    MPI_Comm_free( &myComm );
    MPI_Comm_free( &myFirstComm );
    MPI_Comm_free( &mySecondComm );
    MPI_Comm_free( &merge1 );
    MPI_Comm_free( &merge2 );
    MPI_Comm_free( &merge3 );
    MPI_Comm_free( &merge4 );
  }
  else 
    printf("[%d] Failed - at least 2 nodes must be used\n",rank);

  MPI_Barrier( MPI_COMM_WORLD );
  MPI_Allreduce( &errors, &sum_errors, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD );
  if (sum_errors > 0) {
      printf( "%d errors on process %d\n", errors, rank );
      }
  else if (rank == 0) {
      printf( "Completed successfully\n" );
      }
  /* Finalize and end! */

  MPI_Finalize();
  return 0;
}










