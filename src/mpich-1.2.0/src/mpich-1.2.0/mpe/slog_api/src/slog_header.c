#include <stdlib.h>
#include "slog_fileio.h"
#include "slog_header.h"


SLOG_hdr_t *SLOG_HDR_Create( FILE  *outfd )
{
    SLOG_hdr_t  *hdr;

    hdr = ( SLOG_hdr_t * ) malloc( sizeof( SLOG_hdr_t ) );

    /*  Initialize the various fields in the SLOG_hdr_t data structure  */
    hdr->version[ 0 ]             = VERSION_MainID;
    hdr->version[ 1 ]             = VERSION_SubID;
    hdr->frame_bytesize           = SLOG_BUFFER_UNIT;
    hdr->frame_reserved_size      = SLOG_FRAME_RESERVED_SIZE;
    hdr->max_Ndirframe            = SLOG_NDirEntry;
    hdr->IsIncreasingStarttime    = SLOG_FALSE;
    hdr->IsIncreasingEndtime      = SLOG_FALSE;
    hdr->HasReserveSpaceBeenUsed  = SLOG_FALSE;
    hdr->fptr2statistics          = SLOG_fptr_NULL;
    hdr->fptr2preview             = SLOG_fptr_NULL;
    hdr->fptr2profile             = SLOG_fptr_NULL;
    hdr->fptr2threadtable         = SLOG_fptr_NULL;
    hdr->fptr2recdefs             = SLOG_fptr_NULL;
    hdr->fptr2framedata           = SLOG_fptr_NULL;

    /*  Initialize the file buffer and the location of header in file  */
    hdr->file_loc = SLOG_fptr_NULL;
    hdr->fbuf = ( filebuf_t * ) fbuf_create( SLOG_typesz[ Header ] );
    fbuf_filedesc( hdr->fbuf, outfd );

    return hdr;
}



void SLOG_HDR_Free( SLOG_hdr_t *hdr )
{
    if ( hdr != NULL && SLOG_typesz[ Header ] > 0 ) {
        if ( hdr->fbuf != NULL ) {
            fbuf_free( hdr->fbuf );
            hdr->fbuf = NULL;
        }
        free( hdr );
        hdr = NULL;
    }
}



