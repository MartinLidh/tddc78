#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "slog_fileio.h"
#include "slog_bbuf.h"
#include "slog_header.h"
#include "slog_preview.h"
#include "slog_pstat.h"
#include "slog_ttable.h"
#include "slog_profile.h"
#include "slog_recdefs.h"
#include "slog_impl.h"               /*I  "slog_impl.h"  I*/

/*@C

    SLOG_OpenInputStream - Returns Input SLOG_STREAM associcated with
                           the given filename.

  Unmodified Input Variables :
. filename - constant char pointer to the filename.

  Modified Output Variables :
. returned value - pointer to newly allocated SLOG_STREAM.
                   it is NULL when the routine encounters error.

  Include File Needed :
  slog.h

.N SLOG_RETURN_STATUS
@*/
SLOG_STREAM *SLOG_OpenInputStream( const char *filename )
{
    SLOG_STREAM  *slog;


    SLOG_InitGlobalData();

    /*  memory for slog is allocated by SLOG_OpenStream()  */
    if ( ( slog = SLOG_OpenStream( filename, "rb" ) ) == NULL )
        return NULL;

    if ( SLOG_ReadHeader( slog ) != SLOG_SUCCESS )
        return NULL;
    if ( SLOG_PSTAT_Read( slog ) != SLOG_SUCCESS )
        return NULL;
    if ( SLOG_PVIEW_Read( slog ) != SLOG_SUCCESS )
        return NULL;
    if ( SLOG_TTAB_ReadThreadInfos( slog ) != SLOG_SUCCESS )
        return NULL;
    if ( SLOG_PROF_ReadIntvlInfos( slog ) != SLOG_SUCCESS )
        return NULL;
    if ( SLOG_RDEF_ReadRecDefs( slog ) != SLOG_SUCCESS )
        return NULL;

    return slog;
}



