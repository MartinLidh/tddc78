#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "slog_fileio.h"
#include "slog_bbuf.h"
#include "slog_tasklabel.h"
#include "slog_vtrarg.h"
#include "slog_header.h"
#include "slog_pstat.h"
#include "slog_profile.h"
#include "slog_recdefs.h"
#include "slog_impl.h"                  /*I  "slog_impl.h"  I*/

/*@C
    SLOG_OpenOutputStream - Returns Output SLOG_STREAM associcated with
                            the given filename.

  Unmodified Input Variables :
. filename - constant char pointer to the filename.

  Modified Output Variables :
. returned value - pointer to the newly allocated SLOG_STREAM.  It is
                   NULL when the routine encounters error.

  Include File Needed :
  slog.h

.N SLOG_RETURN_STATUS
@*/
SLOG_STREAM *SLOG_OpenOutputStream( const char *filename )
{
    SLOG_STREAM *slog;


    SLOG_InitGlobalData();

    /* 
        memory for slog is allocated by SLOG_OpenStream()
        the "+" is for the 2nd Pass of the file
    */
    if ( ( slog = SLOG_OpenStream( filename, "w+b" ) ) == NULL )
        return NULL;

    if ( SLOG_WriteHeader( slog ) != SLOG_SUCCESS )
        return NULL;
    if ( SLOG_PSTAT_Open( slog ) != SLOG_SUCCESS )
        return NULL;

    return slog;
}



/*@C
    SLOG_Irec_ToOutputStream - store the given interval record in the frame
                               buffer and write it out to the disk when
                               the frame buffer is full.

   Modified Input Variables :
. slog - pointer to SLOG_STREAM where the interval record will be stored.

   Unmodified Input Variables :
. irec - pointer to constant interval record with the fields set by 
         the user before this routine is called.

   Modified Output Variables :
. returned value - integer return status.

   Usage Notes on this subroutine :
   The routine provides all the core management of the implementation of
   SLOG_STREAM.  Be sure to call SLOG_Irec_SetMinRec() at least once for
   the interval record to be appended to the SLOG_STREAM.  Also this routine
   can be called in either increasing starttime and endtime order depending
   if SLOG_IsIncreasingStarttimeOrder() or SLOG_IsIncreasingEndtimeOrder()
   is called.   But NOT both.  And there is time order checking code
   if user turns on the corresponding compiler flag.

  Include File Needed :
  slog.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_Irec_ToOutputStream(       SLOG_STREAM      *slog,
                              const SLOG_intvlrec_t  *irec )
{
    SLOG_uint32     old_buf_Nbytes_in_file;
    SLOG_uint32     new_buf_Nbytes_in_file;

    SLOG_uint32     actual_Nbytes_irec_reserved;
    SLOG_uint32     Nbytes_in_bbufs_patched;
    SLOG_uint32     Nbytes_written;
    SLOG_int32      irec_bytesize_estimate;
    int             IsLastFrame = SLOG_FALSE;
    int             ierr;

#if defined( CHECKTIMEORDER )
    SLOG_time       irec_endtime;
#endif
#if defined( COMPRESSION )
    SLOG_recdef_t      *recdef;
    SLOG_intvlinfo_t   *intvlinfo;
#endif
#if defined( DEBUG )
    long slogfile_loc_cur;
#endif

    if ( slog->HasIrec2IOStreamBeenUsed == SLOG_FALSE ) {
        /*  Make sure this block is executed once for each SLOG_STREAM  */
        slog->HasIrec2IOStreamBeenUsed = SLOG_TRUE;  
        ierr = SLOG_STM_CreateBbufs( slog );
        if ( ierr != SLOG_SUCCESS )
            return ierr;

        /*  Update the content of SLOG_hdr_t to both memory and disk  */
        slog->hdr->fptr2framedata = slog_ftell( slog->fd );
        ierr = SLOG_WriteUpdatedHeader( slog );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_Irec_ToOutputStream() - "
                                      "SLOG_WriteUpdatedHeader() fails\n" );
            fflush( errfile );
            return ierr;
        }

        /*  Write the content of SLOG Preview to the disk  */

        /*
           SLOG_STM_Init() can be called either __BEFORE__ or _AFTER_ 
           SLOG_WriteInitFrameDir().  SLOG_STM_Init() initializes the 
           Frame_Dir components and various Bbufs in the SLOG_STREAM.  
           So SLOG_WriteInitFrameDir() and SLOG_STM_Init() are independent
           from one another.
        */
        SLOG_STM_Init( slog );
        ierr = SLOG_WriteInitFrameDir( slog );
        if ( ierr != SLOG_SUCCESS )
            return ierr;

