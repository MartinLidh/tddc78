#include <stdio.h>
#include "slog_tasklabel.h"


void SLOG_TaskID_Assign(       SLOG_tasklabel_t *task,
                         const SLOG_nodeID_t     in_node,
                         const SLOG_cpuID_t      in_cpu,
                         const SLOG_threadID_t   in_thread )
{
    task->node   = in_node;
    task->cpu    = in_cpu;
    task->thread = in_thread;
}

void SLOG_TaskID_Retrieve( const SLOG_tasklabel_t *task,
                                 SLOG_nodeID_t    *node,
                                 SLOG_cpuID_t     *cpu,
                                 SLOG_threadID_t  *thread )
{
    *node   = task->node;
    *cpu    = task->cpu;
    *thread = task->thread;
}

void SLOG_TaskID_Copy(       SLOG_tasklabel_t *dest,
                       const SLOG_tasklabel_t *src )
{
    dest->node   = src->node;
    dest->cpu    = src->cpu;
    dest->thread = src->thread;
}

int SLOG_TaskID_IsEqualTo( const SLOG_tasklabel_t *task1, 
                           const SLOG_tasklabel_t *task2 )
{
    return(    task1->node   == task2->node
            && task1->cpu    == task2->cpu
            && task1->thread == task2->thread );
}

void SLOG_TaskID_Print( const SLOG_tasklabel_t *task, FILE *fd )
{
    fprintf( fd, "( " fmt_nodeID_t " " fmt_cpuID_t " " fmt_thID_t " )",
             task->node, task->cpu, task->thread );
}

int SLOG_TaskID_DepositToFbuf( const SLOG_tasklabel_t *task,
                                     filebuf_t        *fbuf )
{
    SLOG_uint32         Nbytes_written;
    int                 ierr;

    Nbytes_written = 0;

    ierr = fbuf_deposit( &( task->node ), SLOG_typesz[ nodeID_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_TaskID_DepositToFbuf() - Cannot "
                          "deposit the NODE_ID to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fprintf( errfile, "      ""task label = " );
        SLOG_TaskID_Print( task, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ nodeID_t ];

#if ! defined( NOCPUID )
    ierr = fbuf_deposit( &( task->cpu ), SLOG_typesz[ cpuID_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_TaskID_DepositToFbuf() - Cannot "
                          "deposit the CPU_ID to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fprintf( errfile, "      ""task label = " );
        SLOG_TaskID_Print( task, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ cpuID_t ];
#endif

    ierr = fbuf_deposit( &( task->thread ), SLOG_typesz[ thID_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_TaskID_DepositToFbuf() - Cannot "
                          "deposit the THREAD_ID to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fprintf( errfile, "      ""task label = " );
        SLOG_TaskID_Print( task, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ thID_t ];

    return Nbytes_written;
}

int SLOG_TaskID_WithdrawFromFbuf( SLOG_tasklabel_t *task,
                                  filebuf_t        *fbuf )
{
    SLOG_uint32      Nbytes_read;
    int              ierr;

    Nbytes_read = 0;

    ierr = fbuf_withdraw( &( task->node ),
                          SLOG_typesz[ nodeID_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_TaskID_WithdrawFromFbuf() - Cannot "
                          "withdraw the NODE_ID from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fprintf( errfile, "      ""task label = " );
        SLOG_TaskID_Print( task, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[ nodeID_t ];

#if ! defined( NOCPUID )
    ierr = fbuf_withdraw( &( task->cpu ), SLOG_typesz[ cpuID_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_TaskID_WithdrawFromFbuf() - Cannot "
                          "withdraw the CPU_ID from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fprintf( errfile, "      ""task label = " );
        SLOG_TaskID_Print( task, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[ cpuID_t ];
#else
    task->cpu = 0;
#endif

    ierr = fbuf_withdraw( &( task->thread ),
                          SLOG_typesz[ thID_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_TaskID_WithdrawFromFbuf() - Cannot "
                          "withdraw the THREAD_ID from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fprintf( errfile, "      ""task label = " );
        SLOG_TaskID_Print( task, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[ thID_t ];

    return Nbytes_read;
}
