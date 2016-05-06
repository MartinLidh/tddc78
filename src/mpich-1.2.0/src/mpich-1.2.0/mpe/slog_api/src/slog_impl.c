#include <stdio.h>
#include <stdlib.h>

#include "fbuf.h"
#include "bswp_fileio.h"
#include "slog_fileio.h"
#include "slog_bbuf.h"
#include "slog_bebits.h"
#include "slog_vtrarg.h"
#include "slog_header.h"
#include "slog_preview.h"
#include "slog_pstat.h"
#include "slog_ttable.h"
#include "slog_recdefs.h"
#include "slog_profile.h"
#include "slog_impl.h"


/*  Define the Global data structure  */
SLOG_global_t SLOG_global;

SLOG_uint32 SLOG_typesz[ SLOG_MAX_TYPES ];



/*  Global Static Methdos  */
/*
    Check if the given intvltype has an entry for 
    the number of associations and the number of call arguments
*/
int SLOG_global_IsVarRec( const SLOG_rectype_t  rectype )
{
    return    (    rectype >= SLOG_global.MinRectype4VarDiagRec
                && rectype <= SLOG_global.MaxRectype4VarDiagRec )
           || (    rectype >= SLOG_global.MinRectype4VarOffdRec
                && rectype <= SLOG_global.MaxRectype4VarOffdRec );
}



int SLOG_global_IsOffDiagRec( const SLOG_rectype_t  rectype )
{
    return    (    rectype >= SLOG_global.MinRectype4VarOffdRec
                && rectype <= SLOG_global.MaxRectype4VarOffdRec )
           || (    rectype >= SLOG_global.MinRectype4FixOffdRec
                && rectype <= SLOG_global.MaxRectype4FixOffdRec );
}




/*  Low Level API  */
void SLOG_InitGlobalData( void )
{
  /*  Initialize the GLOBAL constant and variables  */
    /*  Define the byte size of the basic type used in SLOG  */
    SLOG_typesz[ bit  ] = sizeof( SLOG_bit );
    SLOG_typesz[ ui8  ] = sizeof( SLOG_uint8 );
    SLOG_typesz[ ui16 ] = sizeof( SLOG_uint16 );
    SLOG_typesz[ ui32 ] = sizeof( SLOG_uint32 );
    SLOG_typesz[ ui64 ] = sizeof( SLOG_uint64 );
    SLOG_typesz[  ts  ] = sizeof( SLOG_time );
    SLOG_typesz[ fptr ] = sizeof( SLOG_fptr );
    SLOG_typesz[ sbin ] = sizeof( SLOG_statbintype );

    /*  Define the byte size of the basic type used in SLOG's Interval Rec  */
    SLOG_typesz[ sz_t       ] = sizeof( SLOG_bytesize_t );
    SLOG_typesz[ rtype_t    ] = sizeof( SLOG_rectype_t );
    SLOG_typesz[ itype_t    ] = sizeof( SLOG_intvltype_t );
    SLOG_typesz[ bebit_t    ] = sizeof( SLOG_bebit_t );
    SLOG_typesz[ bebits_t   ] = sizeof( SLOG_bebits_t );
    SLOG_typesz[ stime_t    ] = sizeof( SLOG_starttime_t );
    SLOG_typesz[ dura_t     ] = sizeof( SLOG_duration_t );
    SLOG_typesz[ nodeID_t   ] = sizeof( SLOG_nodeID_t );
    SLOG_typesz[ cpuID_t    ] = sizeof( SLOG_cpuID_t );
    SLOG_typesz[ thID_t     ] = sizeof( SLOG_threadID_t );
    SLOG_typesz[ taskID_t   ] = SLOG_typesz[ nodeID_t ]
#if ! defined( NOCPUID )
                              + SLOG_typesz[ cpuID_t ]
#endif
                              + SLOG_typesz[ thID_t ];
    SLOG_typesz[ where_t    ] = sizeof( SLOG_where_t );
    SLOG_typesz[ Nassocs_t  ] = sizeof( SLOG_N_assocs_t );
    SLOG_typesz[ assoc_t    ] = sizeof( SLOG_assoc_t );
    SLOG_typesz[ Nargs_t    ] = sizeof( SLOG_N_args_t );
    SLOG_typesz[ arg_t      ] = sizeof( SLOG_arg_t );

    /*  Define the byte size of the Often used datatype  */
    SLOG_typesz[ Header ]        = SLOG_typesz[ ui32 ] * 2
                                 + SLOG_typesz[ ui32 ] * 3
                                 + SLOG_typesz[ ui16 ] * 3
                                 + SLOG_typesz[ fptr ] * 6;
    SLOG_typesz[ FrameDirHdr ]   = SLOG_typesz[ fptr ] * 2
                                 + SLOG_typesz[ ui32 ];
    SLOG_typesz[ FrameDirEntry ] = SLOG_typesz[ fptr ]
                                 + SLOG_typesz[  ts  ] * 2;
    SLOG_typesz[ FrameHdr ]      = SLOG_typesz[ ui32 ] * 8;
    SLOG_typesz[ min_IntvlRec ]  = SLOG_typesz[ sz_t ]
                                 + SLOG_typesz[ rtype_t ]
                                 + SLOG_typesz[ itype_t ]
                                 + SLOG_typesz[ bebits_t ]
                                 + SLOG_typesz[ stime_t ]
                                 + SLOG_typesz[ dura_t ]
                                 + SLOG_typesz[ taskID_t ]
#if ! defined( NOWHERE )
                                 + SLOG_typesz[ where_t ]
#endif
                                 ;
    /*
       SLOG_typesz[ min_IntvlRec ] has to match actual bytesize written
       to the slogfile for a minimal interval record
    */

    SLOG_global.MinRectype4VarDiagRec   = 0;
    SLOG_global.MaxRectype4VarDiagRec   = 63;
    SLOG_global.MinRectype4FixDiagRec   = 64;
    SLOG_global.MaxRectype4FixDiagRec   = 127;
    SLOG_global.MinRectype4VarOffdRec   = 128;
    SLOG_global.MaxRectype4VarOffdRec   = 191;
    SLOG_global.MinRectype4FixOffdRec   = 192;
    SLOG_global.MaxRectype4FixOffdRec   = 255;
}



/*
   Create/allocate the data structure SLOG_STREAM
   and open the file descriptor
*/
SLOG_STREAM *SLOG_OpenStream( const char *path, const char *mode )
{
    SLOG_STREAM *slog;

    if ( strlen( mode ) < 2 ) {
        fprintf( errfile, __FILE__":SLOG_OpenStream( %s, %s ) - "
                          "The input file mode string %s is too short\n",
                          path, mode, mode );
        fflush( errfile );
        return NULL;
    }

    if ( strchr( mode, 'b' ) == NULL ) {
        fprintf( errfile, __FILE__":SLOG_OpenStream( %s, %s ) - "
                          "The input file mode string %s does NOT specify "
                          "binary file mode(b)\n",
                          path, mode, mode );
        fflush( errfile );
        return NULL;
    }

    slog = ( SLOG_STREAM * ) malloc ( sizeof ( SLOG_STREAM ) );
    if ( slog == NULL ) {
        fprintf( errfile, __FILE__":SLOG_OpenStream( %s, %s ) - "
                          "malloc() fails\n", path, mode );
        fflush( errfile );
        return NULL;
    }
    
    slog->fd = fopen( path, mode );
    if ( slog->fd == NULL ) {
        fprintf( errfile, __FILE__":SLOG_OpenStream( %s, %s ) - "
                          "fopen() fails\n", path, mode ); 
        fflush( errfile );
        SLOG_CloseStream( slog );
        return NULL;
    }
    
    if ( mode[0] != 'w' && mode[0] != 'r' ) {
        fprintf( errfile, __FILE__":SLOG_OpenStream( %s, %s ) - "
                          "The input file mode string %s is NEITHER for "
                          "writing(w) NOR for reading(r)\n",
                          path, mode, mode );
        fflush( errfile );
        SLOG_CloseStream( slog );
        return NULL;
    }

    /*  Initialize the various pointers in SLOG_STREAM  */

    slog->HasIrec2IOStreamBeenUsed  = SLOG_FALSE;
    slog->prev_starttime            = SLOG_time_NULL;
    slog->prev_endtime              = SLOG_time_NULL;
    slog->fbuf                      = NULL;
    slog->hdr                       = NULL;
    slog->pstat                     = NULL;
    slog->pview                     = NULL;
    slog->prof                      = NULL;
    slog->thread_tab                = NULL;
    slog->rec_defs                  = NULL;
    slog->cur_bbuf                  = NULL;
    slog->inc_bbuf                  = NULL;
    slog->pas_bbuf                  = NULL;
    slog->out_bbuf                  = NULL;
    slog->tmp_bbuf                  = NULL;    /*  For Write API  */
    slog->ptr2bbuf                  = NULL;    /*  For Read  API  */
    slog->reserve                   = NULL;    /*  For Write API  */

    return slog;
}



/*
   Close/free the components of data structre SLOG_STREAM
*/
int SLOG_CloseStream( SLOG_STREAM *slog )
{
    int ierr;                   /* error return code */

    if ( ( slog->frame_dir ).entries != NULL ) {
       free( ( slog->frame_dir ).entries );
       ( slog->frame_dir ).entries = NULL;
    }

    /*
        The order of the following subroutine calls here is in 
        reverse order of that in SLOG_OpenStream()
    */

    SLOG_STM_FreeBbufs( slog );

    SLOG_RDEF_Free( slog->rec_defs );
    SLOG_TTAB_Free( slog->thread_tab );
    SLOG_PROF_Free( slog->prof );
    SLOG_PVIEW_Free( slog->pview );
    SLOG_PSTAT_Free( slog->pstat );
    SLOG_HDR_Free( slog->hdr );

    ierr = fclose( slog->fd );
    if ( slog != NULL ) {
       free( slog );
       slog = NULL;
    }

#if defined( DEBUG )
    if ( ierr ) {
        fprintf( errfile, __FILE__":SLOG_CloseStream() - fclose() "
                          "fails with ierr = %d\n", ierr );
        fflush( errfile );
        return SLOG_FAIL;
    } 
#endif

    return SLOG_SUCCESS;
}