/*@C
    SLOG_Irec_FromInputStream - Return a pointer to the next interval 
                                record in the given SLOG_STREAM.
                                The interval record returned can be real
                                or pseudo record.

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM where interval record is retrieved.
. irc  - pointer to the returned integer status code.

  Modified Output Variables :
. returned value - pointer to the next interval record of type 
                   SLOG_intvlrec_t in the frame buffer.  The "const" 
                   qualifier indicates that user should NOT modify the
                   content.  The pointer is NULL when the routine 
                   encounters error or end of file.


  Usage Notes on this subroutine :

    The routine returns a pointer to the interval record of the various
    Bbuf in current frame buffer in the SLOG_STREAM given.  The 
    routine does NOT create new node/irec.  It just simply returns a pointer
    to the existing one in the bi-directional linked list( an internal data
    structure ).  If one wants to modify the content of the interval record 
    without modification of the specified bi-directional linked node, one 
    should use SLOG_Irec_Copy() to copy the returned record content to a new
    record.  The routine also ADVANCES the internal pointer of the specified
    linked list buffer.  Also, the routine does NOT gurantee the memory
    associated with previously _returned_ interval record be retained in the
    next call of the routine.  So if one wants to keep interval record for
    future reference, one need to use SLOG_Irec_Copy() to copy
    the content of the interval record to another memory location __BEFORE__
    calling SLOG_Irec_FromInputStream() again.  The example program,
    slog_readtest.c shows the typical way of using this subroutine.

  Include File Needed :
  slog.h

.N SLOG_RETURN_STATUS
@*/
const SLOG_intvlrec_t *
SLOG_Irec_FromInputStream( SLOG_STREAM *slog, int *irc )
{
    const SLOG_intvlrec_t *src_irec;
    int                    ierr;


    if ( slog->HasIrec2IOStreamBeenUsed == SLOG_FALSE ) {
        /*  Make sure this block is executed once for each SLOG_STREAM  */
        slog->HasIrec2IOStreamBeenUsed = SLOG_TRUE; 
        if ( SLOG_STM_CreateBbufs( slog ) != SLOG_SUCCESS ) {
            *irc = SLOG_FAIL;
            return NULL;
        }

        /*  Temporary put slog_fseek( fptr2framedata ) here  */
        if ( slog->hdr->fptr2framedata == SLOG_fptr_NULL ) {
            fprintf( errfile, __FILE__":SLOG_Irec_FromInputStream() - \n" );
            fprintf( errfile, "\t""slog->hdr->fptr2framedata is NULL \n" );
            fprintf( errfile, "\t""SLOG_Irec_ToOutputStream() has NOT been "
                              "called\n" );
            fflush( errfile );
            *irc = SLOG_EOF;
            return NULL;
        }
        ierr = slog_fseek( slog->fd, slog->hdr->fptr2framedata, SEEK_SET );
        if ( ierr != 0 ) {
            fprintf( errfile, __FILE__":SLOG_Irec_FromInputStream() - "
                              "slog_fseek( fptr2framedata ) fails\n" );
            fflush( errfile );
            *irc = SLOG_FAIL;
            return NULL;
        }

        /*
           It is necessary to call SLOG_STM_Init() _BEFORE_ SLOG_ReadFrameDir().
           SLOG_STM_Init() initializes the Frame_Dir components and various
           Bbufs in the SLOG_STREAM.  Then SLOG_ReadFrameDir() can update
           Frame_Dir compoents afterward.
        */
        SLOG_STM_Init( slog );

        if ( SLOG_ReadFrameDir( slog ) != SLOG_SUCCESS )
            return NULL;
    }   /*  Endof { if ( slog->HasIrec2IOStreamBeenUsed == SLOG_FALSE ) }  */


    /*
        Since SLOG_Bbuf_GetIntvlRec() advances internal pointer at the end
        of the routine before returning.  In stead of checking
        (slog->cur_bbuf)->lptr == (slog->cur_bbuf)->ltail,  one checks for
        (slog->cur_bbuf)->lptr == NULL.
    */
    if (    SLOG_Bbuf_IsPointerAtNULL( slog->inc_bbuf ) 
         && SLOG_Bbuf_IsPointerAtNULL( slog->pas_bbuf )
         && SLOG_Bbuf_IsPointerAtNULL( slog->cur_bbuf )
         && SLOG_Bbuf_IsPointerAtNULL( slog->out_bbuf ) ) {

        /*
            Reinitialize all the Bbufs & cleanup memory used in Bbufs
            before Reading the frame buffer
        */
        SLOG_Bbuf_DelAllNodes( slog->cur_bbuf );
        SLOG_Bbuf_DelAllNodes( slog->inc_bbuf );
        SLOG_Bbuf_DelAllNodes( slog->pas_bbuf );
        SLOG_Bbuf_DelAllNodes( slog->out_bbuf );
        SLOG_Bbuf_Init( slog->cur_bbuf );
        SLOG_Bbuf_Init( slog->inc_bbuf );
        SLOG_Bbuf_Init( slog->pas_bbuf );
        SLOG_Bbuf_Init( slog->out_bbuf );

        ierr = SLOG_STM_ReadFRAME( slog );
        if ( ierr == SLOG_FAIL ) {
            fprintf( errfile, __FILE__":SLOG_Irec_FromInputStream() - "
                              "Unexpected termination of "
                              "SLOG_STM_ReadFRAME()\n" );
            fflush( errfile );
            *irc = SLOG_FAIL;
            return NULL;
        }
        if ( ierr == SLOG_EOF ) {
            *irc = SLOG_EOF;
            return NULL;
        }

        /*  Set the cur_bbuf's pointer to point at the head of the blist  */
        if (    SLOG_Bbuf_IsPointerAtNULL( slog->inc_bbuf )
             && SLOG_Bbuf_IsPointerAtNULL( slog->pas_bbuf )
             && SLOG_Bbuf_IsPointerAtNULL( slog->cur_bbuf )
             && SLOG_Bbuf_IsPointerAtNULL( slog->out_bbuf ) ) {
            SLOG_Bbuf_SetPointerAtHead( slog->inc_bbuf );
            SLOG_Bbuf_SetPointerAtHead( slog->pas_bbuf );
            SLOG_Bbuf_SetPointerAtHead( slog->cur_bbuf );
            SLOG_Bbuf_SetPointerAtHead( slog->out_bbuf );
        }

        slog->ptr2bbuf = slog->inc_bbuf;

    }  /*  endof if ( SLOG_Bbuf_IsPointerAtNULL( slog->ALL_bbufs ) )  */

    /*
        The above IF statement guarantees that there should exist
        at least one non-returned irec among all bbufs in the frame

        So the following WHILE statement tries to return a non-empty
        bbuf which still has one non-returned irec.
    */
    while ( SLOG_Bbuf_IsPointerAtNULL( slog->ptr2bbuf ) ) {
        if ( slog->ptr2bbuf == slog->inc_bbuf )
            slog->ptr2bbuf = slog->pas_bbuf;
        else if ( slog->ptr2bbuf == slog->pas_bbuf ) 
            slog->ptr2bbuf = slog->cur_bbuf;
        else if ( slog->ptr2bbuf == slog->cur_bbuf ) 
            slog->ptr2bbuf = slog->out_bbuf;
        else if ( slog->ptr2bbuf == slog->out_bbuf ) {
            fprintf( errfile, __FILE__":SLOG_Irec_FromInputStream() - "
                              "Unexpected NULL pointer at "
                              "SLOG_Bbuf_IsPointerAtNULL( _ptr_ )\n" );
            fflush( errfile );
            *irc = SLOG_FAIL;
            return NULL;
        }
    }

    /*
        An extra IF statement to guarantee to return any interval record
        ( pointer ) if there is any left in the SLOG_STREAM/disk.
    */
    if ( ! SLOG_Bbuf_IsPointerAtNULL( slog->ptr2bbuf ) ) {

        src_irec = SLOG_Bbuf_GetIntvlRec( slog->ptr2bbuf );
        if ( src_irec == NULL ) {
            fprintf( errfile, __FILE__":SLOG_Irec_FromInputStream() - "
                              "SLOG_Bbuf_GetIntvlRec() return FAIL\n" );
            fflush( errfile );
            *irc = SLOG_FAIL;
            return NULL;
        }
        *irc = SLOG_SUCCESS;
        return src_irec;

    }   /*  endof ( NOT IsSLOG_Bbuf_PointerAtNULL( slog->ptr2bbuf ) )  */

    fprintf( errfile, __FILE__":SLOG_Irec_FromInputStream() - "
                      "something goes wrong\n" );
    fflush( errfile );
    *irc = SLOG_FAIL;
    return NULL;
}



