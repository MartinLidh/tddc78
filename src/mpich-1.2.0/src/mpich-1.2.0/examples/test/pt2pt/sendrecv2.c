#include <mpi.h>
#include <stdio.h>
#include "dtypes.h"
#include "gcomm.h"
#include "test.h"

/*
   This program is from mpich/tsuite/pt2pt and should be changed there only.
   It needs gcomm and dtype from mpich/tsuite, and can be run with 
   any number of processes > 1.
 */
int main( int argc, char **argv)
{
MPI_Datatype *types;
void         **inbufs, **outbufs;
char         **names;
int          *counts, *bytesize, ntype;
MPI_Comm     comms[20];
int          ncomm = 20, rank, np, partner, tag, count;
int          i, j, k, err, world_rank, errloc;
MPI_Status   status;
char         *obuf;

MPI_Init( &argc, &argv );

AllocateForData( &types, &inbufs, &outbufs, &counts, &bytesize, 
		 &names, &ntype );
GenerateData( types, inbufs, outbufs, counts, bytesize, names, &ntype );

MPI_Comm_rank( MPI_COMM_WORLD, &world_rank );
MakeComms( comms, 20, &ncomm, 0 );

/* Test over a wide range of datatypes and communicators */
err = 0;
for (i=0; i<ncomm; i++) {
    if (comms[i] == MPI_COMM_NULL) continue;
    MPI_Comm_rank( comms[i], &rank );
    MPI_Comm_size( comms[i], &np );
    if (np < 2) continue;
    tag = i;
    for (j=0; j<ntype; j++) {
	if (world_rank == 0) 
	    fprintf( stdout, "Testing type %s\n", names[j] );
        if (rank == 0) {
	    partner = np - 1;
#if 0
		MPIR_PrintDatatypePack( stdout, counts[j], types[j], 0, 0 );
#endif
            MPI_Send( inbufs[j], counts[j], types[j], partner, tag, comms[i] );
            }
        else if (rank == np-1) {
	    partner = 0;
	    obuf = outbufs[j];
	    for (k=0; k<bytesize[j]; k++) 
		obuf[k] = 0;
            MPI_Recv( outbufs[j], counts[j], types[j], partner, tag, comms[i],
                      &status );
            /* Test correct */
            MPI_Get_count( &status, types[j], &count );
            if (count != counts[j]) {
		fprintf( stderr, 
			"Error in counts (got %d expected %d) with type %s\n",
			 count, counts[j], names[j] );
                err++;
                }
            if (status.MPI_SOURCE != partner) {
		fprintf( stderr, 
			"Error in source (got %d expected %d) with type %s\n",
			 status.MPI_SOURCE, partner, names[j] );
                err++;
                }
            if ((errloc = CheckData( inbufs[j], outbufs[j], bytesize[j] ))) {
		char *p1, *p2;
		fprintf( stderr, 
                  "Error in data with type %s (type %d on %d) at byte %d\n", 
			 names[j], j, world_rank, errloc - 1 );
		p1 = (char *)inbufs[j];
		p2 = (char *)outbufs[j];
		fprintf( stderr, 
			"Got %x expected %x\n", p1[errloc-1], p2[errloc-1] );
                err++;
#if 0
		MPIR_PrintDatatypeUnpack( stderr, counts[j], types[j], 
					  0, 0 );
#endif
                }
            }
	}
    }
if (err > 0) {
    fprintf( stderr, "%d errors on %d\n", err, rank );
    }
FreeDatatypes( types, inbufs, outbufs, counts, bytesize, names, ntype );
FreeComms( comms, ncomm );
MPI_Finalize();
return err;
}