#if defined( CHECKTIMEORDER )
        /*
            Do a more comprehensive testing here, so we can simplify
            the testing later by using one flag, 
            slog->hdr->IsIncreasingEndtime
        */
        if (    slog->hdr->IsIncreasingEndtime    == SLOG_TRUE
             && slog->hdr->IsIncreasingStarttime  == SLOG_FALSE )
            slog->prev_endtime = irec->starttime + irec->duration;
        else if (    slog->hdr->IsIncreasingEndtime   == SLOG_FALSE
                  && slog->hdr->IsIncreasingStarttime == SLOG_TRUE  )
            slog->prev_starttime = irec->starttime;
        else {
            fprintf( errfile, __FILE__":SLOG_Irec_ToOutputStream() - "
                              "Inconsistent time order flags!\n" );
            if ( slog->hdr->IsIncreasingEndtime == SLOG_TRUE )
                fprintf( errfile, "\t""IsIncreasingEndtime = TRUE, " );
            else
                fprintf( errfile, "\t""IsIncreasingEndtime = FALSE, " );
            if ( slog->hdr->IsIncreasingStarttime == SLOG_TRUE )
                fprintf( errfile, "\t""IsIncreasingStarttime = TRUE\n" );
            else
                fprintf( errfile, "\t""IsIncreasingStarttime = FALSE\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }
#endif
    }   /*  Endof { if ( slog->HasIrec2IOStreamBeenUsed == SLOG_FALSE ) }  */

  /*  Check if the input interval record is empty  */
    if ( SLOG_Irec_IsEmpty( irec ) ) {
        fprintf( stdout, __FILE__":SLOG_Irec_ToOutputStream() - \n" );
        fprintf( stdout, "      ""Input Interval Record is empty\n" );
        fprintf( stdout, "      ""irec = " );
        SLOG_Irec_Print( irec, stdout ); fprintf( stdout, "\n" );
        fflush( stdout );
        return SLOG_FAIL;
    }

#if defined( DEBUG )
    if ( ! SLOG_Irec_IsConsistent( irec ) ) {
        fprintf( stdout, __FILE__":SLOG_Irec_ToOutputStream() - \n" );
        fprintf( stdout, "      ""Input Interval Record is InConsistent\n" );
        fprintf( stdout, "      ""irec = " );
        SLOG_Irec_Print( irec, stdout ); fprintf( stdout, "\n" );
        fflush( stdout );
        return SLOG_FAIL;
    }
#endif

  /*  Check if writing the input interval record to the buffer is possible  */
    old_buf_Nbytes_in_file = SLOG_typesz[ FrameHdr ]
                           + slog->hdr->frame_reserved_size
                           + ( slog->cur_bbuf )->Nbytes_in_file
                           + ( slog->tmp_bbuf )->Nbytes_in_file;
    if ( slog->reserve != NULL ) {
        /*
            Since the corresponding _Start_Event_ of the interval has been
            called with SLOG_Irec_ReserveSpace(), and the interval has NOT
            been added to any of the bbufs yet, so irec->Nbytes needs to be
            substracted from slog->reserve->Nbytes_eff to avoid 
            double counting of irec in space usage.
        */
        actual_Nbytes_irec_reserved = slog->reserve->Nbytes_eff
                                    - irec->bytesize;
        old_buf_Nbytes_in_file += actual_Nbytes_irec_reserved;
    }
    new_buf_Nbytes_in_file = old_buf_Nbytes_in_file + irec->bytesize;

#if defined( DEBUG )
    fprintf( stdout, __FILE__":SLOG_Irec_ToOutputStream() - \n" );
    fprintf( stdout, "\t""FrameHdr + sum_reserved_space + "
                     "_cur_ + _tmp_ = "fmt_ui32" bytes\n",
                     old_buf_Nbytes_in_file );
    fflush( stdout );
#endif

  /*
      Check if new buffer size is too big for the buffer
      If so, dump the contents in the Bi-dir linked list buffers first
      before adding another interval record to the buffers
  */
    if ( new_buf_Nbytes_in_file > fbuf_bufsz( slog->fbuf ) ) {

        /* 
            Reset the total allocated size of all completed and incompleted
            irec's by substracting it the size of completed irec's which will
            be saved into the frame/disk.
        */
        if ( slog->reserve != NULL ) {
            slog->reserve->Nbytes_tot -= slog->cur_bbuf->Nbytes_in_file;
            slog->reserve->Nrec_tot   -= slog->cur_bbuf->count_irec;
        }

        /*
            Update the buffer's Frame Directory Record's
            Fptr2FrameHdr, StartTime & EndTime 

            So, SLOG_STM_UpdateFrameDirEntry_Forward() HAS To Be Called
            BEFORE any operations on the various sectors of the frame,
            i.e. xxx_bbuf.  Because the starttime and endtime of the
            frame determines where where each interval record should go.
        */
        ierr = SLOG_STM_UpdateFrameDirEntry_Forward( slog, IsLastFrame );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_Irec_ToOutputStream() - \n"
                              "\t""SLOG_STM_UpdateFrameDirEntry_Forward( "
                              "slog ) fails!!\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }

        ierr = SLOG_STM_UpdateFRAME_Forward( slog );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_Irec_ToOutputStream() - \n"
                              "\t""SLOG_STM_UpdateFRAME_Forward( slog ) "
                              "fails!!\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }

        ierr = SLOG_STM_WriteFRAME( slog );
        if ( ierr == SLOG_FAIL ) {
            fprintf( errfile, __FILE__":SLOG_Irec_ToOutputStream() - "
                              "SLOG_STM_WriteFRAME( slog ) fails!!\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }
        Nbytes_written = ierr;

        ierr = SLOG_STM_UpdateWriteFRAMEDIR( slog );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_Irec_ToOutputStream() - \n"
                              "\t""SLOG_STM_UpdateWriteFRAMEDIR( slog ) "
                              "fails!!\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }

        SLOG_STM_InitAgainBeforeAddRec( slog );

#if 0
        /*  Check Number of Bytes written is consistent  */
        if ( Nbytes_written != old_buf_Nbytes_in_file ) {
            fprintf( errfile, __FILE__":SLOG_Irec_ToOutputStream() - "
                              "WARNING !!\n" );
            fprintf( errfile, "       ""SLOG_STM_WriteFRAME()'s "
                              "Nbytes_written {"fmt_ui32"} != SizeOf( "
                              "FrameHdr + sum_reserved_size + "
                              "_cur_ + _tmp_ ) {"fmt_ui32"}\n",
                              Nbytes_written , old_buf_Nbytes_in_file );
            fprintf( errfile, "        ""Continue with the program.\n" );
            fflush( errfile );
        }
#endif
    
    }  /*  Endof { if ( new_buf_Nbytes_in_file > fbuf( slog->fbuf ) ) }  */

#if defined( DEBUG )
    if ( slog->reserve != NULL ) {
        fprintf( stdout, "\t""reserve->Nbytes_eff = "fmt_i32", "
                         "reserve->Nbytes_tot = "fmt_i32"\n",
                          slog->reserve->Nbytes_eff,
                          slog->reserve->Nbytes_tot );
    }
#endif

#if defined( CHECKTIMEORDER )
    if ( slog->hdr->IsIncreasingEndtime == SLOG_TRUE ) {
        irec_endtime = irec->starttime + irec->duration;
        if ( irec_endtime < slog->prev_endtime ) {
            fprintf( errfile, __FILE__
                              ":SLOG_Irec_ToOutputStream() - Warning!!!\n"
                              "\t""Current endtime("fmt_time") is earlier "
                              "than previous endtime("fmt_time")\n",
                              irec_endtime, slog->prev_endtime );
            fflush( errfile );
        }
        slog->prev_endtime = irec_endtime;
    }
    else {
        if ( irec->starttime < slog->prev_starttime ) {
            fprintf( errfile, __FILE__
                              ":SLOG_Irec_ToOutputStream() - Warning!!!\n"
                              "\t""Current starttime("fmt_time") is earlier "
                              "than previous starttime("fmt_time")\n",
                              irec->starttime, slog->prev_starttime );
            fflush( errfile );
        }
        slog->prev_starttime = irec->starttime;
    }
#endif

    /*
        Check if all the previuous calls of SLOG_Irec_ReserveSpace() 
        and SLOG_Irec_ToOutputStream() have used up all the space
        on the disk.   In particular, this checks if too many of
        SLOG_Irec_ReserveSpace() calls have been used up or over-reserved
        all the space in the frame buffer. 

        This check can only be done in SLOG_Irec_ToOutputStream() instead
        of SLOG_Irec_ReserveSpace(), because there is no way to reset
        the slog->reserve->Nbytes_tot correctly in SLOG_Irec_ReserveSpace().
    */
    if ( slog->reserve != NULL ) { 
        Nbytes_in_bbufs_patched = SLOG_typesz[ FrameHdr ]
                                + slog->hdr->frame_reserved_size
                                + slog->reserve->Nbytes_tot;

        if ( Nbytes_in_bbufs_patched > fbuf_bufsz( slog->fbuf ) ) {
            fprintf( errfile, __FILE__":SLOG_Irec_ToOutputStream() - \n"
                              "\t""At frame idx = "fmt_ui32", the total "
                              "allocated size of all _incompleted_ and \n"
                              "\t""_completed_ intervals is too big to fit "
                              "in the frame, and this may \n"
                              "\t""causes problem in the 2nd pass\n",
                              ( slog->frame_dir ).Nframe );
            fprintf( errfile, "\t""Nbytes_in_bbufs_patched ("fmt_ui32") > "
                              "frame_buffer_size("fmt_ui32")\n",
                              Nbytes_in_bbufs_patched,
                              fbuf_bufsz( slog->fbuf ) );
            fflush( errfile );
        }
    }

    /*
        since the new buffer size is NOT too big for the buffer 
        Add/Copy the new interval record to the SLOG Current buffer 
    */
    ierr = SLOG_Bbuf_AddCopiedIntvlRec( slog->cur_bbuf, irec );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_Irec_ToOutputStream() - \n"
                          "\t""SLOG_Bbuf_AddCopiedIntvlRec() fails !!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*
        Update the record definition table to indicate which definition
        has been used.
    */
#if defined( COMPRESSION )
    recdef = SLOG_RDEF_GetRecDef( slog->rec_defs, irec->intvltype,
                                  irec->bebits[0], irec->bebits[1] );
    if ( recdef == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Irec_ToOutputStream() - \n"
                          "\t""SLOG_RecDef_SetUsed ("fmt_itype_t","
                          fmt_bebit_t","fmt_bebit_t") fails!\n",
                          irec->intvltype, irec->bebits[0], irec->bebits[1] );
        fflush( errfile );
        return SLOG_FAIL;
    }
    SLOG_RecDef_SetUsed( recdef, SLOG_TRUE );

    intvlinfo = SLOG_PROF_GetIntvlInfo( slog->prof, irec->intvltype );
    if ( intvlinfo == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Irec_ToOutputStream() - Warning !!!\n"
                          "\t""SLOG_IntvlInfo_SetUsed ("fmt_itype_t") fails!\n"
                          "\t""Program continues!",
                          irec->intvltype );
        fflush( errfile );
        /*  return SLOG_FAIL;  */
    }
    SLOG_IntvlInfo_SetUsed( intvlinfo, SLOG_TRUE );
#endif

    /*
        Check if there is any frame space reservation inconsistency
    */
    if ( slog->reserve != NULL ) {
        slog->reserve->Nrec_eff --;
        if ( slog->reserve->Nrec_eff < 0 ) {
            fprintf( errfile, __FILE__"SLOG_Irec_ToOutputStream() - \n"
                              "\t""Number of outstanding interval records = "
                              fmt_i32"\n", slog->reserve->Nrec_eff );
            fflush( errfile );
            return SLOG_FAIL;
        }

        ierr = SLOG_RecDef_SizeOfIrecInFile( slog->rec_defs,
                                             irec->rectype, irec->intvltype,
                                             irec->bebits[0], irec->bebits[1] );
        if ( ierr == SLOG_FAIL ) {
            fprintf( errfile, __FILE__"SLOG_Irec_ToOutputStream() - \n"
                              "\t""SLOG_Irec_SizeOf("fmt_rtype_t","fmt_itype_t
                              ","fmt_bebit_t","fmt_bebit_t") fails!\n",
                              irec->rectype, irec->intvltype,
                              irec->bebits[0], irec->bebits[1] );
            fflush( errfile );
            return SLOG_FAIL;
        }
        irec_bytesize_estimate = ierr;

        if ( irec_bytesize_estimate < irec->bytesize ) {
            fprintf( errfile, __FILE__"SLOG_Irec_ToOutputStream() - \n"
                              "\t""SLOG_Irec_SizeOf("fmt_rtype_t","fmt_itype_t
                              ","fmt_bebit_t","fmt_bebit_t") returns %d bytes "
                              "< the actual irec->bytesize, "fmt_sz_t" bytes\n",
                              irec->rectype, irec->intvltype,
                              irec->bebits[0], irec->bebits[1],
                              irec_bytesize_estimate, irec->bytesize );
            fprintf( errfile, "\t""Possible space reservation "
                              "inconsistency!\n" ); 
            fflush( errfile );
            return SLOG_FAIL;
        }
        slog->reserve->Nbytes_eff -= irec_bytesize_estimate;

        if ( slog->reserve->Nbytes_eff < 0 ) {
            fprintf( errfile, __FILE__"SLOG_Irec_ToOutputStream() - \n"
                              "\t""Byte size of all outstanding intervals, "
                              "slog->reserve->Nbytes_eff = "fmt_i32"\n",
                              slog->reserve->Nbytes_eff );
            fflush( errfile );
            return SLOG_FAIL;
        }
    }   /*  Endif ( slog->reserve != NULL )  */

    return SLOG_SUCCESS;
}