/*@C
    SLOG_CloseInputStream - Close the specified SLOG_STREAM.  It frees
                            all the internally allocated memory used in
                            SLOG_STREAM

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM to be closed.

  Modifies Output Variables :
. returned value - integer status code.

  Include File Needed :
  slog.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_CloseInputStream( SLOG_STREAM *slog )
{
    int         ierr;

    ierr = SLOG_CloseStream( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_CloseInputStream() - "
                          "SLOG_CloseStream( slog ) fails!!\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

#if defined( DEBUG )
fprintf( stdout, __FILE__":SLOG_CloseInputStream() - End\n" );
fflush( stdout );
#endif

    return SLOG_SUCCESS;
}



/*@C
    SLOG_Irec_GetMinRec - Retrieve the fields for a bare minimal Interval Record

  Unmodified Input Variables :
. intvlrec - pointer to the internal Interval record to be retrieved.

  Modified Output Variables :
. rectype - pointer to the interval record's record field which determines 
            if the record is a FIXED record or VARIABLE record, this in turns
            will determine the number of association and the number of
            sets of MPI call arguments.
. intvltype - pointer to interval record's interval type field.
. bebit_0 - pointer to interval record's first bebit.
. bebit_1 - pointer to interval record's second bebit.
. starttime - pointer to interval record's start time.
. duration - pointer to interval record's life time.
. orig_node_id - pointer to interval record's origin node ID.
. orig_cpu_id - pointer to interval record's origin cpu ID.
. orig_thread_id - pointer to interval record's origin thread ID.
. orig_where - pointer to interval record's where integer pointer.
. ...   - pointers to destination task labels for Message Record ( optional ) :
  dest_node_id - pointer to interval record's destination node ID.
  dest_cpu_id - pointer to interval record's destination cpu ID.
  dest_thread_id - pointer to interval record's destination thread ID.


  Modified Output Variables :
. returned value - integer return status.

  Usage Notes on this subroutine :
    Since the subroutine copies all the fixed fields of the specified 
    record to the input variables, the record must be non-empty.
    The subroutine can only be called __After__ retrieving the interval
    record from the SLOG file.

  Include File Needed :
  slog.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_Irec_GetMinRec( const SLOG_intvlrec_t   *intvlrec,
                               SLOG_rectype_t    *rectype,
                               SLOG_intvltype_t  *intvltype,
                               SLOG_bebit_t      *bebit_0,
                               SLOG_bebit_t      *bebit_1,
                               SLOG_starttime_t  *starttime,
                               SLOG_duration_t   *duration,
                               SLOG_nodeID_t     *orig_node_id,
                               SLOG_cpuID_t      *orig_cpu_id,
                               SLOG_threadID_t   *orig_thread_id,
                               SLOG_where_t      *where,
                         ... )
{
    /*  Local Variables  */
    va_list            ap;
    SLOG_nodeID_t     *dest_node_id;
    SLOG_cpuID_t      *dest_cpu_id;
    SLOG_threadID_t   *dest_thread_id;

    if ( intvlrec == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Irec_GetMinRec() - "
                          "Source interval record pointer is NULL/n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    *rectype        = intvlrec->rectype;
    *intvltype      = intvlrec->intvltype;
    *bebit_0        = intvlrec->bebits[0];
    *bebit_1        = intvlrec->bebits[1];
    *starttime      = intvlrec->starttime;
    *duration       = intvlrec->duration;
    *orig_node_id   = (intvlrec->origID).node;
    *orig_cpu_id    = (intvlrec->origID).cpu;
    *orig_thread_id = (intvlrec->origID).thread;
    *where          = intvlrec->where;

    if ( SLOG_global_IsOffDiagRec( intvlrec->rectype ) ) {
        va_start( ap, where );
        dest_node_id   = va_arg( ap, SLOG_nodeID_t * );
        dest_cpu_id    = va_arg( ap, SLOG_cpuID_t * );
        dest_thread_id = va_arg( ap, SLOG_threadID_t * );
        va_end( ap );
        *dest_node_id   = (intvlrec->destID).node;
        *dest_cpu_id    = (intvlrec->destID).cpu;
        *dest_thread_id = (intvlrec->destID).thread;
    }

    return SLOG_SUCCESS;
}