/*@C
    SLOG_SetFrameByteSize - Set the byte size of the frame of SLOG file

  Unmodified Input Variables :
. frame_bytesize - unsigned integer to specify the size of the frame.

  Modified Input Variables :
. slog - pointer to SLOG_STREAM where interval records will be stored.

  Modified Output Variables :
. returned value - integer return status.

  Include File Needed :
  slog_header.h

@*/
int SLOG_SetFrameByteSize( SLOG_STREAM  *slog, SLOG_uint32 frame_bytesize )
{
          int         ierr;

    if ( slog == NULL ) {
        fprintf( errfile, __FILE__":SLOG_SetFrameByteSize() - "
                          "the input SLOG_STREAM is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->hdr == NULL ) {
        fprintf( errfile, __FILE__":SLOG_SetFrameByteSize() - "
                          "the input SLOG_hdr_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->hdr->frame_reserved_size != SLOG_FRAME_RESERVED_SIZE ) {
        if ( ( SLOG_typesz[ FrameHdr ]
             + SLOG_typesz[ min_IntvlRec ]
             + slog->hdr->frame_reserved_size )
             > frame_bytesize ) {
            fprintf( errfile, __FILE__":SLOG_SetFrameByteSize() - "
                              "the input frame byte size, "fmt_ui32", is "
                              "too small\n", frame_bytesize );
            fprintf( errfile, "\t""It is NOT even enough for 1 minimal "
                              "interval record.\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }
    }

    slog->hdr->frame_bytesize = frame_bytesize;
    ierr = SLOG_WriteUpdatedHeader( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_SetFrameByteSize() - "
                          "SLOG_WriteUpdatedHeader() fails\n" );
        fflush( errfile );
        return ierr;
    }

    return SLOG_SUCCESS;
}



/*@C
    SLOG_SetFrameReservedSpace - Set the byte size of the frame's 
                                 reserved space for SLOG file

  Unmodified Input Variables :
. reserved_bytesize - unsigned integer to specify the reserved byte
                      size of the frame.

  Modified Input Variables :
. slog - pointer to SLOG_STREAM where interval records will be stored.

  Modified Output Variables :
. returned value - integer return status.

  Usage Notes on this subroutine :
  The routine is one of the methods provided to reserve frame space
  for the pseudo records in the 1st pass of the logfile.  Reservation
  of the frame space is only necessary if the completed interval records
  are arranged in increasing endtime order and there is no way that
  user can rearrange the call sequence of the start event to call
  SLOG_Irec_ReserveSpace().  When the completed interval records 
  are arranged in increasing starttime order, there is no need
  to reserve space for the pseudo interval records, so this routine should
  NOT be called.  Also if user has knowledge of the start event of the
  interval record, like in an event based model where all start and end
  events are mixed and are arranged in increasing time order, one
  should call SLOG_Irec_ReserveSpace() instead of this routine
  to do frame space reservation.

  Include File Needed :
  slog_header.h

@*/
int SLOG_SetFrameReservedSpace( SLOG_STREAM  *slog,
                                SLOG_uint32   reserved_bytesize )
{
          int         ierr;

    if ( slog == NULL ) {
        fprintf( errfile, __FILE__":SLOG_SetFrameReservedSpace() - "
                          "the input SLOG_STREAM is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->hdr == NULL ) {
        fprintf( errfile, __FILE__":SLOG_SetFrameReservedSpace() - "
                          "the input SLOG_hdr_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->hdr->frame_bytesize != SLOG_BUFFER_UNIT ) {
        if ( ( SLOG_typesz[ FrameHdr ]
             + SLOG_typesz[ min_IntvlRec ]
             + reserved_bytesize )
             > slog->hdr->frame_bytesize ) {
            fprintf( errfile, __FILE__":SLOG_SetFrameReservedSpace() - "
                              "the input frame reserved size, "fmt_ui32", is "
                              "too big\n", reserved_bytesize );
            fprintf( errfile, "\t""It is NOT even enough for 1 minimal "
                              "interval record.\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }
    }

    slog->hdr->frame_reserved_size = reserved_bytesize;
    ierr = SLOG_WriteUpdatedHeader( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_SetFrameReservedSpace() - "
                          "SLOG_WriteUpdatedHeader() fails\n" );
        fflush( errfile );
        return ierr;
    }

    return SLOG_SUCCESS;
}



/*@C
    SLOG_SetMaxNumOfFramesPerDir - Set the Maximum Number of frames
                                   per directory in SLOG file

  Unmodified Input Variables :
. Ndirframe - unsigned integer to specify the maximum number of frames
              per directory in SLOG file.

  Modified Input Variables :
. slog - pointer to SLOG_STREAM where interval records will be stored.

  Modified Output Variables :
. returned value - integer return status.

  Include File Needed :
  slog_header.h
@*/
int SLOG_SetMaxNumOfFramesPerDir( SLOG_STREAM  *slog, SLOG_uint32 Ndirframe )
{
          int         ierr;

    if ( slog == NULL ) {
        fprintf( errfile, __FILE__":SLOG_SetMaxNumOfFramesPerDir() - "
                          "the input SLOG_STREAM is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->hdr == NULL ) {
        fprintf( errfile, __FILE__":SLOG_SetMaxNumOfFramesPerDir() - "
                          "the input SLOG_hdr_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    slog->hdr->max_Ndirframe = Ndirframe;
    ierr = SLOG_WriteUpdatedHeader( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_SetMaxNumOfFramesPerDir() - "
                          "SLOG_WriteUpdatedHeader() fails\n" );
        fflush( errfile );
        return ierr;
    }

    return SLOG_SUCCESS;
}



/*@C
    SLOG_SetIncreasingStarttimeOrder - Set the flag for the interval
                                       records will be arranged in
                                       increasing starttime order.  That
                                       means there is NO second pass
                                       of the logfile and the starttime of
                                       the interval will be used to determine
                                       endtime of the frame.

  Modified Input Variables :
. slog - pointer to SLOG_STREAM where interval records will be stored.

  Modified Output Variables :
. returned value - integer return status.

  Include File Needed :
  slog_header.h
@*/
int SLOG_SetIncreasingStarttimeOrder( SLOG_STREAM  *slog )
{
          int         ierr;

    if ( slog == NULL ) {
        fprintf( errfile, __FILE__":SLOG_SetIncreasingStarttimeOrder() - "
                          "the input SLOG_STREAM is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->hdr == NULL ) {
        fprintf( errfile, __FILE__":SLOG_SetIncreasingStarttimeOrder() - "
                          "the input SLOG_hdr_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    slog->hdr->IsIncreasingStarttime = SLOG_TRUE;
    ierr = SLOG_WriteUpdatedHeader( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_SetIncreasingStarttimeOrder() - "
                          "SLOG_WriteUpdatedHeader() fails\n" );
        fflush( errfile );
        return ierr;
    }

    return SLOG_SUCCESS;
}



/*@C
    SLOG_SetIncreasingEndtimeOrder - Set the flag for the interval
                                     records will be arranged in
                                     increasing endtime order.  That
                                     means there will be a second pass
                                     of the logfile and the endtime of
                                     the interval will be used to determine
                                     endtime of the frame.

  Modified Input Variables :
. slog - pointer to SLOG_STREAM where interval records will be stored.

  Modified Output Variables :
. returned value - integer return status.

  Include File Needed :
  slog_header.h
@*/
int SLOG_SetIncreasingEndtimeOrder( SLOG_STREAM  *slog )
{
          int         ierr;

    if ( slog == NULL ) {
        fprintf( errfile, __FILE__":SLOG_SetIncreasingEndtimeOrder() - "
                          "the input SLOG_STREAM is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( slog->hdr == NULL ) {
        fprintf( errfile, __FILE__":SLOG_SetIncreasingEndtimeOrder() - "
                          "the input SLOG_hdr_t is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    slog->hdr->IsIncreasingEndtime = SLOG_TRUE;
    ierr = SLOG_WriteUpdatedHeader( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_SetIncreasingEndtimeOrder() - "
                          "SLOG_WriteUpdatedHeader() fails\n" );
        fflush( errfile );
        return ierr;
    }

    return SLOG_SUCCESS;
}



/*
    SLOG_HDR_Deposit() - copy the content of SLOG_hdr_t into the file buffer
*/
int SLOG_HDR_Deposit( SLOG_hdr_t *hdr )
{
    SLOG_uint32  bsize;

    bsize = 0;

    bsize += fbuf_deposit( hdr->version, SLOG_typesz[ ui32 ], 2, hdr->fbuf )
           * SLOG_typesz[ ui32 ];
    bsize += fbuf_deposit( &( hdr->frame_bytesize ), 
                           SLOG_typesz[ ui32 ], 1, hdr->fbuf )
           * SLOG_typesz[ ui32 ];
    bsize += fbuf_deposit( &( hdr->frame_reserved_size ), 
                           SLOG_typesz[ ui32 ], 1, hdr->fbuf )
           * SLOG_typesz[ ui32 ];
    bsize += fbuf_deposit( &( hdr->max_Ndirframe ), 
                           SLOG_typesz[ ui32 ], 1, hdr->fbuf )
           * SLOG_typesz[ ui32 ];
    bsize += fbuf_deposit( &( hdr->IsIncreasingStarttime ),
                           SLOG_typesz[ ui16 ], 1, hdr->fbuf )
           * SLOG_typesz[ ui16 ];
    bsize += fbuf_deposit( &( hdr->IsIncreasingEndtime ),
                           SLOG_typesz[ ui16 ], 1, hdr->fbuf )
           * SLOG_typesz[ ui16 ];
    bsize += fbuf_deposit( &( hdr->HasReserveSpaceBeenUsed ),
                           SLOG_typesz[ ui16 ], 1, hdr->fbuf )
           * SLOG_typesz[ ui16 ];
    bsize += fbuf_deposit( &( hdr->fptr2statistics ),
                           SLOG_typesz[ fptr ], 1, hdr->fbuf )
           * SLOG_typesz[ fptr ];
    bsize += fbuf_deposit( &( hdr->fptr2preview ),
                           SLOG_typesz[ fptr ], 1, hdr->fbuf )
           * SLOG_typesz[ fptr ];
    bsize += fbuf_deposit( &( hdr->fptr2profile ),
                           SLOG_typesz[ fptr ], 1, hdr->fbuf )
           * SLOG_typesz[ fptr ];
    bsize += fbuf_deposit( &( hdr->fptr2threadtable ),
                           SLOG_typesz[ fptr ], 1, hdr->fbuf )
           * SLOG_typesz[ fptr ];
    bsize += fbuf_deposit( &( hdr->fptr2recdefs ),
                           SLOG_typesz[ fptr ], 1, hdr->fbuf )
           * SLOG_typesz[ fptr ];
    bsize += fbuf_deposit( &( hdr->fptr2framedata ),
                           SLOG_typesz[ fptr ], 1, hdr->fbuf )
           * SLOG_typesz[ fptr ];

    if ( bsize != SLOG_typesz[ Header ] ) {
        fprintf( errfile, __FILE__":SLOG_HDR_Deposit() - "
                          "the number of bytes deposited( "fmt_ui32" ) < "
                          "the expected( "fmt_ui32" )\n",
                          bsize, SLOG_typesz[ Header ] );
        fflush( errfile );
        return SLOG_FAIL;
    }

    return SLOG_SUCCESS;
}


/*
   SLOG_HDR_Withdraw() - withdraw from the file buffer to fill up SLOG_hdr_t
*/
int SLOG_HDR_Withdraw( SLOG_hdr_t *hdr )
{
    SLOG_uint32  bsize;

    bsize = 0;

    bsize += fbuf_withdraw( hdr->version, SLOG_typesz[ ui32 ], 2, hdr->fbuf )
           * SLOG_typesz[ ui32 ];
    bsize += fbuf_withdraw( &( hdr->frame_bytesize ),
                            SLOG_typesz[ ui32 ], 1, hdr->fbuf )
           * SLOG_typesz[ ui32 ];
    bsize += fbuf_withdraw( &( hdr->frame_reserved_size ),
                            SLOG_typesz[ ui32 ], 1, hdr->fbuf )
           * SLOG_typesz[ ui32 ];
    bsize += fbuf_withdraw( &( hdr->max_Ndirframe ), 
                            SLOG_typesz[ ui32 ], 1, hdr->fbuf )
           * SLOG_typesz[ ui32 ];
    bsize += fbuf_withdraw( &( hdr->IsIncreasingStarttime ),
                            SLOG_typesz[ ui16 ], 1, hdr->fbuf )
           * SLOG_typesz[ ui16 ];
    bsize += fbuf_withdraw( &( hdr->IsIncreasingEndtime ),
                            SLOG_typesz[ ui16 ], 1, hdr->fbuf )
           * SLOG_typesz[ ui16 ];
    bsize += fbuf_withdraw( &( hdr->HasReserveSpaceBeenUsed ),
                            SLOG_typesz[ ui16 ], 1, hdr->fbuf )
           * SLOG_typesz[ ui16 ];
    bsize += fbuf_withdraw( &( hdr->fptr2statistics ),
                            SLOG_typesz[ fptr ], 1, hdr->fbuf )
           * SLOG_typesz[ fptr ];
    bsize += fbuf_withdraw( &( hdr->fptr2preview ),
                            SLOG_typesz[ fptr ], 1, hdr->fbuf )
           * SLOG_typesz[ fptr ];
    bsize += fbuf_withdraw( &( hdr->fptr2profile ),
                            SLOG_typesz[ fptr ], 1, hdr->fbuf )
           * SLOG_typesz[ fptr ];
    bsize += fbuf_withdraw( &( hdr->fptr2threadtable ),
                            SLOG_typesz[ fptr ], 1, hdr->fbuf )
           * SLOG_typesz[ fptr ];
    bsize += fbuf_withdraw( &( hdr->fptr2recdefs ),
                            SLOG_typesz[ fptr ], 1, hdr->fbuf )
           * SLOG_typesz[ fptr ];
    bsize += fbuf_withdraw( &( hdr->fptr2framedata ),
                            SLOG_typesz[ fptr ], 1, hdr->fbuf )
           * SLOG_typesz[ fptr ];

    if ( bsize != SLOG_typesz[ Header ] ) {
        fprintf( errfile, __FILE__":SLOG_HDR_Withdraw() - "
                          "the number of bytes withdrawn( "fmt_ui32" ) < "
                          "the expected( "fmt_ui32" )\n",
                          bsize, SLOG_typesz[ Header ] );
        fflush( errfile );
        return SLOG_FAIL;
    }

    return SLOG_SUCCESS;
}

int SLOG_WriteHeader( SLOG_STREAM  *slog )
{
    slog->hdr = SLOG_HDR_Create( slog->fd );
    if ( slog->hdr == NULL ) {
        fprintf( errfile, __FILE__":SLOG_WriteHeader() - "
                          "SLOG_HDR_Create() fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  update the file pointer to the Header in file  */
    slog->hdr->file_loc = slog_ftell( slog->fd );
    
    if ( SLOG_HDR_Deposit( slog->hdr ) == SLOG_SUCCESS ) {
        fbuf_empty( slog->hdr->fbuf );
        fflush( slog->fd );
    }

    return SLOG_SUCCESS;
}


/*
    SLOG_WriteUpdatedHeader() is expected to be used in each basic building
    block of SLOG file.  Before each section is written to the disk.
    slog_ftell() is called to update its corresponding file pointer in the
    data structure, SLOG_hdr_t.  e.g. fptr2threadtable.  Once update
    is done, SLOG_WriteUpdatedHeader() is called to update the header.
*/
int SLOG_WriteUpdatedHeader( SLOG_STREAM  *slog )
{
    SLOG_fptr   file_loc_saved;
    int         ierr;

    /*  Save the current file pointer position  */
    file_loc_saved = slog_ftell( slog->fd );

    /*  Empty the Header into file using a temporary buffer  */
    if ( SLOG_HDR_Deposit( slog->hdr ) == SLOG_SUCCESS ) {
        ierr = slog_fseek( slog->fd, slog->hdr->file_loc, SEEK_SET );
        if ( ierr != 0 ) {
            fprintf( errfile, __FILE__":SLOG_WriteUpdatedHeader() - "
                              "slog_fseek( slog->hdr->file_loc ) fails\n" ); 
            fflush( errfile );
            return SLOG_FAIL;
        }
        fbuf_empty( slog->hdr->fbuf );
        fflush( slog->fd );
    }

    /*  Restore the saved file pointer position  */
    ierr = slog_fseek( slog->fd, file_loc_saved, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_WriteUpdatedHeader() - slog_fseek() "
                          "fails, cannot restore the orginal file position "
                          "before the end of the subroutine call\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    return SLOG_SUCCESS;
}



int SLOG_HDR_CheckVersionID( const SLOG_hdr_t  *hdr )
{
    if (    hdr->version[ 0 ] == VERSION_MainID
         && hdr->version[ 1 ] == VERSION_SubID )
        return SLOG_SUCCESS;
    else {
        fprintf( errfile, __FILE__":SLOG_HDR_CheckVersionID() - "
                          "Incompatible Version between the SLOG file "
                          "and the API used!\n" );
        fprintf( errfile, "\t""The SLOG file being read is generated with "
                          "version %d.%d SLOG-API library\n",
                          hdr->version[ 0 ], hdr->version[ 1 ] );
        fprintf( errfile, "\t""The API used is version %d.%d\n",
                          VERSION_MainID, VERSION_SubID );
        fflush( errfile );
        return SLOG_FAIL;
    }
}



int SLOG_ReadHeader( SLOG_STREAM *slog )
{
    int        ierr;

    slog->hdr = SLOG_HDR_Create( slog->fd );
    if ( slog->hdr == NULL ) {
        fprintf( errfile, __FILE__":SLOG_ReadHeader() - "
                          "SLOG_HDR_Create() fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  update the file pointer to the Header in file  */
    slog->hdr->file_loc = ( SLOG_fptr ) 0;
    ierr = slog_fseek( slog->fd, slog->hdr->file_loc, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_ReadHeader() - \n"
                          "\t""slog_fseek() fails, cannot seek to the "
                          "file position requested, "fmt_fptr"\n",
                          slog->hdr->file_loc );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Fill up the file buffer from the disk  */
    if ( fbuf_refill( slog->hdr->fbuf ) == EOF ) {
        fprintf( errfile, __FILE__":SLOG_ReadHeader() - \n"
                          "\t""fbuf_refill( slog->hdr->fbuf ) fails\n" );
        fflush( errfile );
        return SLOG_EOF;
    }

    /*  Withdraw data from the file buffer to fill up SLOG_hdr_t */
    if ( SLOG_HDR_Withdraw( slog->hdr ) != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_ReadHeader() - \n"
                          "\t""SLOG_HDR_Withdraw() detects inconsistency\n" );
        fflush( errfile );
        return SLOG_EOF;
    }

    if ( SLOG_HDR_CheckVersionID( slog->hdr ) != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_ReadHeader() - Warning!\n"
                          "\t""SLOG_HDR_CheckVersionID() returns incompatible "
                          "version ID !\n" );
        fflush( errfile );
        /*  return SLOG_FAIL;  */
    }

#if defined( DEBUG )
    fprintf( outfile, __FILE__":SLOG_ReadHeader() - Read :\n" );
    SLOG_HDR_Print( outfile, slog->hdr );
    fflush( outfile );
#endif

    return SLOG_SUCCESS;
}



void SLOG_HDR_Print( FILE *outfd, const SLOG_hdr_t  *hdr )
{
    fprintf( outfd, "\t""version         = "fmt_ui32"."fmt_ui32"\n",
                    hdr->version[0], hdr->version[1] );
    fprintf( outfd, "\t""byte size of the frame                = "fmt_ui32"\n",
                    hdr->frame_bytesize );
    fprintf( outfd, "\t""byte size of the frame reserved space = "fmt_ui32"\n",
                    hdr->frame_reserved_size );
    fprintf( outfd, "\t""max. number of frames per directory   = "fmt_ui32"\n",
                    hdr->max_Ndirframe );
    if ( hdr->IsIncreasingStarttime == SLOG_TRUE )
        fprintf( outfd, "\t""Is_Increasing_Starttime_Order         = TRUE\n" );
    else
        fprintf( outfd, "\t""Is_Increasing_Starttime_Order         = FALSE\n" );
    if ( hdr->IsIncreasingEndtime == SLOG_TRUE )
        fprintf( outfd, "\t""Is_Increasing_Endtime_Order           = TRUE\n" );
    else
        fprintf( outfd, "\t""Is_Increasing_Endtime_Order           = FALSE\n" );
    if ( hdr->HasReserveSpaceBeenUsed == SLOG_TRUE )
        fprintf( outfd, "\t""Has_ReserveSpace()_Been_Used          = TRUE\n" );
    else
        fprintf( outfd, "\t""Has_ReserveSpace()_Been_Used          = FALSE\n" );
    fprintf( outfd, "\t""location of Preview Statistics        = "fmt_fptr"\n",
                    hdr->fptr2statistics );
    fprintf( outfd, "\t""location of Preview                   = "fmt_fptr"\n",
                    hdr->fptr2preview );
    fprintf( outfd, "\t""location of Threads Table             = "fmt_fptr"\n",
                    hdr->fptr2threadtable );
    fprintf( outfd, "\t""location of Profile                   = "fmt_fptr"\n",
                    hdr->fptr2profile );
    fprintf( outfd, "\t""location of Record Definition Table   = "fmt_fptr"\n",
                    hdr->fptr2recdefs );
    fprintf( outfd, "\t""location of FRAMES Data               = "fmt_fptr"\n",
                    hdr->fptr2framedata );
    fflush( outfd );
}