/*@C
    SLOG_CloseOutputStream - Close the specified SLOG_STREAM. It frees 
                             all the internally allocated memory used 
                             in SLOG_STREAM

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM to be closed.

  Modifies Output Variables :
. returned value - integer status code

  Include File Needed :
  slog.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_CloseOutputStream( SLOG_STREAM *slog )
{
    SLOG_uint32 Nbytes_written;
    SLOG_uint32 count_irec_in_bbufs;
    int         IsLastFrame = SLOG_TRUE;
    int         ierr;

#if defined( DEBUG )
    SLOG_uint32     old_buf_Nbytes_in_file;
    SLOG_uint32     Nbytes_in_bbufs_patched;
#endif

    if ( slog->HasIrec2IOStreamBeenUsed == SLOG_TRUE ) {

#if defined( DEBUG )
        old_buf_Nbytes_in_file = SLOG_typesz[ FrameHdr ]
                               + slog->hdr->frame_reserved_size
                               + ( slog->cur_bbuf )->Nbytes_in_file
                               + ( slog->tmp_bbuf )->Nbytes_in_file;
        if ( slog->reserve != NULL ) 
            old_buf_Nbytes_in_file += slog->reserve->Nbytes_eff;
        fprintf( stdout, __FILE__":SLOG_CloseOutputStream() - "
                 "Starting SLOG_STM_UpdateFrameDirEntry_Forward()\n" );
        fprintf( stdout, "\t""FrameHdr + sum_reserved_space "
                         "+ _cur_ + _tmp_ = "fmt_ui32" bytes\n",
                         old_buf_Nbytes_in_file );
        fflush( stdout );
#endif

#if defined( DEBUG )
        if ( slog->reserve != NULL ) {
            slog->reserve->Nbytes_tot -= slog->cur_bbuf->Nbytes_in_file;
            slog->reserve->Nrec_tot   -= slog->cur_bbuf->count_irec;
        }
        if ( slog->reserve != NULL ) {
            Nbytes_in_bbufs_patched = SLOG_typesz[ FrameHdr ]
                                    + slog->hdr->frame_reserved_size
                                    + slog->reserve->Nbytes_tot;
    
            if ( Nbytes_in_bbufs_patched > fbuf_bufsz( slog->fbuf ) ) {
                fprintf( errfile, __FILE__":SLOG_Irec_ToOutputStream() - \n"
                                  "\t""At frame idx = "fmt_ui32", the total "
                                  "allocated size of all _incompleted_ and \n"
                                  "\t""_completed_ intervals is too big to fit "
                                  "in the frame, and this may \n"
                                  "\t""causes problem in the 2nd pass\n",
                                  ( slog->frame_dir ).Nframe );
                fprintf( errfile, "\t""Nbytes_in_bbufs_patched ("fmt_ui32") > "
                                  "frame buffer size("fmt_ui32")\n",
                                  Nbytes_in_bbufs_patched,
                                  fbuf_bufsz( slog->fbuf ) );
                fflush( errfile );
            }
        }
        if ( slog->reserve != NULL ) {
            fprintf( stdout, "\t""reserve->Nbytes_eff = "fmt_i32", "
                             "reserve->Nbytes_tot = "fmt_i32"\n",
                              slog->reserve->Nbytes_eff,
                              slog->reserve->Nbytes_tot );
        }
#endif

        count_irec_in_bbufs = ( slog->cur_bbuf )->count_irec
                            + ( slog->tmp_bbuf )->count_irec;

        if ( count_irec_in_bbufs > 0 ) { 
            ierr = SLOG_STM_UpdateFrameDirEntry_Forward( slog, IsLastFrame );
            if ( ierr != SLOG_SUCCESS ) {
                fprintf( errfile, __FILE__":SLOG_CloseOutputStream() - "
                                  "SLOG_STM_UpdateFrameDirEntry_Forward() "
                                  "fails!!\n" );
                fflush( errfile );
                return SLOG_FAIL;
            }

            ierr = SLOG_STM_UpdateFRAME_Forward( slog );
            if ( ierr != SLOG_SUCCESS ) {
                fprintf( errfile, __FILE__":SLOG_CloseOutputStream() - "
                                  "SLOG_STM_UpdateFRAME_Forward( slog ) "
                                  "fails!!\n" );
                fflush( errfile );
                return SLOG_FAIL;
            }

            ierr = SLOG_STM_WriteFRAME( slog );
            if ( ierr == SLOG_FAIL ) {
                fprintf( errfile, __FILE__":SLOG_CloseOutputStream() - "
                                  "SLOG_STM_WriteFRAME( slog ) fails!!\n" );
                fflush( errfile );
                return SLOG_FAIL;
            }
            Nbytes_written = ierr;
        
            ierr = SLOG_STM_UpdateWriteFRAMEDIR( slog );
            if ( ierr != SLOG_SUCCESS ) {
                fprintf( errfile, __FILE__":SLOG_CloseOutputStream() - "
                         "SLOG_STM_UpdateWriteFRAMEDIR( slog ) fails!!\n" );
                fflush( errfile );
                return SLOG_FAIL;
            }

            /*  SLOG_STM_InitAgainBeforeAddRec( slog );  */
        }   /*  Endof    if ( count_irec_in_bbufs > 0 )  */

        /*
            Compress the various file headers
        */
