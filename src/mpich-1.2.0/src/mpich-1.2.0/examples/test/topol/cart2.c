#include "mpi.h"
#include <stdio.h>
#include "test.h"

#define NUM_DIMS 2

int main( int argc, char **argv )
{
    int              rank, size, i;
    int              dims[NUM_DIMS];
    int              periods[NUM_DIMS];
    int              new_coords[NUM_DIMS];
    int              new_new_coords[NUM_DIMS];
    int              reorder = 1;
    MPI_Comm         comm_cart;

    MPI_Init( &argc, &argv );

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    /* Clear dims array and get dims for topology */
    for(i=0;i<NUM_DIMS;i++) { dims[i] = 0; periods[i] = 0; }
    MPI_Dims_create ( size, NUM_DIMS, dims );

    /* Make a new communicator with a topology */
    MPI_Cart_create ( MPI_COMM_WORLD, 2, dims, periods, reorder, &comm_cart );

    /* Does the mapping from rank to coords work */
    MPI_Cart_coords ( comm_cart, rank, NUM_DIMS, new_coords ); 

    /* 2nd call to Cart coords gives us an error - why? */
    MPI_Cart_coords ( comm_cart, rank, NUM_DIMS, new_new_coords ); /***34***/ 

    MPI_Comm_free( &comm_cart );
    Test_Waitforall( );
    MPI_Finalize();
    
    return 0;
}
