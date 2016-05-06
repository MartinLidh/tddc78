#include <ctype.h>
#include <stdio.h>
#include "slog_fileio.h"
#include "slog.h"
#include "slog_impl.h"
#include "slog_bbuf.h"

#ifdef HAVE_SLOGCONF_H
#include "slog_config.h"
#endif
#if defined( HAVE_UNISTD_H )
#    include <unistd.h>
#endif

#define  MaxAssocs 10
#define  MaxArgs   10

#define  NaN         0

int main( int argc, char **argv )
{
          char slog_name[80] = "slogfile.data";
          SLOG_STREAM     *slog;
    const char             FileSeparator = '/';   
          int              name_char;
          int              frame_idx;
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
#endif    

    printf( "Preview Statistics: \n" );
    SLOG_PSTAT_Print( stdout, slog->pstat );
    printf( "\n" );

#if ! defined( DEBUG )
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

    /*
        Initialize the SLOG
    */
        slog->HasIrec2IOStreamBeenUsed = SLOG_TRUE;
        if ( SLOG_STM_CreateBbufs( slog ) != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":Main(): slog_printbbuf - "
                              "SLOG_STM_CreateBbufs() fails\n" );
            fflush( errfile );
            exit( 1 );
        }

        /*  Temporary put slog_fseek( fptr2framedata ) here  */
        if ( slog->hdr->fptr2framedata == SLOG_fptr_NULL ) {
            fprintf( errfile, __FILE__":Main(): slog_printbbuf - \n" );
            fprintf( errfile, "\t""slog->hdr->fptr2framedata is NULL \n" );
            fprintf( errfile, "\t""SLOG_Irec_ToOutputStream() has NOT been "
                              "called\n" );
            fflush( errfile );
            ierr = SLOG_EOF;
            exit( 1 );
        }
        ierr = slog_fseek( slog->fd, slog->hdr->fptr2framedata, SEEK_SET );
        if ( ierr != 0 ) {
            fprintf( errfile, __FILE__":Main(): slog_printbbuf - "
                              "slog_fseek( fptr2framedata ) fails\n" );
            fflush( errfile );
            ierr = SLOG_FAIL;
            exit( 1 );
        }

        /*
           It is necessary to call SLOG_STM_Init() _BEFORE_ SLOG_ReadFrameDir().
           SLOG_STM_Init() initializes the Frame_Dir components and various
           Bbufs in the SLOG_STREAM.  Then SLOG_ReadFrameDir() can update
           Frame_Dir compoents afterward.
        */
        SLOG_STM_Init( slog );

        if ( SLOG_ReadFrameDir( slog ) != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":Main(): slog_printbbuf - "
                              "SLOG_ReadFrameDir( slog ) fails\n" );
            fflush( errfile );
            ierr = SLOG_FAIL;
            exit( 1 );
        }
        printf( "The Frame Directory Header:\n" );
        SLOG_STM_PrintFrameDirHdr( slog, stdout );
        printf( "\n" );
        printf( "The Frame Directory Non-empty Entries:\n" );
        SLOG_STM_PrintFrameDirEntries( slog, stdout );
        printf( "\n" );


    printf( "Start reading records :\n" );
    printf( "\n" );

    /*
        Going Through All the frames
    */
        for ( frame_idx = 0;
              ( ierr = SLOG_STM_ReadFRAME( slog ) ) > 0;
              frame_idx++ ) {
            fprintf( stdout, "****  Frame Index = %d  ****\n", frame_idx );

            fprintf( stdout, "---- The INCOMING sector:\n" );
            SLOG_Bbuf_Print( slog->inc_bbuf, stdout ); 
            fprintf( stdout, "---- The PASSING THROUGH sector:\n" );
            SLOG_Bbuf_Print( slog->pas_bbuf, stdout ); 
            fprintf( stdout, "---- The CURRENT sector:\n" );
            SLOG_Bbuf_Print( slog->cur_bbuf, stdout ); 
            fprintf( stdout, "---- The OUTGOING sector:\n" );
            SLOG_Bbuf_Print( slog->out_bbuf, stdout ); 
            fprintf( stdout, "\n" );

            SLOG_Bbuf_DelAllNodes( slog->cur_bbuf );
            SLOG_Bbuf_DelAllNodes( slog->inc_bbuf );
            SLOG_Bbuf_DelAllNodes( slog->pas_bbuf );
            SLOG_Bbuf_DelAllNodes( slog->out_bbuf );
            SLOG_Bbuf_Init( slog->cur_bbuf );
            SLOG_Bbuf_Init( slog->inc_bbuf );
            SLOG_Bbuf_Init( slog->pas_bbuf );
            SLOG_Bbuf_Init( slog->out_bbuf );
        }   /*  Endof for ( frame_idx = 0; .... )  */

        if ( ierr == SLOG_FAIL ) {
            fprintf( errfile, __FILE__":Main(): slog_printbbuf - "
                              "Unexpected termination of "
                              "SLOG_STM_ReadFRAME()\n" );
            fflush( errfile );
            exit( 1 );
        }
    
    printf( "Finish reading records :\n" );

    SLOG_CloseInputStream( slog );

    return 0;
}