#if defined( COMPRESSION )
        ierr = SLOG_RDEF_CompressTable( slog->rec_defs );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_CloseOutputStream() - "
                              "SLOG_RDEF_CompressTable() fails\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }
        ierr = SLOG_RDEF_CompressedTableToFile( slog );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_CloseOutputStream() - "
                              "SLOG_RDEF_CompressdTableToFile() fails\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }

        ierr = SLOG_PROF_CompressTable( slog->prof );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_CloseOutputStream() - "
                              "SLOG_PROF_CompressTable() fails\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }
        ierr = SLOG_PROF_CompressedTableToFile( slog );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_CloseOutputStream() - "
                              "SLOG_PROF_CompressdTableToFile() fails\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }
#endif

        /*  Initialize SLOG's Preview Statistics Array Table  */
        ierr = SLOG_PSTAT_Init( slog );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_CloseOutputStream() - "
                              "SLOG_PSTAT_Init() fails\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }

        /*
            Process the logfile backward in time to patch the pseudo record
            holes created during the 1st forward pass of the logfile 
        */
        if ( slog->hdr->IsIncreasingEndtime == SLOG_TRUE ) {
            ierr = SLOG_PSTAT_SetBinWidthInTime( slog );
            if ( ierr != SLOG_SUCCESS ) {
                fprintf( errfile, __FILE__":SLOG_CloseOutputStream() - "
                                  "SLOG_PSTAT_SetBinWidthInTime() fails\n" );
                fflush( errfile );
                return SLOG_FAIL;
            }

            ierr = SLOG_STM_2ndPass( slog );
            if ( ierr != SLOG_SUCCESS ) {
                fprintf( errfile, __FILE__":SLOG_CloseOutputStream() - "
                                  "SLOG_STM_2ndPass() fails\n" );
                fflush( errfile );
                return SLOG_FAIL;
            }
        }   /*  Endof  if ( slog->hdr->IsIncreasingEndtime == SLOG_TRUE )  */

        /*
            Check if all the pseudo record holes has been patched.
            i.e. check if the SLOG_Irec_ReserveSpace() and
            SLOG_Irec_ToOutputStream() are called in pair for
            each interval record created
        */
        if ( slog->reserve != NULL ) {
            if ( slog->reserve->Nrec_eff != 0 ) {
                fprintf( errfile, __FILE__"SLOG_CloseOutputStream() - "
                                  "Warning !!!!\n"
                                  "\t""Number of outstanding intervals  = "
                                  fmt_i32"\n", slog->reserve->Nrec_eff );
                fflush( errfile );
            }
            if ( slog->reserve->Nbytes_eff != 0 ) {
                fprintf( errfile, __FILE__"SLOG_CloseOutputStream() - "
                                  "Warning !!!!\n"
                                  "\t""Bytesize of all outstanding intervals, "
                                  "slog->reserve->Nbytes_eff = "
                                   fmt_i32"\n",
                                   slog->reserve->Nbytes_eff );
                fflush( errfile );
            }
        }   /*  Endif ( slog->reserve != NULL )  */

        /*  Write the updated statistics info onto the disk again  */
        ierr = SLOG_PSTAT_Close( slog );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_CloseOutputStream() - "
                              "SLOG_PSTAT_Close( slog ) fails!!\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }

    }   /*  Endof  if ( slog->HasIrec2IOStreamBeenUsed == SLOG_TRUE )  */

    ierr = SLOG_CloseStream( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_CloseOutputStream() - "
                          "SLOG_CloseStream( slog ) fails!!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

#if defined( DEBUG )
fprintf( stdout, __FILE__":SLOG_CloseOutputStream() - End\n" );
fflush( stdout );
#endif

    return SLOG_SUCCESS;
}