int SLOG_STM_CreateBbufs( SLOG_STREAM  *slog )
{
    /*  allocate the file buffer data structure for IO buffering  */
    slog->fbuf = fbuf_create( slog->hdr->frame_bytesize );
    if ( slog->fbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_STM_CreateBbufs() - "
                          "fbuf_create( %d bytes ) fails\n",
                          slog->hdr->frame_bytesize ); 
        fflush( errfile );
        SLOG_CloseStream( slog );
        return SLOG_FAIL;
    }

    /*  Set the file buffer's file descriptor equal to the SLOG_STREAM's  */
    fbuf_filedesc( slog->fbuf, slog->fd );

    /*  Create the various linked lists in the SLOG_STREAM  */

    slog->cur_bbuf = ( SLOG_intvlrec_blist_t * )
                     malloc( sizeof( SLOG_intvlrec_blist_t ) );
    if ( slog->cur_bbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_STM_CreateBbufs() - "
                          "malloc( CUR_BBUF ) fails\n" ); 
        fflush( errfile );
        SLOG_CloseStream( slog );
        return SLOG_FAIL;
    }
    slog->inc_bbuf = ( SLOG_intvlrec_blist_t * )
                     malloc( sizeof( SLOG_intvlrec_blist_t ) );
    if ( slog->inc_bbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_STM_CreateBbufs() - "
                          "malloc( INC_BBUF ) fails\n" ); 
        fflush( errfile );
        SLOG_CloseStream( slog );
        return SLOG_FAIL;
    }
    slog->pas_bbuf = ( SLOG_intvlrec_blist_t * )
                     malloc( sizeof( SLOG_intvlrec_blist_t ) );
    if ( slog->pas_bbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_STM_CreateBbufs() - "
                          "malloc( PAS_BBUF ) fails\n" ); 
        fflush( errfile );
        SLOG_CloseStream( slog );
        return SLOG_FAIL;
    }
    slog->out_bbuf = ( SLOG_intvlrec_blist_t * )
                     malloc( sizeof( SLOG_intvlrec_blist_t ) );
    if ( slog->out_bbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_STM_CreateBbufs() - "
                          "malloc( OUT_BBUF ) fails\n" ); 
        fflush( errfile );
        SLOG_CloseStream( slog );
        return SLOG_FAIL;
    }
    slog->tmp_bbuf = ( SLOG_intvlrec_blist_t * )
                     malloc( sizeof( SLOG_intvlrec_blist_t ) );
    if ( slog->tmp_bbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_STM_CreateBbufs() - "
                          "malloc( TMP_BBUF ) fails\n" ); 
        fflush( errfile );
        SLOG_CloseStream( slog );
        return SLOG_FAIL;
    }

    return SLOG_SUCCESS;
}



void SLOG_STM_FreeBbufs( SLOG_STREAM  *slog )
{
    SLOG_Bbuf_Free( slog->tmp_bbuf );
    SLOG_Bbuf_Free( slog->out_bbuf );
    SLOG_Bbuf_Free( slog->pas_bbuf );
    SLOG_Bbuf_Free( slog->inc_bbuf );
    SLOG_Bbuf_Free( slog->cur_bbuf );

    fbuf_free( slog->fbuf );
}