/*@C
    SLOG_Irec_GetAssocs - Retrieve the association array of the interval record.

  Unmodified Input Variables :
. intvlrec - pointer to the internal Interval record to be retrieved.
. size_assocs - Number of elements available for the input association array.

  Modified Intput Variables :
. assocs - association array, array of file pointers to the associated
           interval records of the specified interval record.
           It is assumed the input array, assocs[], is big enough
           to hold the association array of the interval record.

  Modified Output Variables :
. returned value - N_assocs : Number of elements in the association array
                   returned.  If the input size, size_assocs, is smaller
                   than the actual number of elements of the association
                   array, the return value will be SLOG_EOF and only the
                   first size_assocs elements will be returned.

  Usage Notes on this subroutine :
    The subroutine gets the association array of the specified Internal 
    Interval Record.  This subroutine is meant to be called once for
    each interval record.  Calling multiple times for the same interval
    record returns the same results.  If the return value, N_assocs, 
    is equal to zero.  It means there is no associated record.

  Include File Needed :
  slog.h

@*/
int SLOG_Irec_GetAssocs( const SLOG_intvlrec_t  *intvlrec,
                         const SLOG_N_assocs_t   size_assocs,
                               SLOG_assoc_t     *assocs )
{
    int   ii;

    if ( intvlrec == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Irec_GetAssocs() - "
                          "Source interval record pointer is NULL/n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( size_assocs >= intvlrec->N_assocs ) {
        for ( ii = 0; ii < intvlrec->N_assocs; ii++ )
            assocs[ ii ] = intvlrec->assocs[ ii ];
        return ( intvlrec->N_assocs );
    }
    else {
        for ( ii = 0; ii < size_assocs; ii++ )
            assocs[ ii ] = intvlrec->assocs[ ii ];
        return SLOG_EOF;
    }
}



/*@C
    SLOG_Irec_GetArgs - Retrieve the argument array of the interval record.

  Unmodified Input Variables :
. size_args - Number of elements available for the input array.

  Modified Intput Variables :
. intvlrec - pointer to the internal interval record to be retrieved.
             The internal pointer to Vector Argument will be updated.
. args - argument array, array of call arguments of the specified 
         interval record.  It is assumed the input array, args[], is 
         big enough to hold the argument array of the interval record.


  Modified Output Variables :
. returned value - N_args : Number of elements in the argument array
                   returned.  If the input size, size_args, is smaller
                   than the actual number of elements of the argument
                   array, the return value will be SLOG_EOF and only the
                   first size_args elements will be returned.  When there
                   is no more argument array in the interval record,
                   a zero will be returned.

  Usage Notes on this subroutine :
    The subroutine retrieves argument array(s) of the specified Internal 
    Interval Record.  This subroutine is meant to be called at least once
    for each interval record extracted from the slogfile.
    If the return value, N_args, is equal to zero.  It means there is
    no argument array.   That suggests that the subroutine needs to be
    called as long as it returns positive integer ( > 0 ).

  Include File Needed :
  slog.h



@*/
int SLOG_Irec_GetArgs(       SLOG_intvlrec_t  *intvlrec,
                       const SLOG_N_args_t     size_args,
                             SLOG_arg_t       *args )
{
    const SLOG_vtrarg_t *vtr_args;
          int            ii;

    if ( intvlrec == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Irec_GetArgs() - "
                          "Source interval record pointer is NULL/n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    vtr_args = SLOG_Irec_GetVtrArgs( intvlrec );
    if ( vtr_args != NULL ) {
        if ( size_args >= vtr_args->size ) {
            for ( ii = 0; ii < vtr_args->size; ii++ )
                args[ ii ] = vtr_args->values[ ii ];
            return ( vtr_args->size );
        }
        else {
            for ( ii = 0; ii < size_args; ii++ )
                args[ ii ] = vtr_args->values[ ii ];
            return SLOG_EOF;
        }
    }
    else
        return 0;
}