int SLOG_STM_2ndPass( SLOG_STREAM *slog )
{
    const SLOG_uint32 zero        = (SLOG_uint32) 0;
          SLOG_uint32 Nbytes_read;
          SLOG_uint32 Nbytes_changed;
          SLOG_uint32 Nbytes_written;
          int         idx;
          int         ierr;
    
    fflush( slog->fd );
    /*  Reinitialize the Frame Header for next frame buffer  */
    SLOG_STM_UpdateFrameHdrGiven( slog, zero, zero, zero, zero,
                                        zero, zero, zero, zero );
    /*  Free all the nodes in all the bbufs  */
    SLOG_Bbuf_DelAllNodes( slog->cur_bbuf );
    SLOG_Bbuf_DelAllNodes( slog->inc_bbuf );
    SLOG_Bbuf_DelAllNodes( slog->pas_bbuf );
    SLOG_Bbuf_DelAllNodes( slog->out_bbuf );
    SLOG_Bbuf_DelAllNodes( slog->tmp_bbuf );
    /*  Reinitialize the bi-directional list buffers & related variables  */
    SLOG_Bbuf_Init( slog->cur_bbuf );
    SLOG_Bbuf_Init( slog->inc_bbuf );
    SLOG_Bbuf_Init( slog->pas_bbuf );
    SLOG_Bbuf_Init( slog->out_bbuf );
    SLOG_Bbuf_Init( slog->tmp_bbuf );

    /*  Reopen slog->fbuf  */
    fbuf_free( slog->fbuf );
    slog->fbuf = fbuf_create( slog->hdr->frame_bytesize );
    if ( slog->fbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_STM_2ndPass() - "
                          "fbuf_create( %d bytes ) fails\n",
                          slog->hdr->frame_bytesize );
        fflush( errfile );
        SLOG_CloseStream( slog );
        return SLOG_FAIL;
    }

    /*  Set the file buffer's file descriptor equal to the SLOG_STREAM's  */
    fbuf_filedesc( slog->fbuf, slog->fd );

    for ( idx = ( slog->frame_dir ).Nframe-1; idx >= 0; idx-- ) {
        ierr = SLOG_STM_ReadFRAMEatIdx( slog, idx );
        if ( ierr == SLOG_FAIL || ierr == SLOG_EOF ) {
            fprintf( errfile, __FILE__":SLOG_STM_2ndPass() - "
                              "Unexpected termination of "
                              "SLOG_STM_ReadFRAMEatIdx(%d)\n", idx );
            fflush( errfile );
            return SLOG_FAIL;
        }
        Nbytes_read = ierr;

        ierr = SLOG_STM_UpdateFrameDirEntry_Backward( slog, idx );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_STM_2ndPass() - "
                     "SLOG_STM_UpdateFrameDirEntry_Backward(%d) fails\n",
                     idx );
            fflush( errfile );
            return SLOG_FAIL;
        }

        ierr = SLOG_STM_UpdateFRAME_Backward( slog );
        if ( ierr == SLOG_FAIL ) {
            fprintf( errfile, __FILE__":SLOG_STM_2ndPass() - "
                              "SLOG_STM_UpdateFRAME_Backward(%d) fails\n",
                              idx );
            fflush( errfile );
            return SLOG_FAIL;
        }
        Nbytes_changed = ierr;

        if ( Nbytes_read + Nbytes_changed > slog->hdr->frame_bytesize ) {
            fprintf( errfile, __FILE__":SLOG_STM_2ndPass() - "
                              "At frame index = %d :\n", idx );
            fprintf( errfile, "\t""The patched Bbufs are too big to fit "
                              "into a frame buffer\n"
                              "\t""Nbytes_read("fmt_ui32") + "
                              "Nbytes_changed("fmt_ui32") > "
                              "frame_bytesize("fmt_ui32")\n",
                              Nbytes_read, Nbytes_changed,
                              slog->hdr->frame_bytesize );
            fprintf( errfile, "\t""Check the printout of the logfile at "
                              "frame index = %d and %d or maybe earlier ones\n",
                              idx, idx+1 );
            fflush( errfile );
            return SLOG_FAIL;
        }

        fbuf_ptr2head( slog->fbuf );
        ierr = SLOG_STM_WriteFRAMEatIdx( slog, idx );
        if ( ierr == SLOG_FAIL || ierr == SLOG_EOF ) {
            fprintf( errfile, __FILE__":SLOG_STM_2ndPass() - "
                              "SLOG_STM_WriteFRAMEatIdx(%d) fails\n", idx );
            fflush( errfile );
            return SLOG_FAIL;
        }
        Nbytes_written = ierr;


        /*
            Since the irec's are arranged in increasing order of endtime
            stamps, only cur_bbuf and inc_bbuf contain REAL records.
            So statistics are collected in these 2 Bbufs.
        */
        ierr = SLOG_PSTAT_Update( slog->pstat, slog->cur_bbuf );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_STM_2ndPass() - "
                              "SLOG_PSTAT_Update( _CUR_ ) fails\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }
        ierr = SLOG_PSTAT_Update( slog->pstat, slog->inc_bbuf );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_STM_2ndPass() - "
                              "SLOG_PSTAT_Update( _INC_ ) fails\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }


        /*  For the 1st frame in the logfile  */
        if ( idx == 0 ) {
            /*
                Set slog->file_dir_cur pointing at the 1st entry 
                of FrameDirEntry
            */
            slog->file_dir_cur = slog->file_dir_hdr
                               + SLOG_typesz[ FrameDirHdr ];
            ierr = SLOG_WriteUpdatedFrameDirEntry( slog );
            if ( ierr != SLOG_SUCCESS ) {
                fprintf( errfile, __FILE__":SLOG_STM_2ndPass() - "
                                  "SLOG_WriteUpdatedFrameDirEntry( slog ) "
                                  "fails!!\n" );
                fflush( errfile );
                return SLOG_FAIL;
            }
        }
        else
            SLOG_STM_InitAgainBeforePatchFrame( slog );

    }   /*  for (  (slog->frame_dir).Nframe > idx >= 0 )  */

    return SLOG_SUCCESS;
}



