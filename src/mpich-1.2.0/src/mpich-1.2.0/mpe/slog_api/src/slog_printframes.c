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
          int              ierr;

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

    if ( 1 ) {
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
            printf( "%d: ", MaxIrec );
            irec = SLOG_Irec_Copy( const_irec );
            SLOG_Irec_Print( irec, stdout ); fprintf( stdout, "\n" );
            SLOG_Irec_Free( irec );
            MaxIrec++;
        }
    }
    else {
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
            printf( "%d: ", MaxIrec );
            SLOG_Irec_Print( const_irec, stdout ); fprintf( stdout, "\n" );
            MaxIrec++;
        }
    }

    printf( "Finish reading records :\n" );

    SLOG_CloseInputStream( slog );

    return 0;
}
