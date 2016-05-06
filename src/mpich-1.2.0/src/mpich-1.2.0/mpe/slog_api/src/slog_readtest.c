#include <ctype.h>
#include <stdio.h>
#include "slog.h"
#include "slog_impl.h"

#define  MaxAssocs 10
#define  MaxArgs   10

#define  NaN         0

int main( int argc, char **argv )
{
          char slog_name[80] = "slogfile.data";
          SLOG_STREAM     *slog;
    const SLOG_intvlrec_t *const_irec;
          SLOG_intvlrec_t *irec;
    const char             FileSeparator = '/';
          int              name_char;
          int              MaxIrec;
          int              ii, ierr;

    SLOG_rectype_t   irec_rectype;
    SLOG_intvltype_t irec_intvltype;
    SLOG_starttime_t irec_starttime;
    SLOG_duration_t  irec_duration;
    SLOG_bebit_t     irec_bebits[2];
    SLOG_nodeID_t    irec_node_id;
    SLOG_cpuID_t     irec_cpu_id;
    SLOG_threadID_t  irec_thread_id;
    SLOG_where_t     irec_where;

    SLOG_N_assocs_t  irec_Nassocs;
    SLOG_assoc_t     irec_assocs[ MaxAssocs ];
    SLOG_N_args_t    irec_Nargs;
    SLOG_arg_t       irec_args[ MaxArgs ];

    if ( argc > 1 ) {
        printf( "argv[1] = _%s_\n", argv[1] );
        name_char = (int) argv[1][0];
        if (  isalnum( name_char )
           || name_char == FileSeparator || name_char == '.' )
            strcpy( slog_name, argv[1] );
    }

    slog = SLOG_OpenInputStream( slog_name );
    if ( slog == NULL ) {
        printf( "SLOG_OpenInputStream() returns NULL\n" );
        exit(1);
    }

#if ! defined( DEBUG )
    printf( "The Header :\n" );
    SLOG_HDR_Print( stdout, slog->hdr );
    printf( "\n" );

    printf( "The Preview :\n" );
    SLOG_PVIEW_Print( stdout, slog->pview );
    printf( "\n" );

    printf( "The Display Profile :\n" );
    SLOG_PROF_Print( stdout, slog->prof );
    printf( "\n" );
#endif

    printf("\n" );
    printf( "The Thread Table :\n" );
    SLOG_TTAB_Print( stdout, slog->thread_tab );
    printf( "\n" );

    printf("\n" );
    printf( "The Interval Record Definition Table :\n" );
    SLOG_STM_PrintRecDefs( slog, stdout );
    printf( "\n" );

    printf( "Start reading records :\n" );
    printf( "\n" );

    MaxIrec = 0;
    while( ( const_irec  = SLOG_Irec_FromInputStream( slog, &ierr ) ) 
                        != NULL ) {
        if ( MaxIrec == 0 ) {
            printf( "The Frame Directory Header:\n" );
            SLOG_STM_PrintFrameDirHdr( slog, stdout );
            printf( "\n" );
            printf( "The Frame Directory Non-empty Entries:\n" );
            SLOG_STM_PrintFrameDirEntries( slog, stdout );
            printf( "\n" );
        }
        printf( "%u: ",MaxIrec );
        /*
            The following call to copy const_irec to irec is Redundant,
            It is for illustration purpose
        irec = const_irec;
        */
        irec = SLOG_Irec_Copy( const_irec );

        SLOG_Irec_GetMinRec( irec, &irec_rectype, &irec_intvltype,
                             &irec_bebits[0], &irec_bebits[1],
                             &irec_starttime, &irec_duration,
                             &irec_node_id, &irec_cpu_id, &irec_thread_id,
                             &irec_where );
        printf( fmt_rtype_t" ",  irec_rectype );
        printf( fmt_itype_t" ",  irec_intvltype );
        printf( "( "fmt_bebit_t", "fmt_bebit_t" ) ", 
                irec_bebits[0], irec_bebits[1] );
        printf( fmt_stime_t" ",  irec_starttime );
        printf( fmt_dura_t" ",   irec_duration );
        printf( fmt_nodeID_t" ", irec_node_id );
        printf( fmt_cpuID_t" ",  irec_cpu_id );
        printf( fmt_thID_t" ",   irec_thread_id );
        printf( fmt_where_t" ",  irec_where );

        irec_Nassocs = SLOG_Irec_GetAssocs(irec, MaxAssocs, irec_assocs);
        if ( irec_Nassocs > 0 ) {
            printf( "{ " );
            for ( ii = 0; ii < irec->N_assocs; ii++ )
                printf( fmt_assoc_t" ", irec->assocs[ ii ] );
            printf( "} " );
        }

        while( ( irec_Nargs = SLOG_Irec_GetArgs(irec, MaxArgs, irec_args) )
                 > 0 ) {
            printf( "[ " );
            for ( ii = 0; ii < irec_Nargs; ii++ )
                printf( fmt_arg_t" ", irec_args[ii] );
            printf( "] " );
        }
        fprintf( stdout, "\n" );

        /*
            The is again redundant.  
            It is used only if irec is allocated by SLOG_Irec_Copy
        */
        SLOG_Irec_Free( irec );
        MaxIrec++;
    }

    printf( "Finish reading records :\n" );

    SLOG_CloseInputStream( slog );

    return 0;
}