/*@C
    SLOG_Irec_ReserveSpace - Reserve space in the current frame
                             for the current interval record identified
                             by record type ID and interval record type ID. 

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM where the interval record locates.

  Unmodified Input Variables :
. rectype - interval record's record field which determines if the
            record is a FIXED, VARIABLE or Message record, this in turns
            will determine the number of association and the number of 
            sets of MPI call arguments or the extra space needed for Message
            record.
. intvltype - interval record's interval type field.
. bebit_0 - interval record's first bebit.
. bebit_1 - interval record's second bebit.
. starttime - interval record's start time, used to check if the interval
              follows the time order rule set by 
              SLOG_SetIncreasingEndtimeOrder() or
              SLOG_SetIncreasingStarttimeOrder().  The checking is only
              done when user turns on the corresponding compiler
              flag.  If not, this input variable will not be used.

  Modified Output Variables :
. returned value - integer return status.

  Usage Notes on this subroutine :
    ***  IMPORTANT ***, this routine is totally optional.
    User doesn't have to use this routine to use the API.
    And this routine can only be called when the completed
    interval records are arranged in increasing endtime order.
    The subroutine allocates space in the current frame for the
    interval record.  Typically, the routine is called when user
    knows only __start_event__ of an interval record without the 
    knowledge of the __end_event__.  So the routine must be 
    called before calling SLOG_Irec_ToOutputStream().  For completed
    interval records which are arranged in increasing endtime stamp
    order, each interval record may be called with
    SLOG_Irec_ReserveSpace() first when the time of the corresponding
    __start_event__ is known and called with SLOG_Irec_ToOutputStream()
    when the time of the corresponding __end_event__ is known.  Also
    the routine will check if the starttime supplied is called in
    increasing time order which is a sufficient condition to guarantee
    that space reservation for every pseudo record in each frame. 

  Include File Needed :
  slog.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_Irec_ReserveSpace(       SLOG_STREAM      *slog,
                            const SLOG_rectype_t    rectype,
                            const SLOG_intvltype_t  intvltype,
                            const SLOG_bebit_t      bebit_0,
                            const SLOG_bebit_t      bebit_1,
                            const SLOG_starttime_t  starttime )
{
/*
    SLOG_uint32     old_buf_Nbytes_in_file;
    SLOG_uint32     new_buf_Nbytes_in_file;
*/
    SLOG_uint32     irec_bytesize_estimate;
    int             ierr;

    if ( slog->reserve == NULL ) {
        if ( slog->hdr->IsIncreasingEndtime != SLOG_TRUE ) {
            fprintf( errfile, __FILE__"SLOG_Irec_ReserveSpace() - \n"
                              "SLOG_SetIncreasingEndtimeOrder() has NOT been "
                              "called!\n" );
            fflush( errfile );
            return SLOG_SUCCESS;
        }

        slog->hdr->HasReserveSpaceBeenUsed = SLOG_TRUE;
        ierr = SLOG_WriteUpdatedHeader( slog );
        if ( ierr != SLOG_SUCCESS ) {
            fprintf( errfile,  __FILE__":SLOG_Irec_ReserveSpace() - "
                               "SLOG_WriteUpdatedHeader() fails\n" );
            fflush( errfile );
            return ierr;
        }

        /*
            Allocate SLOG_reserve_info_t which stores all the bookkeeping
            info about status of space utilization of reserved Irec in a frame.
        */
        slog->reserve = ( SLOG_reserve_info_t * ) 
                         malloc( sizeof ( SLOG_reserve_info_t ) );
        if ( slog->reserve == NULL ) {
            fprintf( errfile, __FILE__":SLOG_Irec_ReserveSpace() - "
                              "malloc() for SLOG_reserve_info_t fails\n" );
            fflush( errfile );
            return ierr;
        }

        /*
            Initialize the SLOG_reserve_info_t's component

            Nrec_eff   : effective No of Irec reserved in a frame.
                         This number is decremented by 1 for each
                         SLOG_Irec_ToOutputStream() call.  But increment
                         by 1 as SLOG_Irec_ReserveSpace() is called.
            Nbytes_eff : companion variable to store bytesize of Nrec_eff
            Nrec_tot   : Total No. of Irec reserved in a frame.
                         This number is incremented by 1 for each
                         SLOG_Irec_ReserveSpace() call.  And it is 
                         reinitialized to zero after the frame Bbufs are
                         dumped to the disk( or when new frame bbufs are
                         allocated )
            Nbytes_tot : companion variable to store bytesize of Nrec_tot
        */
        slog->reserve->Nrec_eff       = 0;
        slog->reserve->Nbytes_eff     = 0;
        slog->reserve->Nrec_tot       = 0;
        slog->reserve->Nbytes_tot     = 0;

#if defined( CHECKTIMEORDER )
        slog->prev_starttime = starttime;
#endif
    }

    ierr = SLOG_RecDef_SizeOfIrecInFile( slog->rec_defs,
                                         rectype, intvltype,
                                         bebit_0, bebit_1 );
    if ( ierr == SLOG_FAIL ) {
        fprintf( errfile, __FILE__"SLOG_Irec_ReserveSpace() - \n"
                          "\t""SLOG_Irec_SizeOf("fmt_rtype_t","fmt_itype_t
                          ","fmt_bebit_t","fmt_bebit_t") fails!\n",
                          rectype, intvltype, bebit_0, bebit_1 );
        fflush( errfile );
        return SLOG_FAIL;
    }
    irec_bytesize_estimate = ierr;

    /*  Update the content in SLOG_reserve_info_t  */
    slog->reserve->Nrec_eff++;
    slog->reserve->Nbytes_eff += irec_bytesize_estimate;
    slog->reserve->Nrec_tot++;
    slog->reserve->Nbytes_tot += irec_bytesize_estimate;