int SLOG_WriteInitFrameDir( SLOG_STREAM *slog )
{
    int ii;

    /*  Initialization of the frame directory   */
    ( slog->frame_dir ).prevdir      = SLOG_fptr_NULL;
    ( slog->frame_dir ).nextdir      = SLOG_fptr_NULL;
    ( slog->frame_dir ).Nframe       = 0;

    ( slog->frame_dir ).entries
    = ( SLOG_dir_entry_t * ) malloc( slog->hdr->max_Ndirframe
                                   * sizeof( SLOG_dir_entry_t ) );
    if (    slog->hdr->max_Ndirframe > 0
         && ( slog->frame_dir ).entries == NULL ) {
        fprintf( errfile, __FILE__":SLOG_WriteInitFrameDir() - malloc() calls "
                          "for ( slog->frame_dir ).entries[] fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    for ( ii = 0; ii < slog->hdr->max_Ndirframe; ii ++ ){
        ( ( slog->frame_dir ).entries[ ii ] ).fptr2framehdr
        = SLOG_fptr_NULL;
        ( ( slog->frame_dir ).entries[ ii ] ).starttime
        = SLOG_time_NULL;
        ( ( slog->frame_dir ).entries[ ii ] ).endtime
        = SLOG_time_NULL;
    }

  /*
      Set the SLOG buffer's Frame Directory pointer to the
      beginning of Frame Directory
  */
    fflush( slog->fd );
    slog->file_dir_hdr = slog_ftell( slog->fd );

  /*
      Write the initialization section of the frame directory 
  */
    bswp_fwrite( &( ( slog->frame_dir ).prevdir ),
                 SLOG_typesz[ fptr ], 1, slog->fd );
    bswp_fwrite( &( ( slog->frame_dir ).nextdir ),
                 SLOG_typesz[ fptr ], 1, slog->fd );
    bswp_fwrite( &( ( slog->frame_dir ).Nframe ),
                 SLOG_typesz[ ui32 ], 1, slog->fd );

  /*
      Set the SLOG buffer's Frame Directory CURRENT Entry pointer to the
      1st entry of Frame Directory 
  */
    fflush( slog->fd );
    slog->file_dir_cur = slog_ftell( slog->fd );

#if defined( DEBUG )
    if (    slog->file_dir_cur - slog->file_dir_hdr
         != SLOG_typesz[ FrameDirHdr ] ) {
        fprintf( errfile, __FILE__":SLOG_WriteInitFrameDir() -  Inconsistency "
                          " between file pointers\n" );
        fprintf( errfile, "      ""slog->file_dir_cur("fmt_fptr") - "
                          "slog->file_dir_hdr("fmt_fptr") "
                          "!= SLOG_typesz[ FrameDirHdr ]("fmt_ui32")\n",
                          slog->file_dir_cur, slog->file_dir_hdr,
                          SLOG_typesz[ FrameDirHdr ] );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    for ( ii = 0; ii < slog->hdr->max_Ndirframe; ii ++ ) {
        SLOG_WriteFrameDirEntryGiven( slog->fd, 
                                      &( ( slog->frame_dir ).entries[ ii ] )
                                    );
    }

  /*
      Save the location of the end of the Frame Directory
      For writing of the 1st Frame Header and Buffer.
  */
    fflush( slog->fd );
    slog->file_loc = slog_ftell( slog->fd );

#if defined( DEBUG )
    if (    slog->file_loc - slog->file_dir_cur 
         != slog->hdr->max_Ndirframe * SLOG_typesz[ FrameDirEntry ] ) {
        fprintf( errfile, __FILE__":SLOG_WriteInitFrameDir() -  Inconsistency "
                          " between file pointers\n" );
        fprintf( errfile, "      ""slog->file_loc("fmt_fptr") - "
                          "slog->file_dir_cur("fmt_fptr") "
                          "!= SLOG_typesz[ FrameDirEntries ]("fmt_ui32")\n",
                          slog->file_loc, slog->file_dir_cur,
                          slog->hdr->max_Ndirframe
                          * SLOG_typesz[ FrameDirEntry ] );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    return SLOG_SUCCESS;
}



int SLOG_ReadFrameDir( SLOG_STREAM *slog )
{
    int ii;

  /*
      Set the SLOG buffer's Frame Directory pointer to the
      beginning of Frame Directory
  */
    slog->file_dir_hdr = slog_ftell( slog->fd );

  /*
      Read in the Frame Directory header
  */
    bswp_fread( &( ( slog->frame_dir ).prevdir ),
                SLOG_typesz[ fptr ], 1, slog->fd );
    bswp_fread( &( ( slog->frame_dir ).nextdir ),
                SLOG_typesz[ fptr ], 1, slog->fd );
    bswp_fread( &( ( slog->frame_dir ).Nframe ),
                SLOG_typesz[ ui32 ], 1, slog->fd );

  /*
      Allocate the memory for the Frame Directory
  */
    ( slog->frame_dir ).entries
    = ( SLOG_dir_entry_t * ) malloc( ( slog->frame_dir ).Nframe
                                   * sizeof( SLOG_dir_entry_t ) );
    if (    ( slog->frame_dir ).Nframe > 0
         && ( slog->frame_dir ).entries == NULL ) {
        fprintf( errfile, __FILE__":SLOG_ReadFrameDir() - malloc() calls "
                          "for ( slog->frame_dir ).entries[] fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

  /*
      Set the SLOG buffer's Frame Directory CURRENT Entry pointer to the
      1st entry of Frame Directory 
  */
    slog->file_dir_cur = slog_ftell( slog->fd );

#if defined( DEBUG )
    if (    slog->file_dir_cur - slog->file_dir_hdr
         != SLOG_typesz[ FrameDirHdr ] ) {
        fprintf( errfile, __FILE__":SLOG_ReadFrameDir() -  Inconsistency "
                          " between file pointers\n" );
        fprintf( errfile, "      ""slog->file_dir_cur("fmt_fptr") - "
                          "slog->file_dir_hdr("fmt_fptr") "
                          "!= SLOG_typesz[ FrameDirHdr ]("fmt_ui32")\n", 
                          slog->file_dir_cur, slog->file_dir_hdr,
                          SLOG_typesz[ FrameDirHdr ] );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

  /*
      Read in the Frame Directory records entries
  */
    for ( ii = 0; ii < ( slog->frame_dir ).Nframe; ii ++ )
        SLOG_ReadFrameDirEntry( slog->fd,
                                &( (slog->frame_dir).entries[ ii ] )
                              );
  
  /*
      Save the location of the end of the Frame Directory
      For writing of the 1st Frame Header and Buffer.
  */
    slog->file_loc = slog_ftell( slog->fd );

#if defined( DEBUG )
    if (    slog->file_loc - slog->file_dir_cur
         != ( slog->frame_dir ).Nframe * SLOG_typesz[ FrameDirEntry ] ) {
        fprintf( errfile, __FILE__":SLOG_ReadFrameDir() -  Inconsistency "
                          " between file pointers\n" );
        fprintf( errfile, "      ""slog->file_loc("fmt_fptr") - "
                          "slog->file_dir_cur("fmt_fptr") "
                          "!= SLOG_typesz[ FrameDirEntries ]("fmt_ui32")\n",
                          slog->file_loc, slog->file_dir_cur,
                          ( slog->frame_dir ).Nframe
                          * SLOG_typesz[ FrameDirEntry ] );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    return SLOG_SUCCESS;
}



/*  Simple update the Frame Directory Entry  */
int SLOG_WriteFrameDirEntryGiven(       FILE             *slog_fd,
                                  const SLOG_dir_entry_t *framedir_entry )
{
    bswp_fwrite( &( framedir_entry->fptr2framehdr ),
                 SLOG_typesz[ fptr ], 1, slog_fd );
    bswp_fwrite( &( framedir_entry->starttime )    ,
                 SLOG_typesz[  ts  ], 1, slog_fd );
    bswp_fwrite( &( framedir_entry->endtime )      ,
                 SLOG_typesz[  ts  ], 1, slog_fd );

    return SLOG_SUCCESS;
}



/*  Simple update the Frame Directory Entry  */
int SLOG_ReadFrameDirEntry( FILE             *slog_fd,
                            SLOG_dir_entry_t *framedir_entry )
{
    bswp_fread( &( framedir_entry->fptr2framehdr ),
                SLOG_typesz[ fptr ], 1, slog_fd );
    bswp_fread( &( framedir_entry->starttime )    ,
                SLOG_typesz[  ts  ], 1, slog_fd );
    bswp_fread( &( framedir_entry->endtime )      ,
                SLOG_typesz[  ts  ], 1, slog_fd );

    return SLOG_SUCCESS;
}



/*  Update the Frame Directory Entry by looking the SLOG_STREAM.file_dir_rec  */
int SLOG_WriteUpdatedFrameDirEntry( SLOG_STREAM  *slog )
{
    SLOG_fptr file_loc_saved;
    SLOG_fptr offset;
    SLOG_time starttime;
    SLOG_time endtime;
    int       ierr;


    /*  Save the current file pointer position  */
    file_loc_saved = slog_ftell( slog->fd );

    offset    = ( slog->frame_dir_entry ).fptr2framehdr;
    starttime = ( slog->frame_dir_entry ).starttime;
    endtime   = ( slog->frame_dir_entry ).endtime;

    /*  Point to the current frame directory entry for writing  */
    ierr = slog_fseek( slog->fd, slog->file_dir_cur, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_WriteUpdatedFrameDirEntry() - "
                          "slog_fseek("fmt_fptr") fails\n",
                          slog->file_dir_cur );
        fprintf( errfile, "\tCannot load the current frame directory entry\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Writing the current Directory Entry  */
    ierr = bswp_fwrite( &offset   , SLOG_typesz[ fptr ], 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_WriteUpdatedFrameDirEntry() - "
                          "Cannot write the OFFSET to the SLOG file\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    ierr = bswp_fwrite( &starttime, SLOG_typesz[  ts  ], 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_WriteUpdatedFrameDirEntry() - "
                          "Cannot write the STARTtime to the SLOG file\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    ierr = bswp_fwrite( &endtime  , SLOG_typesz[  ts  ], 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_WriteUpdatedFrameDirEntry() - "
                          "Cannot write the ENDtime to the SLOG file\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

  /*
      Update the SLOG buffer's Frame Directory CURRENT Entry pointer to the
      Next entry of Frame Directory 
  */
    fflush( slog->fd );
    slog->file_dir_cur = slog_ftell( slog->fd );

    /*  Restore the saved file pointer position  */
    ierr = slog_fseek( slog->fd, file_loc_saved, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_WriteUpdatedFrameDirEntry() - "
                          "\t""slog_fseek() fails, cannot restore the "
                          "original file position before the subroutine "
                          "call\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    return SLOG_SUCCESS;
}



/*  **************  LOWER Level API  **************  */


/*
    The index returned by SLOG_STM_IdxOfCurFrameDirEntry() starts from 0.
*/
int SLOG_STM_IdxOfCurFrameDirEntry( const SLOG_STREAM *slog )
{
    SLOG_fptr entry0;
    SLOG_fptr entry0_entryCUR;
    SLOG_fptr ifptr;

    entry0          = slog->file_dir_hdr + SLOG_typesz[ FrameDirHdr ];
    entry0_entryCUR = slog->file_dir_cur - entry0;

    if ( entry0_entryCUR % SLOG_typesz[ FrameDirEntry ] != 0 ) {
        fprintf( errfile, __FILE__":SLOG_STM_IdxOfCurFrameDirEntry() - "
                          "entry0_entryCUR("fmt_fptr") is indivisable by "
                          "SLOG_typesz[ FrameDirEntry ]("fmt_ui32")\n",
                          entry0_entryCUR, SLOG_typesz[ FrameDirEntry ] );
        fflush( errfile );
        return SLOG_FAIL;
    }

    ifptr = entry0_entryCUR / SLOG_typesz[ FrameDirEntry ];

    if ( ifptr >= slog->hdr->max_Ndirframe ) { 
        fprintf( errfile, __FILE__":SLOG_STM_IdxOfCurFrameDirEntry() - "
                          "ifptr("fmt_fptr") is bigger than the largest "
                          "value(%d)\n", ifptr, slog->hdr->max_Ndirframe );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( ifptr >= (slog->frame_dir).Nframe ) {
        /*
        fprintf( errfile, __FILE__":SLOG_STM_IdxOfCurFrameDirEntry() - "
                          "ifptr("fmt_fptr") is bigger Number of Frame("
                          fmt_ui32") in File Frame Directory\n",
                          ifptr, (slog->frame_dir).Nframe );
        fflush( errfile );
        */
        return SLOG_EOF;
    }

    return (int) ifptr;
}



void SLOG_STM_Init( SLOG_STREAM *slog )
{
    const SLOG_uint32 zero = (SLOG_uint32) 0;

    /*  Initialize the Frame Direcrtory Header in the buffer  */
    /*  This is only done ONCE for SINGLE directory SLOG file */

    SLOG_STM_UpdateFrameDirHdrGiven( slog, SLOG_fptr_NULL,
                                     SLOG_fptr_NULL, zero );
    
    /*  Initialize the frame header in the buffer  */
    SLOG_STM_UpdateFrameHdrGiven( slog, zero, zero, zero, zero,
                                        zero, zero, zero, zero );

    /*  Initialize the bi-directional list buffers & related variables  */
    SLOG_Bbuf_Init( slog->cur_bbuf );
    SLOG_Bbuf_Init( slog->inc_bbuf );
    SLOG_Bbuf_Init( slog->pas_bbuf );
    SLOG_Bbuf_Init( slog->out_bbuf );
    SLOG_Bbuf_Init( slog->tmp_bbuf );
}



void SLOG_STM_UpdateFrameDirHdrGiven(       SLOG_STREAM       *slog,
                                      const SLOG_fptr          prevdir_i,
                                      const SLOG_fptr          nextdir_i,
                                      const SLOG_uint32        Nframe_i )
{
    ( slog->frame_dir ).prevdir     = prevdir_i;
    ( slog->frame_dir ).nextdir     = nextdir_i;
    ( slog->frame_dir ).Nframe      = Nframe_i;
    ( slog->frame_dir ).entries     = NULL;
}



int SLOG_STM_IncrementFrameDirHdr( SLOG_STREAM       *slog )
{
    /*  Increment the Number of Frame in the Frame Buffer  */
    if ( ( slog->frame_dir ).Nframe < slog->hdr->max_Ndirframe )
       ( slog->frame_dir ).Nframe ++;
    else {
        fprintf( errfile, __FILE__":SLOG_STM_IncrementFrameDirHdr() - "
                          "( slog->frame_dir ).Nframe = "fmt_ui32" >= Maximum "
                          "Allowed Record Numbers per Frame Directory\n",
                          ( slog->frame_dir ).Nframe );
        fflush( errfile );
        exit( 1 );
        return SLOG_FAIL;
    }

    return SLOG_SUCCESS;
}



int SLOG_STM_WriteFrameDirHdr( SLOG_STREAM       *slog )
{
    SLOG_fptr file_loc_saved;
    int       ierr;

    /*  Save the current file pointer position  */
    file_loc_saved = slog_ftell( slog->fd );

    ierr = slog_fseek( slog->fd, slog->file_dir_hdr, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_STM_WriteFrameDirHdr() - slog_fseek() "
                          "cannot load the current frame directory entry\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

  /*
      Write the initialization section of the frame directory
      Could be done in bswp_fwrite statement to improve speed
  */
    bswp_fwrite( &( ( slog->frame_dir ).prevdir ),
                 SLOG_typesz[ fptr ], 1, slog->fd );
    bswp_fwrite( &( ( slog->frame_dir ).nextdir ),
                 SLOG_typesz[ fptr ], 1, slog->fd );
    bswp_fwrite( &( ( slog->frame_dir ).Nframe ),
                 SLOG_typesz[ ui32 ], 1, slog->fd );
    
    /*  Restore the saved file pointer position  */
    ierr = slog_fseek( slog->fd, file_loc_saved, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_STM_WriteFrameDirHdr() - slog_fseek() "
                          "fails, cannot restore the orginal file position "
                          "before the subroutine call\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    return SLOG_SUCCESS;
}



/*
    This routine assume that 
    NEITHER ( slog->cur_bbuf )->lhead == NULL
    NOR     ( slog->cur_bbuf )->ltail == NULL
    i.e.  This routine is meant to be used WHEN the cur_bbuf is FILLED
*/
int SLOG_STM_UpdateFrameDirEntry_Forward(       SLOG_STREAM  *slog,
                                          const int           IsLastFrame )
{
    SLOG_time   endtime_tmp;
    int         idx_of_cur_frame;

    /*  Update the Buffer's Frame Directory Entry  */

    ( slog->frame_dir_entry ).fptr2framehdr = slog->file_loc;
    if ( ( slog->frame_dir_entry ).fptr2framehdr == SLOG_fptr_NULL ) {
        fprintf( errfile, __FILE__":SLOG_STM_UpdateFrameDirEntry_Forward() - "
                          "the current frame buffer's Fptr2FrameHdr "
                          "is assigned NULL!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( SLOG_Bbuf_IsEmpty( slog->cur_bbuf ) ) {
        fprintf( errfile, __FILE__":SLOG_STM_UpdateFrameDirEntry_Forward() - "
                          "slog->cur_bbuf contains NO elements!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    idx_of_cur_frame = ( slog->frame_dir ).Nframe;

    /*  Set the frame's starttime  */
    if ( idx_of_cur_frame == 0 ) 
        ( slog->frame_dir_entry ).starttime
        = SLOG_Bbuf_EarliestStarttime( slog->cur_bbuf ); 
    else
        ( slog->frame_dir_entry ).starttime
        = ( (slog->frame_dir).entries[idx_of_cur_frame-1] ).endtime;

    /*  Set the frame's endtime  */
    if ( ( slog->cur_bbuf )->ltail != NULL ) {
        if ( slog->hdr->IsIncreasingEndtime == SLOG_TRUE )
            ( slog->frame_dir_entry ).endtime
            = ( ( ( slog->cur_bbuf )->ltail )->irec )->starttime
            + ( ( ( slog->cur_bbuf )->ltail )->irec )->duration;
        else if ( slog->hdr->IsIncreasingStarttime == SLOG_TRUE )
            ( slog->frame_dir_entry ).endtime
            = ( ( ( slog->cur_bbuf )->ltail )->irec )->starttime;
        else
            ( slog->frame_dir_entry ).endtime = SLOG_time_NULL;
    }
    else
        ( slog->frame_dir_entry ).endtime = SLOG_time_NULL;

    if ( IsLastFrame == SLOG_TRUE ) {
        ( slog->frame_dir_entry ).endtime
        = SLOG_Bbuf_LatestEndtime( slog->cur_bbuf );

        if ( ! SLOG_Bbuf_IsEmpty( slog->tmp_bbuf ) ) {
            endtime_tmp = SLOG_Bbuf_LatestEndtime( slog->tmp_bbuf );
            if ( endtime_tmp > ( slog->frame_dir_entry ).endtime ) 
                ( slog->frame_dir_entry ).endtime = endtime_tmp;
        }
    }

    /*  Update the (slog->frame_dir).entries[ idx_of_current_frame ]  */
    ( slog->frame_dir ).entries[ idx_of_cur_frame ].fptr2framehdr
    = ( slog->frame_dir_entry ).fptr2framehdr;
    ( slog->frame_dir ).entries[ idx_of_cur_frame ].starttime
    = ( slog->frame_dir_entry ).starttime;
    ( slog->frame_dir ).entries[ idx_of_cur_frame ].endtime
    = ( slog->frame_dir_entry ).endtime;


    /*  Inconsistency Check  */
    if (    ( slog->frame_dir_entry ).starttime
         == ( slog->frame_dir_entry ).endtime   ) {
        if ( ( slog->cur_bbuf )->lhead != NULL ) {
            if ( ( ( ( slog->cur_bbuf )->lhead )->irec )->duration > 0.0 ) {
                fprintf( errfile, __FILE__
                                  ":SLOG_STM_UpdateFrameDirEntry_Forward() - "
                                  "the current frame buffer's STARTtime ("
                                  fmt_time") == ENDtime ("fmt_time") !!!\n",
                                  ( slog->frame_dir_entry ).starttime,
                                  ( slog->frame_dir_entry ).endtime );
                fflush( errfile );
                return SLOG_FAIL;
            }
        }
    }

    /*  Inconsistency Check  */
    if (    ( slog->frame_dir_entry ).starttime
         >  ( slog->frame_dir_entry ).endtime   ) {
        if ( ( slog->cur_bbuf )->count_irec > 0 ) { 
            fprintf( errfile, __FILE__
                              ":SLOG_STM_UpdateFrameDirEntry_Forward() - "
                              "the current frame buffer's STARTtime ("
                              fmt_time") >  ENDtime ("fmt_time") !!!\n",
                              ( slog->frame_dir_entry ).starttime,
                              ( slog->frame_dir_entry ).endtime );
            fflush( errfile );
            return SLOG_FAIL;
        }
    }

    return SLOG_SUCCESS;
}



int SLOG_STM_UpdateFrameDirEntry_Backward(       SLOG_STREAM *slog,
                                           const SLOG_uint32  idx_of_cur_frame )
{
    SLOG_time   starttime_tmp;

    
    if ( idx_of_cur_frame >= (slog->frame_dir).Nframe ) {
        fprintf( errfile, __FILE__
                          ":SLOG_STM_UpdateFrameDirEntry_Backward() - \n" );
        fprintf( errfile, "\t""the input index("fmt_ui32") is bigger "
                          "the Number of Frames("fmt_ui32") in "
                          "File Frame Directory\n",
                          idx_of_cur_frame, (slog->frame_dir).Nframe );
        fflush( errfile );
        return SLOG_EOF;
    }

    /* 
        Update the SLOG_STREAM's internal data structure, frame_dir_entry,
        which is used in SLOG_STM_UpdateFRAME_Backward(),
    */
    ( slog->frame_dir_entry ).fptr2framehdr
    = ( slog->frame_dir ).entries[ idx_of_cur_frame ].fptr2framehdr;
    ( slog->frame_dir_entry ).starttime
    = ( slog->frame_dir ).entries[ idx_of_cur_frame ].starttime;
    ( slog->frame_dir_entry ).endtime
    = ( slog->frame_dir ).entries[ idx_of_cur_frame ].endtime;

    slog->file_dir_cur = slog->file_dir_hdr + SLOG_typesz[ FrameDirHdr ]
                       + idx_of_cur_frame * SLOG_typesz[ FrameDirEntry ];
    
    if ( idx_of_cur_frame > 0 ) return SLOG_SUCCESS;

    /* 
        The following statements are for the very 1st frame,
        with frame_idx = 0
    */

    /*  Update the Buffer's Frame Directory Entry  */

    if ( ( slog->frame_dir_entry ).fptr2framehdr == SLOG_fptr_NULL ) {
        fprintf( errfile, __FILE__":SLOG_STM_UpdateFrameDirEntry_Backward() - "
                          "the current frame buffer's Fptr2FrameHdr "
                          "is assigned NULL!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( SLOG_Bbuf_IsEmpty( slog->cur_bbuf ) ) {
        fprintf( errfile, __FILE__":SLOG_STM_UpdateFrameDirEntry_Backward() - "
                          "slog->cur_bbuf contains NO elements!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( ! SLOG_Bbuf_IsEmpty( slog->tmp_bbuf ) ) {
        starttime_tmp = SLOG_Bbuf_EarliestStarttime( slog->tmp_bbuf );
        if ( starttime_tmp < ( slog->frame_dir_entry ).starttime )
            ( slog->frame_dir_entry ).starttime = starttime_tmp;
    }

    /*  Update the (slog->frame_dir).entries[ idx_of_current_frame ]  */
    ( slog->frame_dir ).entries[ idx_of_cur_frame ].fptr2framehdr
    = ( slog->frame_dir_entry ).fptr2framehdr;
    ( slog->frame_dir ).entries[ idx_of_cur_frame ].starttime
    = ( slog->frame_dir_entry ).starttime;
    ( slog->frame_dir ).entries[ idx_of_cur_frame ].endtime
    = ( slog->frame_dir_entry ).endtime;

    return SLOG_SUCCESS;
}



void SLOG_STM_UpdateFrameHdrGiven(       SLOG_STREAM  *slog,
                                   const SLOG_uint32   Nbytes_cur_i,
                                   const SLOG_uint32   Nrec_cur_i,
                                   const SLOG_uint32   Nbytes_inc_i,
                                   const SLOG_uint32   Nrec_inc_i,
                                   const SLOG_uint32   Nbytes_pas_i,
                                   const SLOG_uint32   Nrec_pas_i,
                                   const SLOG_uint32   Nbytes_out_i, 
                                   const SLOG_uint32   Nrec_out_i )
{
    ( slog->frame_hdr ).Nbytes_cur = Nbytes_cur_i;
    ( slog->frame_hdr ).Nrec_cur   = Nrec_cur_i;
    ( slog->frame_hdr ).Nbytes_inc = Nbytes_inc_i;
    ( slog->frame_hdr ).Nrec_inc   = Nrec_inc_i;
    ( slog->frame_hdr ).Nbytes_pas = Nbytes_pas_i;
    ( slog->frame_hdr ).Nrec_pas   = Nrec_pas_i;
    ( slog->frame_hdr ).Nbytes_out = Nbytes_out_i;
    ( slog->frame_hdr ).Nrec_out   = Nrec_out_i;
}



/*
    This routine automatically updates the Frame Header,
    slog->frame_hdr, everytime when the frame is filled.  It is
    meant to be used right BEFORE Writing of the Frame Header
    to the disk or disk buffer.

    Currently, it is used in SLOG_STM_UpdateFRAME_Forward().
*/
void SLOG_STM_UpdateFrameHdr( SLOG_STREAM  *slog )
{
    ( slog->frame_hdr ).Nbytes_cur = ( slog->cur_bbuf )->Nbytes_in_file;
    ( slog->frame_hdr ).Nrec_cur   = ( slog->cur_bbuf )->count_irec;
    ( slog->frame_hdr ).Nbytes_inc = ( slog->inc_bbuf )->Nbytes_in_file;
    ( slog->frame_hdr ).Nrec_inc   = ( slog->inc_bbuf )->count_irec;
    ( slog->frame_hdr ).Nbytes_pas = ( slog->pas_bbuf )->Nbytes_in_file;
    ( slog->frame_hdr ).Nrec_pas   = ( slog->pas_bbuf )->count_irec;
    ( slog->frame_hdr ).Nbytes_out = ( slog->out_bbuf )->Nbytes_in_file;
    ( slog->frame_hdr ).Nrec_out   = ( slog->out_bbuf )->count_irec;
}



int SLOG_STM_DepositFrameHdr( const SLOG_framehdr_t *frame_hdr,
                                    filebuf_t       *fbuf )
{
    SLOG_uint32 Nbytes_written;
    int         ierr;

    Nbytes_written = 0;

    ierr = fbuf_deposit( &( frame_hdr->Nbytes_cur ),
                         SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_DepositFrameHdr() - Cannot write "
                          "the Nbytes_cur to the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_deposit( &( frame_hdr->Nrec_cur ),
                         SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_DepositFrameHdr() - Cannot write "
                          "the Nrec_cur to the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_deposit( &( frame_hdr->Nbytes_inc ),
                         SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_DepositFrameHdr() - Cannot write "
                          "the Nbytes_inc to the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_deposit( &( frame_hdr->Nrec_inc ),
                         SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_DepositFrameHdr() - Cannot write "
                          "the Nrec_inc to the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_deposit( &( frame_hdr->Nbytes_pas ),
                         SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_DepositFrameHdr() - Cannot write "
                          "the Nbytes_pas to the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_deposit( &( frame_hdr->Nrec_pas ),
                         SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_DepositFrameHdr() - Cannot write "
                          "the Nrec_pas to the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_deposit( &( frame_hdr->Nbytes_out ),
                         SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_DepositFrameHdr() - Cannot write "
                          "the Nbytes_out to the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_deposit( &( frame_hdr->Nrec_out ),
                         SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_DepositFrameHdr() - Cannot write "
                          "the Nrec_out to the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ ui32 ];

    return Nbytes_written;
}



int SLOG_STM_WithdrawFrameHdr( SLOG_framehdr_t *frame_hdr,
                               filebuf_t       *fbuf )
{
    SLOG_uint32 Nbytes_read;
    int         ierr;

    Nbytes_read = 0;

    ierr = fbuf_withdraw( &( frame_hdr->Nbytes_cur ),
                          SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_WithdrawFrameHdr() - Cannot read "
                          "the Nbytes_cur from the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_withdraw( &( frame_hdr->Nrec_cur ),
                          SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_WithdrawFrameHdr() - Cannot read "
                          "the Nrec_cur from the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_withdraw( &( frame_hdr->Nbytes_inc ),
                          SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_WithdrawFrameHdr() - Cannot read "
                          "the Nbytes_inc from the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_withdraw( &( frame_hdr->Nrec_inc ),
                          SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_WithdrawFrameHdr() - Cannot read "
                          "the Nrec_inc from the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_withdraw( &( frame_hdr->Nbytes_pas ),
                          SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_WithdrawFrameHdr() - Cannot read "
                          "the Nbytes_pas from the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_withdraw( &( frame_hdr->Nrec_pas ),
                          SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_WithdrawFrameHdr() - Cannot read "
                          "the Nrec_pas from the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_withdraw( &( frame_hdr->Nbytes_out ),
                          SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_WithdrawFrameHdr() - Cannot read "
                          "the Nbytes_out from the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[ ui32 ];

    ierr = fbuf_withdraw( &( frame_hdr->Nrec_out ),
                          SLOG_typesz[ ui32 ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_STM_WithdrawFrameHdr() - Cannot read "
                          "the Nrec_out from the SLOG filebuffer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr * SLOG_typesz[ ui32 ];

    return Nbytes_read;
}



/*   obsoleted   */
int SLOG_STM_UpdateBBuf_endtime(       SLOG_intvlrec_blist_t *out_bbuf,
                                 const SLOG_intvlrec_blist_t *in_bbuf,
                                 const SLOG_dir_entry_t       frame_dir_entry )
{
    const SLOG_intvlrec_lptr_t  *lptr;
          SLOG_time              frame_endtime;
          SLOG_time              irec_endtime;
          int                    ierr;


    frame_endtime = frame_dir_entry.endtime;

    /*
        Check the content of the current Frame Buffer to see if there
        is any record that needs to be copied to the outgoing Frame Buffer
    */
    for ( lptr = in_bbuf->lhead; lptr != NULL; lptr = lptr->next ) {

        irec_endtime = ( lptr->irec )->starttime + ( lptr->irec )->duration;

        if ( irec_endtime > frame_endtime ) {
            ierr = SLOG_Bbuf_AddCopiedIntvlRec( out_bbuf, lptr->irec );
            if ( ierr != SLOG_SUCCESS ) {
                fprintf( errfile, __FILE__":SLOG_STM_UpdateBBuf_endtime() - "
                                  "SLOG_Bbuf_AddCopiedIntvlRec() fails at "
                                  "adding Interval Record to the "
                                  "_Out_ Buffer!!\n" );
                fflush( errfile );
                return SLOG_FAIL;
            }
            /*
                (lptr->irec)->vhead..vtail does NOT needed to be allocated as
                SLOG_Bbuf_AddCopiedIntvlRec() comment said.  Since lptr a 
                temporary variable for going through in_bbuf which is 
                declared CONSTANT
            */
        }
    }

    return SLOG_SUCCESS;
}



int SLOG_STM_DepositBBuf( const SLOG_intvlrec_blist_t *Bbuf,
                                filebuf_t             *fbuf )
{
    const SLOG_intvlrec_lptr_t *lptr;
          SLOG_uint32           Nbytes_written;
          SLOG_uint32           N_irec_written;
          int                   ierr;


    Nbytes_written = 0;
    N_irec_written = 0;

    for ( lptr = Bbuf->lhead; lptr != NULL; lptr = lptr->next ) {
        ierr = SLOG_Irec_DepositToFbuf( lptr->irec, fbuf );
        if ( ierr == SLOG_FAIL ) {
            fprintf( errfile, __FILE__":SLOG_STM_DepositBBuf() - "
                              "SLOG_Irec_DepositToFbuf() fails at writing "
                              fmt_ui32"-th Interval Record to the SLOG "
                              "filebuffer\n ", N_irec_written  );
            fflush( errfile );
            return SLOG_FAIL;
        }
        Nbytes_written += ierr;
        N_irec_written ++;
    }

#if defined( DEBUG )
    /*   Redundancy checks of the Byte Sizes and Number of records written  */
    if ( Nbytes_written != Bbuf->Nbytes_in_file ) {
        fprintf( errfile, __FILE__":SLOG_STM_DepositBBuf() - Nbytes_written = "
                          fmt_ui32", but expected Nbytes_in_file = "fmt_ui32
                          "\n", Nbytes_written, Bbuf->Nbytes_in_file );
        fflush( errfile );
        return SLOG_FAIL;
    }
    if ( N_irec_written != Bbuf->count_irec ) {
        fprintf( errfile, __FILE__":SLOG_STM_DepositBBuf() - N_irec_written = "
                          fmt_ui32", but expected writing count_irec = "
                          fmt_ui32"\n", Nbytes_written, Bbuf->count_irec );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    return Nbytes_written;
}



/*
    Assume Bbuf->Nbytes_in_file and Bbuf->count_irec have been updated
    correctly _before_ this routine is called.  So this routine will use
    the values of these 2 variables to update the whole Bbuf.
*/
int SLOG_STM_WithdrawBBuf(       SLOG_intvlrec_blist_t *Bbuf,
                           const SLOG_recdefs_table_t  *slog_recdefs,
                                 filebuf_t             *fbuf )
{
    SLOG_intvlrec_t  *irec;
    SLOG_uint32       Nrec_expected;
    SLOG_uint32       Nbytes_expected;
    int               ierr;
    int               ii;

    Nbytes_expected = Bbuf->Nbytes_in_file;
    Nrec_expected   = Bbuf->count_irec;
    Bbuf->Nbytes_in_file = 0;
    Bbuf->count_irec     = 0;

    for ( ii = 0; ii < Nrec_expected; ii++ ) {

        irec = SLOG_Irec_WithdrawFromFbuf( slog_recdefs, fbuf );
        if ( irec == NULL ) {
            fprintf( errfile, __FILE__":SLOG_STM_WithdrawBBuf - "
                              "SLOG_Irec_WithdrawFromFbuf() fails\n" );
            fprintf( errfile, "       ""SLOG_Irec_WithdrawFromFbuf() returns "
                              "a NULL pointer at %d-th expected interval "
                              "record.\n", ii );
            fflush( errfile );
            return SLOG_FAIL;
        }

        /*
        ierr = SLOG_Bbuf_AddCopiedIntvlRec( Bbuf, irec );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_STM_WithdrawBBuf - "
                              "SLOG_Bbuf_AddCopiedIntvlRec() fails at %d-th "
                              "expected interval record.\n", ii );
            fflush( errfile );
            return SLOG_FAIL;
        }
        SLOG_Irec_Free( irec );
        */

        ierr = SLOG_Bbuf_AddMovedIntvlRec( Bbuf, irec );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_STM_WithdrawBBuf - "
                              "SLOG_Bbuf_AddMovedIntvlRec() fails at %d-th "
                              "expected interval record.\n", ii );
            fflush( errfile );
            return SLOG_FAIL;
        }

    }   /*  for ( ii = 0; ii < Nrec_expected; ii++ )  */

#if defined( DEBUG )
    /*  Redundancy checks of the Byte Sizes and Number of records written  */
    if ( Bbuf->Nbytes_in_file != Nbytes_expected ) {
        fprintf( errfile, __FILE__":SLOG_STM_WithdrawBBuf() - "
                          "Nbytes_expected = "fmt_ui32
                          ", Nbytes_in_file read = "fmt_ui32"\n",
                          Nbytes_expected, Bbuf->Nbytes_in_file );
        fflush( errfile );
        return SLOG_FAIL;
    }
    if ( Bbuf->count_irec != Nrec_expected ) {
        fprintf( errfile, __FILE__":SLOG_STM_WithdrawBBuf() - "
                          "Nrec_expected = "fmt_ui32
                          ", count_irec read = "fmt_ui32"\n",
                          Nrec_expected, Bbuf->count_irec );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    return SLOG_SUCCESS;
}



/*
    SLOG_STM_UpdateFrameDirEntry_Forward() = Update the buffer's Frame 
    Directory Record's Fptr2FrameHdr, StartTime & EndTime 

    So, SLOG_STM_UpdateFrameDirEntry_Forward( slog ) HAS To Be Called BEFORE
    any operations on the various sectors of the frame, i.e. xxx_bbuf
    because the starttime and endtime of the frame determines where
    where each interval record should go.
*/
int SLOG_STM_UpdateFRAME_Forward( SLOG_STREAM *slog )
{
    /* const SLOG_intvlrec_lptr_t  *lptr;  */
          SLOG_intvlrec_lptr_t  *lptr;
          SLOG_time              frame_starttime;
          SLOG_time              frame_endtime;
          SLOG_time              irec_starttime;
          SLOG_time              irec_endtime;
          int                    ierr;


    frame_starttime = ( slog->frame_dir_entry ).starttime;
    frame_endtime   = ( slog->frame_dir_entry ).endtime;

    /*
        Split interval records in TEMPORARY Bbuf into INCOMING and PASSING
        THROUGH Bbufs according to the criterion( exceeds the Frame's endtime )
    */
    lptr = SLOG_Bbuf_GetPointerAtHead( slog->tmp_bbuf );
    while ( lptr != NULL ) {

        irec_endtime = ( lptr->irec )->starttime + ( lptr->irec )->duration;

        /*  Set slog->tmp_bbuf->lptr = lptr  */
        ierr = SLOG_Bbuf_SetPointerAt( slog->tmp_bbuf, lptr );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_STM_UpdateFRAME_Forward() - "
                              "SLOG_Bbuf_SetPointerAt( _tmp_, lptr ) fails\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }
        lptr = lptr->next;

        if ( irec_endtime <= frame_endtime ) {
            ierr = SLOG_Bbufs_MoveIntvlRec( slog->tmp_bbuf,
                                            slog->inc_bbuf );
            if ( ierr != SLOG_SUCCESS ) {
                fprintf( errfile, __FILE__":SLOG_STM_UpdateFRAME_Forward() - "
                                  "SLOG_Bbufs_MoveIntvlRec( _tmp_ -> _inc_ ) "
                                  "fails\n" );
                fflush( errfile );
                return SLOG_FAIL;
            }
        }
        else {   /*  if ( irec_endtime > frame_endtime )  */
            ierr = SLOG_Bbufs_MoveIntvlRec( slog->tmp_bbuf,
                                            slog->pas_bbuf );
            if ( ierr != SLOG_SUCCESS ) {
                fprintf( errfile, __FILE__":SLOG_STM_UpdateFRAME_Forward() - "
                                  "SLOG_Bbufs_MoveIntvlRec( _tmp_ -> _pas_ ) "
                                  "fails\n" );
                fflush( errfile );
                return SLOG_FAIL;
            }
        }

    }   /*  Endof while ( lptr != NULL ) for tmp_bbuf  */


    /*
        Split the interval records in CURRENT Bbuf into the all the other
        Bbufs, INCOMING, OUTGOING and PASSING THROUGH Bbufs according
        to the timestamp limits set for each sectors
    */
    lptr = SLOG_Bbuf_GetPointerAtHead( slog->cur_bbuf );
    while ( lptr != NULL ) {

        irec_starttime = ( lptr->irec )->starttime;
        irec_endtime   = ( lptr->irec )->starttime + ( lptr->irec )->duration;
        ierr = SLOG_Bbuf_SetPointerAt( slog->cur_bbuf, lptr );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_STM_UpdateFRAME_Forward() - "
                              "SLOG_Bbuf_SetPointerAt( _cur_, lptr ) fails\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }
        lptr = lptr->next;

        if ( irec_endtime <= frame_endtime ) {
            if ( irec_starttime < frame_starttime ) {
                ierr = SLOG_Bbufs_MoveIntvlRec( slog->cur_bbuf,
                                                slog->inc_bbuf );
                if ( ierr != SLOG_SUCCESS ) {
                    fprintf( errfile, __FILE__
                                      ":SLOG_STM_UpdateFRAME_Forward() - "
                                      "SLOG_Bbufs_MoveIntvlRec( _cur_ "
                                      "-> _inc_ ) fails\n" );
                    fflush( errfile );
                    return SLOG_FAIL;
                }
            }
            else {  /*  if ( irec_starttime >= frame_starttime )  */
                /*  Leave the irec in slog->cur_bbuf  */
            }
        }
        else {  /*  if ( irec_endtime > frame_endtime )  */
            if ( irec_starttime < frame_starttime ) {
                ierr = SLOG_Bbufs_MoveIntvlRec( slog->cur_bbuf,
                                                slog->pas_bbuf );
                if ( ierr != SLOG_SUCCESS ) {
                    fprintf( errfile, __FILE__
                                      ":SLOG_STM_UpdateFRAME_Forward() - "
                                      "SLOG_Bbufs_MoveIntvlRec( _cur_ "
                                      "-> _pas_ ) fails\n" );
                    fflush( errfile );
                    return SLOG_FAIL;
                }
            }
            else {  /*  if ( irec_starttime >= frame_starttime )  */
                ierr = SLOG_Bbufs_MoveIntvlRec( slog->cur_bbuf,
                                                slog->out_bbuf );
                if ( ierr != SLOG_SUCCESS ) {
                    fprintf( errfile, __FILE__
                                      ":SLOG_STM_UpdateFRAME_Forward() - "
                                      "SLOG_Bbufs_MoveIntvlRec( _cur_ "
                                      "-> _out_ ) fails\n" );
                    fflush( errfile );
                    return SLOG_FAIL;
                }
            }
        }  
        
    }   /*  Endof while ( lptr != NULL ) for cur_bbuf  */

#if defined( DEBUG )
    if ( ! SLOG_Bbuf_IsEmpty( slog->tmp_bbuf ) ) {
        fprintf( errfile, __FILE__":SLOG_STM_UpdateFRAME_Forward() - "
                          "slog->tmp_bbuf is NON-empty\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    /*
        After the distribution of irec in slog->tmp_bbuf and
        slog->cur_bbuf in all the other bbufs.  Update the
        Frame Header data structure before Writing the Frame
        and Reinitialization of the Frame data.  i.e. Number 
        of Interval records and Number of bytes in each sectors.
    */
    SLOG_STM_UpdateFrameHdr( slog );

    return SLOG_SUCCESS;
}



/*
    SLOG_STM_UpdateFrameDirEntry_Backward() = Update the buffer's Frame 
    Directory Record's Fptr2FrameHdr, StartTime & EndTime 

    So, for the 1st frame, (slog->frame_dir).entries[ 0 ],
    SLOG_STM_UpdateFrameDirEntry_Backward( slog ) HAS To Be Called
    BEFORE any operations on the various sectors of the frame, i.e. xxx_bbuf
    because the starttime and endtime of the frame determines where
    where each interval record should go.
*/
int SLOG_STM_UpdateFRAME_Backward( SLOG_STREAM *slog )
{
    /* const SLOG_intvlrec_lptr_t  *lptr;  */
          SLOG_intvlrec_lptr_t  *lptr;
          SLOG_intvlrec_t       *irec;
          SLOG_time              frame_starttime;
          SLOG_time              frame_endtime;
          SLOG_time              irec_starttime;
          SLOG_uint32            Nbytes_changed;
          int                    ierr;


    frame_starttime = ( slog->frame_dir_entry ).starttime;
    frame_endtime   = ( slog->frame_dir_entry ).endtime;
    Nbytes_changed  = 0;

    /*
        Split interval records in TEMPORARY Bbuf into PASSING THROUGH
        and OUTGOING Bbufs according to the criterion
        ( exceeds the Frame's starttime )
    */
    lptr = SLOG_Bbuf_GetPointerAtHead( slog->tmp_bbuf );
    while ( lptr != NULL ) {

        /*
            Save a handle to irec in lptr for later use 
            before lptr is updated below
        */
        irec           = lptr->irec;
        irec_starttime = irec->starttime;

        /*  Set slog->tmp_bbuf->lptr = lptr  */
        ierr = SLOG_Bbuf_SetPointerAt( slog->tmp_bbuf, lptr );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_STM_UpdateFRAME_Backward() - "
                              "SLOG_Bbuf_SetPointerAt( _tmp_, lptr ) fails\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }
        lptr = lptr->next;

        if ( irec_starttime >= frame_starttime ) {
            if (    SLOG_Bbuf_LocateIntvlRec( slog->out_bbuf, irec )
                 == NULL ) {  /*  _tmp_'s irec is NOT in _out_  */
                ierr = SLOG_Bbufs_MoveIntvlRec( slog->tmp_bbuf,
                                                slog->out_bbuf );
                if ( ierr != SLOG_SUCCESS ) {
                    fprintf( errfile, __FILE__
                                      ":SLOG_STM_UpdateFRAME_Backward() - "
                                      "SLOG_Bbufs_MoveIntvlRec( _tmp_ "
                                      "-> _out_ ) fails\n" );
                    fflush( errfile );
                    return SLOG_FAIL;
                }
                Nbytes_changed += irec->bytesize;
            }
            else {  /*  _tmp_'s irec is in _out_  */
                ierr = SLOG_Bbuf_RemoveNodeLinks( slog->tmp_bbuf );
                if ( ierr != SLOG_SUCCESS ) {
                    fprintf( errfile, __FILE__
                                      ":SLOG_STM_UpdateFRAME_Backward() - "
                                      "SLOG_Bbuf_RemoveNodeLinks() fails "
                                      "when irec is in slog->out_bbuf\n" );
                    fflush( errfile );
                    return SLOG_FAIL;
                }
                SLOG_Bbuf_FreeNode( slog->tmp_bbuf );
            }
        }
        else {   /*  if ( irec_endtime < frame_endtime )  */
            if (    SLOG_Bbuf_LocateIntvlRec( slog->pas_bbuf, irec )
                 == NULL ) {   /*  _tmp_'s irec is NOT in _pas_  */
                ierr = SLOG_Bbufs_MoveIntvlRec( slog->tmp_bbuf,
                                                slog->pas_bbuf );
                if ( ierr != SLOG_SUCCESS ) {
                    fprintf( errfile, __FILE__
                                      ":SLOG_STM_UpdateFRAME_Backward() - "
                                      "SLOG_Bbufs_MoveIntvlRec( _tmp_ "
                                      "-> _pas_ ) fails\n" );
                    fflush( errfile );
                    return SLOG_FAIL;
                }
                Nbytes_changed += irec->bytesize;
            }
            else {   /*  _tmp_'s irec is in _pas_  */
                ierr = SLOG_Bbuf_RemoveNodeLinks( slog->tmp_bbuf );
                if ( ierr != SLOG_SUCCESS ) {
                    fprintf( errfile, __FILE__
                                      ":SLOG_STM_UpdateFRAME_Backward() - "
                                      "SLOG_Bbuf_RemoveNodeLinks() fails "
                                      "when irec is in slog->pas_bbuf\n" );
                    fflush( errfile );
                    return SLOG_FAIL;
                }
                SLOG_Bbuf_FreeNode( slog->tmp_bbuf );
            }
        }

    }   /*  Endof while ( lptr != NULL ) for tmp_bbuf  */

#if defined( DEBUG )
    if ( ! SLOG_Bbuf_IsEmpty( slog->tmp_bbuf ) ) {
        fprintf( errfile, __FILE__":SLOG_STM_UpdateFRAME_Backward() - "
                          "slog->tmp_bbuf is NON-empty\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    /*
        After the distribution of irec in slog->tmp_bbuf and
        into slog->out_bbuf and slog->pas_bbuf.  Update the
        Frame Header data structure before Writing the Frame
        and Reinitialization of the Frame data.  i.e. Number 
        of Interval records and Number of bytes in each sectors.
    */
    SLOG_STM_UpdateFrameHdr( slog );

    return Nbytes_changed;
}



int SLOG_STM_WriteFRAMEatIdx(       SLOG_STREAM *slog,
                              const SLOG_uint32  idx_of_cur_frame )
{
    SLOG_fptr   ifptr2framehdr;
    int         ierr;

    if ( idx_of_cur_frame >= (slog->frame_dir).Nframe ) {
        fprintf( errfile, __FILE__":SLOG_STM_WriteFRAMEatIdx() - \n" );
        fprintf( errfile, "\t""the input index("fmt_ui32") is bigger "
                          "the Number of Frames("fmt_ui32") in "
                          "File Frame Directory\n",
                          idx_of_cur_frame, (slog->frame_dir).Nframe );
        fflush( errfile );
        return SLOG_FAIL;
    }

    ifptr2framehdr
        = ( (slog->frame_dir).entries[ idx_of_cur_frame ] ).fptr2framehdr;
#if defined( DEBUG )
    fprintf( errfile, __FILE__":SLOG_STM_WriteFRAMEatIdx("fmt_ui32") - "
                      "ifptr2framehdr = "fmt_fptr"\n", idx_of_cur_frame,
                      ifptr2framehdr );
    fflush( errfile );
#endif

    ierr = slog_fseek( slog->fd, ifptr2framehdr, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_STM_WriteFRAMEatIdx() - "
                          "slog_fseek("fmt_fptr") fails\n", ifptr2framehdr );
        fflush( errfile );
        return SLOG_EOF;
    }
    ierr = SLOG_STM_WriteFRAME( slog );
    if ( ierr == SLOG_FAIL ) {
        fprintf( errfile, __FILE__":SLOG_STM_WriteFRAMEatIdx("fmt_ui32") - "
                          "SLOG_STM_WriteFRAME() fails\n", idx_of_cur_frame );
        fflush( errfile );
        return SLOG_FAIL;
    }

    return ierr;
}



int SLOG_STM_WriteFRAME( SLOG_STREAM *slog )
{
    SLOG_uint32 Nbytes_written;
    int         ierr;

#if defined( DEBUG )
    SLOG_fptr  slogfile_loc_cur;
#endif 


    Nbytes_written = 0;

  /*  Output buffer's frame header to the filebuffer  */
    ierr = SLOG_STM_DepositFrameHdr( &( slog->frame_hdr ), slog->fbuf );
    if ( ierr == SLOG_FAIL ) {
        fprintf( errfile, __FILE__":SLOG_STM_WriteFRAME() - "
                          "SLOG_STM_DepositFrameHdr( slog->Frame_Hdr ) "
                          "fails!!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr;

  /*  Output all buffers' interval records to the filebuffer  */
    ierr = SLOG_STM_DepositBBuf( slog->cur_bbuf, slog->fbuf );
    if ( ierr == SLOG_FAIL ) {
        fprintf( errfile, __FILE__":SLOG_STM_WriteFRAME() - "
                          "SLOG_STM_DepositBBuf( slog->CURRENT_Bbuf ) "
                          "fails!!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr;

    ierr = SLOG_STM_DepositBBuf( slog->inc_bbuf, slog->fbuf );
    if ( ierr == SLOG_FAIL ) {
        fprintf( errfile, __FILE__":SLOG_STM_WriteFRAME() - "
                          "SLOG_STM_DepositBBuf( slog->INCOMING_Bbuf ) "
                          "fails!!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr;

    ierr = SLOG_STM_DepositBBuf( slog->pas_bbuf, slog->fbuf );
    if ( ierr == SLOG_FAIL ) {
        fprintf( errfile, __FILE__":SLOG_STM_WriteFRAME() - "
                          "SLOG_STM_DepositBBuf( slog->PASSING_THROUGH_Bbuf ) "
                          "fails!!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr;

    ierr = SLOG_STM_DepositBBuf( slog->out_bbuf, slog->fbuf );
    if ( ierr == SLOG_FAIL ) {
        fprintf( errfile, __FILE__":SLOG_STM_WriteFRAME() - "
                          "SLOG_STM_DepositBBuf( slog->OUTGOING_Bbuf ) "
                          "fails!!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr;

#if defined( DEBUG )
        fprintf( errfile, __FILE__":SLOG_STM_WriteFRAME() - A\n" );
        slogfile_loc_cur = slog_ftell( slog->fd );
        if ( slogfile_loc_cur != ( slog->frame_dir_entry ).fptr2framehdr ) {
            fprintf( errfile, __FILE__":SLOG_STM_WriteFRAME() - "
                              "Inconsistency in File Pointers BEFORE calling "
                              "fbuf_empty( frame buffer ) at line %d\n",
                              __LINE__ );
            fprintf( errfile, "       ""- slogfile_loc_cur = "fmt_fptr", but "
                              "( slog->frame_dir_entry ).fptr2framehdr = "
                              fmt_fptr"\n", slogfile_loc_cur,
                              ( slog->frame_dir_entry ).fptr2framehdr );
            fflush( errfile );
        }
#endif

  /*
      Empty the SLOG_STREAM's filebuffer to the file/disk, it is equivalent
      to Paddig the SLOGFILE in the unit of slog->hdr->frame_bytesize block
  */
    ierr = fbuf_empty( slog->fbuf );
    if ( ierr == EOF ) {
        fprintf( errfile, __FILE__":SLOG_STM_WriteFRAME() - "
                          "fbuf_empty( frame buffer ) fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    fflush( slog->fd );

#if defined( ___DEBUG )
        fprintf( errfile, __FILE__":SLOG_STM_WriteFRAME() - B\n" );
        slogfile_loc_cur = slog_ftell( slog->fd );
        if ( slogfile_loc_cur - slog->file_loc != fbuf_bufsz( slog->fbuf ) ) {
            fprintf( errfile, __FILE__":SLOG_STM_WriteFRAME() - "
                              "Inconsistency in File Pointers AFTER calling "
                              "fbuf_empty( frame buffer ) at line %d\n",
                              __LINE__ );
            fprintf( errfile, "       ""- slog->file_loc = "fmt_fptr", and "
                              "slogfile_loc_cur = "fmt_fptr", but "
                              "( slog->frame_dir_entry ).fptr2framehdr = "
                              fmt_fptr"\n", slog->file_loc, slogfile_loc_cur,
                              ( slog->frame_dir_entry ).fptr2framehdr );
            fflush( errfile );
        }
#endif

  /*
      Update the SLOG_STREAM's current slog file location
  */
    slog->file_loc = slog_ftell( slog->fd );

    return Nbytes_written;
}



/*
    SLOG_STM_ReadFRAME() should be called SLOG_STM_ReadFRAME_Forward()
    as the subroutine advances slog->file_dir_cur by 1 unit.
*/
int SLOG_STM_ReadFRAME( SLOG_STREAM *slog )
{
    int         idx_of_cur_frame;
    int         ierr;

#if defined( DEBUG )
    SLOG_fptr             ifptr2framehdr;
    SLOG_fptr             slogfile_loc_cur;
    /* Number of times this routine is called */
    static unsigned int   count_time = 0; 

    count_time += 1;
    fprintf( errfile, __FILE__":SLOG_STM_ReadFRAME() - A\n" );
#endif

    /*  Check if the current frame is the last frame  */
    ierr = SLOG_STM_IdxOfCurFrameDirEntry( slog );
    if ( ierr == SLOG_FAIL ) {
        fprintf( errfile, __FILE__":SLOG_STM_ReadFRAME() - "
                          "SLOG_STM_IdxOfCurFrameDirEntry() fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    if ( ierr == SLOG_EOF ) return SLOG_EOF;
    idx_of_cur_frame = ierr;

#if defined( DEBUG )
    if ( count_time > 1 ) {
        slogfile_loc_cur = slog_ftell( slog->fd );
        if ( idx_of_cur_frame >= ( slog->frame_dir ).Nframe ) {
            fprintf( errfile, __FILE__":SLOG_STM_ReadFRAME() - "
                              "SLOG_STM_IdxOfCurFrameDirEntry() returns %d "
                              ">= ( slog->frame_dir ).Nframe( "fmt_ui32" )\n",
                              idx_of_cur_frame, ( slog->frame_dir ).Nframe );
            fflush( errfile );
            /* return SLOG_FAIL; */
        }

        ifptr2framehdr
        = ( (slog->frame_dir).entries[ idx_of_cur_frame ] ).fptr2framehdr;

        if ( slogfile_loc_cur != ifptr2framehdr ) {
            fprintf( errfile, __FILE__":SLOG_STM_ReadFRAME() - "
                              "Inconsistency in File Pointers BEFORE calling "
                              "fbuf_refill( frame buffer ) at line %d\n",
                              __LINE__ );
            fprintf( errfile, "       ""- slogfile_loc_cur("fmt_fptr") != "
                              "( slog->frame_dir_entry ).fptr2framehdr("
                              fmt_fptr")\n", slogfile_loc_cur, ifptr2framehdr );
            fflush( errfile );
            /* return SLOG_FAIL; */
        }
    }
#endif

    ierr = SLOG_STM_ReadFRAMEatIdx( slog, idx_of_cur_frame );
    if ( ierr == SLOG_FAIL ) {
        fprintf( errfile, __FILE__":SLOG_STM_ReadFRAME() - "
                          "Unexpected termination of "
                          "SLOG_STM_ReadFRAMEatIdx(%d)\n",
                          idx_of_cur_frame );
        fflush( errfile );
        return ierr;
    }

  /*
      Update the SLOG_STREAM's file pointer to the File Directory Entry
  */
    slog->file_dir_cur += SLOG_typesz[ FrameDirEntry ];

    return ierr;
}

    

int SLOG_STM_ReadFRAMEatIdx(       SLOG_STREAM *slog,
                             const SLOG_uint32  idx_of_cur_frame )
{
    SLOG_uint32 Nbytes_read;
    SLOG_fptr   ifptr2framehdr;
    int         ierr;

    if ( idx_of_cur_frame >= (slog->frame_dir).Nframe ) {
        fprintf( errfile, __FILE__":SLOG_STM_WriteFRAMEatIdx() - \n" );
        fprintf( errfile, "\t""the input index("fmt_ui32") is bigger "
                          "the Number of Frames("fmt_ui32") in "
                          "File Frame Directory\n",
                          idx_of_cur_frame, (slog->frame_dir).Nframe );
        fflush( errfile );
        return SLOG_FAIL;
    }

    ifptr2framehdr
        = ( (slog->frame_dir).entries[ idx_of_cur_frame ] ).fptr2framehdr;

    /* WE CAN UPDATE slog->frame_dir_entry HERE */

  /*
      Read the frame buffer from the disk/file according to SLOG_FrameDir
  */
#if defined( DEBUG )
    fprintf( errfile, __FILE__":SLOG_STM_ReadFRAMEatIdx("fmt_ui32") - "
                      "ifptr2framehdr = "fmt_fptr"\n", idx_of_cur_frame,
                      ifptr2framehdr );
    fflush( errfile );
#endif

    ierr = slog_fseek( slog->fd, ifptr2framehdr, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_STM_ReadFRAMEatIdx() - "
                          "slog_fseek("fmt_fptr") fails\n", ifptr2framehdr );
        fflush( errfile );
        return SLOG_EOF;
    }
    ierr = fbuf_refill( slog->fbuf );
    if ( ierr == EOF ) {
        fprintf( errfile, __FILE__":SLOG_STM_ReadFRAMEatIdx() - "
                          "fbuf_refill( frame buffer ) fails\n" );
        fflush( errfile );
        return SLOG_EOF;
    }
  /*
      Update the SLOG_STREAM's current slog file location, slog->file_loc,
  */
    slog->file_loc      = slog_ftell( slog->fd );

    Nbytes_read = 0;

    /*  Get the Frame Header from the frame buffer  */
    ierr = SLOG_STM_WithdrawFrameHdr( &( slog->frame_hdr ), slog->fbuf );
    if ( ierr == SLOG_FAIL ) {
        fprintf( errfile, __FILE__":SLOG_STM_ReadFRAMEatIdx() - "
                          "SLOG_STM_WithdrawFrameHdr() fails !!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ierr;

    /*  Get the CURRENT  Bbuf from the frame buffer  */
    ( slog->cur_bbuf )->Nbytes_in_file = ( slog->frame_hdr ).Nbytes_cur; 
    ( slog->cur_bbuf )->count_irec     = ( slog->frame_hdr ).Nrec_cur;
    ierr = SLOG_STM_WithdrawBBuf( slog->cur_bbuf, slog->rec_defs, slog->fbuf );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_STM_ReadFRAMEatIdx() - "
                          "SLOG_STM_WithdrawBBuf( slog->CURRENT_Bbuf ) "
                          "fails !!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ( slog->cur_bbuf )->Nbytes_in_file;

    /*  Get the INCOMING Bbuf from the frame buffer  */
    ( slog->inc_bbuf )->Nbytes_in_file = ( slog->frame_hdr ).Nbytes_inc; 
    ( slog->inc_bbuf )->count_irec     = ( slog->frame_hdr ).Nrec_inc;
    ierr = SLOG_STM_WithdrawBBuf( slog->inc_bbuf, slog->rec_defs, slog->fbuf );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_STM_ReadFRAMEatIdx() - "
                          "SLOG_STM_WithdrawBBuf( slog->INCOMING_Bbuf ) "
                          "fails !!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ( slog->inc_bbuf )->Nbytes_in_file;

    /*  Get the PASSING  Bbuf from the frame buffer  */
    ( slog->pas_bbuf )->Nbytes_in_file = ( slog->frame_hdr ).Nbytes_pas; 
    ( slog->pas_bbuf )->count_irec     = ( slog->frame_hdr ).Nrec_pas;
    ierr = SLOG_STM_WithdrawBBuf( slog->pas_bbuf, slog->rec_defs, slog->fbuf );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_STM_ReadFRAMEatIdx() - "
                          "SLOG_STM_WithdrawBBuf( slog->PASSING_Bbuf ) "
                          "fails !!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ( slog->pas_bbuf )->Nbytes_in_file;

    /*  Get the OUTGOING Bbuf from the frame buffer  */
    ( slog->out_bbuf )->Nbytes_in_file = ( slog->frame_hdr ).Nbytes_out; 
    ( slog->out_bbuf )->count_irec     = ( slog->frame_hdr ).Nrec_out;
    ierr = SLOG_STM_WithdrawBBuf( slog->out_bbuf, slog->rec_defs, slog->fbuf );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_STM_ReadFRAMEatIdx() - "
                          "SLOG_STM_WithdrawBBuf( slog->OUTGOING_Bbuf ) "
                          "fails !!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_read += ( slog->out_bbuf )->Nbytes_in_file;

    return Nbytes_read;
}



int SLOG_STM_UpdateWriteFRAMEDIR( SLOG_STREAM *slog )
{
    int ierr;

      /*
          Update the SLOG FILE's corresponding Frame Directory Entry
          by reading the SLOG_STREAM.file_dir_rec
      */
        ierr = SLOG_WriteUpdatedFrameDirEntry( slog );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_STM_UpdateWriteFRAMEDIR() - "
                              "SLOG_WriteUpdatedFrameDirEntry( slog ) "
                              "fails!!\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }
        ierr = SLOG_STM_IncrementFrameDirHdr( slog );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_STM_UpdateWriteFRAMEDIR() - "
                              "SLOG_STM_IncrementFrameDirHdr( slog ) "
                              "fails!!\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }
        ierr = SLOG_STM_WriteFrameDirHdr( slog );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_STM_UpdateWriteFRAMEDIR() - "
                              "SLOG_STM_WriteFrameDirHdr( slog ) "
                              "fails!!\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }
    return SLOG_SUCCESS;
}



void SLOG_STM_InitAgainBeforeAddRec( SLOG_STREAM *slog )
{
    const SLOG_uint32 zero = (SLOG_uint32) 0;
   
   /*
        Copy the content of PASSING THROUGH and OUTGOING Bbufs
        to TEMPORARY Bbufs which will be used to fill the next
        frame's INCOMING and PASSING THROUGH Bbufs.  Cannot call
        SLOG_Bbuf_DelAllNodes() on slog->out_bbuf, slog->pas_bbuf,
        and slog->tmp_bbuf.  Since slog->tmp_bbuf is created from
        merging slog->out_bbuf and slog_pas_bbuf without copying
        of the irec's( i.e. no malloc() ).  So the actual linked
        list needed to be retained so slog->tmp_bbuf can be
        referenced in the next frame.  
    */
    SLOG_Bbuf_CopyBBufVal( slog->out_bbuf, slog->tmp_bbuf );
    SLOG_Bbufs_Concatenate( slog->pas_bbuf, slog->tmp_bbuf );
    SLOG_Bbuf_DelAllNodes( slog->cur_bbuf );
    SLOG_Bbuf_DelAllNodes( slog->inc_bbuf );

  /*  Reinitialize the Frame Header for next frame buffer  */
    SLOG_STM_UpdateFrameHdrGiven( slog, zero, zero, zero, zero,
                                        zero, zero, zero, zero );

  /*  Reinitialize the bi-directional list buffers & related variables  */
    SLOG_Bbuf_Init( slog->cur_bbuf );
    SLOG_Bbuf_Init( slog->inc_bbuf );
    SLOG_Bbuf_Init( slog->pas_bbuf );
    SLOG_Bbuf_Init( slog->out_bbuf );
}



void SLOG_STM_InitAgainBeforePatchFrame( SLOG_STREAM *slog )
{
    const SLOG_uint32 zero = (SLOG_uint32) 0;
  
   /*
        Copy the content of PASSING THROUGH and INCOMING Bbufs
        to TEMPORARY Bbufs which will be used to fill the next
        frame's OUTGOING and PASSING THROUGH Bbufs.  Cannot call
        SLOG_Bbuf_DelAllNodes() on slog->inc_bbuf, slog->pas_bbuf,
        and slog->tmp_bbuf.  Since slog->tmp_bbuf is created from
        merging slog->inc_bbuf and slog_pas_bbuf without copying
        of the irec's( i.e. no malloc() ).  So the actual linked
        list needed to be retained so slog->tmp_bbuf can be
        referenced in the next frame.  
    */
    SLOG_Bbuf_CopyBBufVal( slog->inc_bbuf, slog->tmp_bbuf );
    SLOG_Bbufs_Concatenate( slog->pas_bbuf, slog->tmp_bbuf );
    SLOG_Bbuf_DelAllNodes( slog->cur_bbuf );
    SLOG_Bbuf_DelAllNodes( slog->out_bbuf );

  /*  Reinitialize the Frame Header for next frame buffer  */
    SLOG_STM_UpdateFrameHdrGiven( slog, zero, zero, zero, zero,
                                        zero, zero, zero, zero );

  /*  Reinitialize the bi-directional list buffers & related variables  */
    SLOG_Bbuf_Init( slog->cur_bbuf );
    SLOG_Bbuf_Init( slog->inc_bbuf );
    SLOG_Bbuf_Init( slog->pas_bbuf );
    SLOG_Bbuf_Init( slog->out_bbuf );
}



void SLOG_STM_PrintRecDefs( const SLOG_STREAM *slog, FILE *outfd )
{
    if ( slog != NULL && slog->rec_defs != NULL )
       SLOG_RDEF_Print( outfd, slog->rec_defs );
}



void SLOG_STM_PrintFrameDirHdr( const SLOG_STREAM *slog, FILE *outfd )
{
    if ( slog != NULL ) 
        fprintf( outfd, "       "fmt_fptr" "fmt_fptr" "fmt_ui32"\n",
                        ( slog->frame_dir ).prevdir,
                        ( slog->frame_dir ).nextdir,
                        ( slog->frame_dir ).Nframe );
}



void SLOG_STM_PrintFrameDirEntries( const SLOG_STREAM *slog, FILE *outfd )
{
    int ii;

    if ( slog != NULL && (slog->frame_dir).entries != NULL )
        for ( ii = 0; ii < (slog->frame_dir).Nframe; ii ++ ) {
            fprintf( outfd, "       "fmt_fptr" "fmt_time" "fmt_time"\n",
                            ( (slog->frame_dir).entries[ ii ] ).fptr2framehdr,
                            ( (slog->frame_dir).entries[ ii ] ).starttime,
                            ( (slog->frame_dir).entries[ ii ] ).endtime );
            fflush( outfd );
        }
}
