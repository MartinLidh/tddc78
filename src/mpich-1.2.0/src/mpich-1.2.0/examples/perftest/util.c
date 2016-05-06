#include <stdio.h>

#include "mpi.h"
extern int __NUMNODES, __MYPROCID;

/*
   Utility programs for mpptest
 */

/* 
    T1 is time to send len1,
    T2 is time to send len2, 
    len is lenght we'd like the number of repititions for
    reps (input) is the default to use
 */
int GetRepititions( T1, T2, Len1, Len2, len, reps )
double T1, T2;
int    Len1, Len2, len, reps;
{
    if (__MYPROCID == 0) {
	if (T1 > 0 && T2 > 0) 
	    reps = ComputeGoodReps( T1, Len1, T2, Len2, len );
    }
    MPI_Bcast(&reps, 1, MPI_INT, 0, MPI_COMM_WORLD );
    return reps;
}