/*
    if ( slog->HasIrec2IOStreamBeenUsed == SLOG_TRUE ) {
        old_buf_Nbytes_in_file = SLOG_typesz[ FrameHdr ]
                               + slog->hdr->frame_reserved_size
                               + slog->reserve->Nbytes_tot;
        if ( old_buf_Nbytes_in_file > fbuf_bufsz( slog->fbuf ) ) {
            fprintf( errfile, __FILE__"SLOG_Irec_ReserveSpace() - Warning!!!\n"
                              "\t""When SLOG_Irec_ToOutputStream( "
                              fmt_rtype_t", " fmt_itype_t", "fmt_bebit_t", "
                              fmt_bebit_t", " fmt_stime_t" ) \n"
                              "\t""is called for the corresponding "
                              "_End_Event_, it may fail!!\n",  rectype,
                              intvltype, bebit_0, bebit_1, starttime );
            fprintf( errfile, "\t""reserve->Nbytes_tot = "fmt_i32", "
                              "old_buf_Nbytes_in_file = "fmt_ui32", "
                              "frame buffer size = "fmt_ui32"\n",
                              slog->reserve->Nbytes_tot,
                              old_buf_Nbytes_in_file,
                              fbuf_bufsz( slog->fbuf ) );
            fflush( errfile );
        }
    }   
*/
        /*  Endif ( slog->HasIrec2IOStreamBeenUsed == SLOG_TRUE )  */

#if defined( CHECKTIMEORDER )
    /*
        Check if the starttime supplied in this call follows the 
        increasing starttime order of the previous call.
    */
    if ( starttime < slog->prev_starttime ) {
        fprintf( errfile, __FILE__"SLOG_Irec_ReserveSpace() - Warning!!!\n"
                          "\t""Current starttime("fmt_time") is earlier "
                          "than previous starttime("fmt_time")\n",
                          starttime, slog->prev_starttime );
        fflush( errfile );
    }
    slog->prev_starttime = starttime;
#endif

    return SLOG_SUCCESS;
}



/*@C
    SLOG_Irec_SetMinRec - Set the fields for a bare minimal Interval Record

  Modified Input Variables :
. intvlrec - pointer to the internal interval record to be modified.

  Unmodified Input Variables :
. rectype - interval record's record field which determines if the
            record is a FIXED record or VARIABLE record, this in turns
            will determine the number of association and the number of 
            sets of MPI call arguments. 
. intvltype - interval record's interval type field.
. bebit_0 - interval record's first bebit.
. bebit_1 - interval record's second bebit.
. starttime - interval record's start time.
. duration - interval record's life time.
. orig_node_id - interval record's origin node ID.
. orig_cpu_id - interval record's origin cpu ID.
. orig_thread_id - interval record's origin thread ID.
. where - interval record's where integer pointer.
. ...   - destination task labels for Message Record ( optional ) :
  dest_node_id - interval record's destination node ID.
  dest_cpu_id - interval record's destination cpu ID.
  dest_thread_id - interval record's destination thread ID.


  Modified Output Variables :
. returned value - integer return status.

  Usage Notes on this subroutine :
    The subroutine initializes an allocated internal interval
    record data structure.   So the routine must be called before
    appending the record to the SLOG file.   The rectype may be
    a Message Record type.

  Include File Needed :
  slog.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_Irec_SetMinRec(       SLOG_intvlrec_t  *intvlrec,
                         const SLOG_rectype_t    rectype,
                         const SLOG_intvltype_t  intvltype,
                         const SLOG_bebit_t      bebit_0,
                         const SLOG_bebit_t      bebit_1,
                         const SLOG_starttime_t  starttime,
                         const SLOG_duration_t   duration,
                         const SLOG_nodeID_t     orig_node_id,
                         const SLOG_cpuID_t      orig_cpu_id,
                         const SLOG_threadID_t   orig_thread_id,
                         const SLOG_where_t      where,
                         ... )
{
    /*  Local Variables  */
    va_list           ap;
    SLOG_nodeID_t     dest_node_id;
    SLOG_cpuID_t      dest_cpu_id;
    SLOG_threadID_t   dest_thread_id;

    if ( intvlrec == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Irec_SetMinRec() - "
                          "Destination interval record pointer is NULL/n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Assemble all the input arguments as a SLOG_intvlrec_t, intvlrec  */
    intvlrec->rectype   = rectype  ;
    intvlrec->intvltype = intvltype;
    intvlrec->bebits[0] = bebit_0  ;
    intvlrec->bebits[1] = bebit_1  ;
    intvlrec->starttime = starttime;
    intvlrec->duration  = duration ;
    SLOG_TaskID_Assign( &( intvlrec->origID ),
                        orig_node_id, orig_cpu_id, orig_thread_id );
    intvlrec->where     = where    ;

    intvlrec->bytesize  = SLOG_typesz[ min_IntvlRec ];

    if ( SLOG_global_IsOffDiagRec( intvlrec->rectype ) ) {
        va_start( ap, where );
        dest_node_id   = va_arg( ap, SLOG_nodeID_t );
        dest_cpu_id    = va_arg( ap, SLOG_cpuID_t );
        dest_thread_id = va_arg( ap, SLOG_threadID_t );
        va_end( ap );
        SLOG_TaskID_Assign( &( intvlrec->destID ),
                            dest_node_id, dest_cpu_id, dest_thread_id );
        intvlrec->bytesize += SLOG_typesz[ taskID_t ];
    }

    return SLOG_SUCCESS;
}



