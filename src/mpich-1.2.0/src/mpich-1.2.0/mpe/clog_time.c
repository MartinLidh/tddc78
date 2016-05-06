#include "mpeconf.h"
#include "clog_time.h"
#include <stdio.h>

static double clog_time_offset;

void CLOG_timeinit()
{
    double   local_time;
    int      flag, *is_globalp;

    PMPI_Initialized(&flag);
    if (!flag)
        PMPI_Init(0,0);

    PMPI_Attr_get( MPI_COMM_WORLD, MPI_WTIME_IS_GLOBAL, &is_globalp, &flag );
    if ( !flag || (is_globalp && !*is_globalp) )
        /*  Clocks are NOT synchronized  */
        clog_time_offset = PMPI_Wtime();
    else {
        /*  Clocks are synchronized  */
        local_time = PMPI_Wtime();
        PMPI_Allreduce( &local_time, &clog_time_offset, 1, MPI_DOUBLE,
                        MPI_COMM_WORLD, MPI_MAX );
        /*  clog_time_offset should be a globally known value  */
        /*
        printf( "local_time = %.20E\n", local_time );
        printf( "clog_time_offset = %.20E\n", clog_time_offset );
        */
    }
        
}

double CLOG_timestamp()
{
    return ( PMPI_Wtime() - clog_time_offset );
}