/*@C
    SLOG_Irec_SetDestTaskLabel - Set the Destination Task IDs which are 
                                 needed for a Message Record, i.e. Arrow.

  Modified Input Variables :
. intvlrec - pointer to the internal Interval record to be modified.

  Unmodified Input Variables :
. node_id - interval record's extra node ID.
. cpu_id - interval record's extra cpu ID.
. thread_id - interval record's extra thread ID.


  Modified Output Variables :
. returned value - integer return status.

  Usage Notes on this subroutine :
    The subroutine appends the specified Internal Interval Record with 
    the extra set of Task Labels.  This subroutine is meant to be called
    once for each "Message" or "Arrow" interval record written to the 
    slogfile.

  Include File Needed :
  slog.h

@*/
int SLOG_Irec_SetDestTaskLabel(       SLOG_intvlrec_t  *intvlrec,
                                const SLOG_nodeID_t     node_id,
                                const SLOG_cpuID_t      cpu_id,
                                const SLOG_threadID_t   thread_id )
{
    if ( intvlrec->bytesize == 0 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_SetDestTaskLabel() - "
                          "SLOG_Irec_SetMinRec() needs to be called first\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( ! SLOG_global_IsOffDiagRec( intvlrec->rectype ) ) {
        fprintf( errfile, __FILE__":SLOG_Irec_SetDestTaskLabel() - \n"
                          "\t""the associated interval record is NOT of "
                          " Message Record type, it has rectype = "
                          fmt_rtype_t"\n", intvlrec->rectype );
        fprintf( errfile, "\t""This function should NOT have been called\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    SLOG_TaskID_Assign( &( intvlrec->destID ), node_id, cpu_id, thread_id );
    intvlrec->bytesize += SLOG_typesz[ taskID_t ];

    return SLOG_SUCCESS;
}



/*@C
    SLOG_Irec_SetAssocs - Set the association array of the interval record.

  Modified Input Variables :
. intvlrec - pointer to the internal Interval record to be modified.

  Unmodified Input Variables :
. slog  - pointer to the SLOG_STREAM where the Record Definition Table is 
          defined. 
. N_assocs - Number of elements in the association array.  This number will
             be checked to determine if it coincides with what is in
             Record Definition Table
. assocs - association array, array of file pointers to the associated 
           interval records of the specified interval record.


  Modified Output Variables :
. returned value - integer return status.

  Usage Notes on this subroutine :
    The subroutine appends the specified Internal Interval Record with 
    the given association array.  This subroutine is meant to be called
    once for each interval record written to the slogfile.

  Include File Needed :
  slog.h

@*/
int SLOG_Irec_SetAssocs(       SLOG_intvlrec_t  *intvlrec,
                         const SLOG_STREAM      *slog,
                         const SLOG_N_assocs_t   N_assocs,
                         const SLOG_assoc_t     *assocs  )
{
    SLOG_N_assocs_t recdefs_Nassocs;
    int             sizeof_assocs;
    int             ii;

    if ( intvlrec->bytesize == 0 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_SetAssocs() - "
                          "SLOG_Irec_SetMinRec() needs to be called first\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( ! SLOG_global_IsVarRec( intvlrec->rectype ) ) {
        recdefs_Nassocs = SLOG_RecDef_NumOfAssocs( slog->rec_defs,
                                                   intvlrec->intvltype,
                                                   intvlrec->bebits[0],
                                                   intvlrec->bebits[1] );
        if ( N_assocs != recdefs_Nassocs ) {
            fprintf( errfile, __FILE__":SLOG_Irec_SetAssocs() - "
                             "Input Number of association does NOT match "
                             "the entry in Record Definition Table\n" );
            fprintf( errfile, "       ""The Input Number of associations = "
                              fmt_Nassocs_t"\n", N_assocs );
            fprintf( errfile, "       ""SLOG_RecDef_NumOfAssocs() returns "
                              fmt_Nassocs_t"\n", recdefs_Nassocs );
            fflush( errfile );
            return SLOG_FAIL;
        }
    }

  /*
      Read in the MPI call's corresponding association list 
      as determined by input intvltype
  */
    intvlrec->N_assocs = N_assocs;
    sizeof_assocs = intvlrec->N_assocs * sizeof( SLOG_assoc_t );
    if ( intvlrec->N_assocs > 0 ) {
        intvlrec->assocs = ( SLOG_assoc_t * ) malloc( sizeof_assocs );
        if ( intvlrec->N_assocs > 0 && intvlrec->assocs == NULL ) {
            fprintf( errfile, __FILE__":SLOG_Irec_SetAssocs() - "
                              "malloc() for intvlrec->assocs fails\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }

        for ( ii = 0; ii < N_assocs; ii++ ) 
            intvlrec->assocs[ ii ] = assocs[ ii ];

        intvlrec->bytesize += sizeof_assocs;
    }
    
    return SLOG_SUCCESS;
}



/*@C
    SLOG_Irec_SetArgs - Set the argument part of the interval record.

  Modified Input Variables :
. intvlrec - pointer to the internal interval record to be modified.

  Unmodified Input Variables :
. slog  - pointer to the SLOG_STREAM where the Record Definition Table 
          is defined.
. N_args - Number of elements in the argument array.  This number will
           be checked to determine if it coincides with what is in
           Record Definition Table
. args - argument array, array of arguments of the specified interval record.


  Modified Output Variables :
. returned value - integer return status.

  Usage Notes on this subroutine :
    The subroutine appends the specified Internal Interval with argument
    array.  For Fixed interval record, the subroutine is meant to called
    once for each specified interval record.  For variable interval 
    record, the subroutine can be called as many times as needed.

  Include File Needed :
  slog.h

@*/
int SLOG_Irec_SetArgs(       SLOG_intvlrec_t  *intvlrec,
                       const SLOG_STREAM      *slog,
                       const SLOG_N_args_t     N_args,
                       const SLOG_arg_t       *args )
{
    SLOG_vtrarg_t *vtr_args;
    SLOG_N_args_t  recdefs_Nargs;

    if ( SLOG_Irec_IsEmpty( intvlrec ) ) {
        fprintf( errfile, __FILE__":SLOG_Irec_SetArgs() - "
                          "SLOG_Irec_SetMinRec() needs to be called first\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( ! SLOG_global_IsVarRec( intvlrec->rectype ) ) {
        recdefs_Nargs = SLOG_RecDef_NumOfArgs( slog->rec_defs,
                                               intvlrec->intvltype,
                                               intvlrec->bebits[0],
                                               intvlrec->bebits[1] );
        if ( N_args != recdefs_Nargs ) {
            fprintf( errfile, __FILE__":SLOG_Irec_SetArgs() - "
                              "Input Number of arguments does NOT match "
                              "the entry in Record Definition Table\n" );
            fprintf( errfile, "       ""The Input Number of arguments = "
                              fmt_Nargs_t"\n", N_args );
            fprintf( errfile, "       ""SLOG_RecDef_NumOfArgs() returns "
                              fmt_Nargs_t"\n", recdefs_Nargs );
            fflush( errfile );
            return SLOG_FAIL;
        }
    }

    vtr_args = SLOG_Varg_Create( N_args, args );
    if ( SLOG_Irec_AddVtrArgs( intvlrec, vtr_args ) != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_Irec_SetArgs() - "
                          "SLOG_Irec_AddVtrArgs() fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( SLOG_global_IsVarRec( intvlrec->rectype ) )
        intvlrec->bytesize += sizeof( SLOG_N_args_t );
    intvlrec->bytesize += N_args * sizeof( SLOG_arg_t );

    return SLOG_SUCCESS;
}
