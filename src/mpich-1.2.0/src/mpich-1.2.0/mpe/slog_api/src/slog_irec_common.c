#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "slog_bebits.h"
#include "slog_tasklabel.h"
#include "slog_assoc.h"
#include "slog_vtrarg.h"
#include "slog_header.h"
#include "slog_pstat.h"
#include "slog_recdefs.h"
#include "slog_impl.h"                  /*I  "slog_impl.h"  I*/

/*@C
    SLOG_Irec_Create - Create a bare minimal interval record data structure
                       without any of its components set. (SLOG_intvlrec_t's
                       constructor)

  Modified Output Variables :
. returned value - pointer to newly allocated SLOG_intvlrec_t, interval
                   record data structure.

  Usage Notes on this routine :
  The memory can be deallocated by SLOG_Irec_Free().

  Include File Needed :
  slog.h

.N SLOG_RETURN_STATUS
@*/
SLOG_intvlrec_t *SLOG_Irec_Create( void )
{
    SLOG_intvlrec_t *irec;

    irec = ( SLOG_intvlrec_t * ) malloc( sizeof( SLOG_intvlrec_t ) );
    if ( irec == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Irec_Create() - "
                          "malloc() fails for irec\n" );
        fflush( errfile );
        return NULL;
    }

    irec->bytesize  = 0;
    irec->N_assocs  = 0;
    irec->assocs    = NULL;
    irec->vhead     = NULL;
    irec->vtail     = NULL;
    irec->vptr      = NULL;

    return irec;
}



/*@C
    SLOG_Irec_Free - Deallocate the given interval record data structure, i.e. 
                     free the memory associated with it. (SLOG_intvlrec_t's
                     destructor)

  Modified Input Variables :
. irec - pointer to the interval record data structure to be erased/freed.

  Usage Notes on this routine :
  Free the memory allocated by SLOG_Irec_Create(), SLOG_Irec_Copy() and
  other related routines.

  Include File Needed :
  slog.h
@*/
void SLOG_Irec_Free( SLOG_intvlrec_t *irec )
{
    if ( irec != NULL ) {
        SLOG_Irec_DelAllVtrArgs( irec );
        if ( irec->assocs   != NULL ) free( irec->assocs );
        free( irec );
        irec = NULL;
    }
}



void SLOG_Irec_CopyVal( const SLOG_intvlrec_t *src, SLOG_intvlrec_t *dest )
{
    dest->bytesize  = src->bytesize;
    dest->rectype   = src->rectype;
    dest->intvltype = src->intvltype;
    dest->bebits[0] = src->bebits[0];
    dest->bebits[1] = src->bebits[1];
    dest->starttime = src->starttime;
    dest->duration  = src->duration;
    SLOG_TaskID_Copy( &( dest->origID ), &( src->origID ) );
    if ( SLOG_global_IsOffDiagRec( src->rectype ) )
        SLOG_TaskID_Copy( &( dest->destID ), &( src->destID ) );
    dest->where     = src->where;

    /*
        Here assume memory for the src->args[] has been allocated
        All the function does is copying the pointer
    */
    dest->N_assocs  = src->N_assocs;
    dest->assocs    = src->assocs;
    dest->vhead     = src->vhead;
    dest->vtail     = src->vtail;
    dest->vptr      = src->vptr;
}



/*@C
    SLOG_Irec_Copy - Copy the content of the specified interval record 
                     to a newly allocated one and return it.  
                     (SLOG_intvlrec_t's Copy constructor)

  Unmodified Input Variables :
. src - pointer to constant SLOG_intvlrec_t to be copied.

  Modified Output Variables :
. returned value - dest, pointer to the newly allocated SLOG_intvlrec_t, 
                   interval record data structure.

  Usage Notes on this routine :
  This routine is similar to SLOG_Irec_Create(), except it uses
  the content of the given SLOG_intvlrec_t to create/copy a new one.
  The memory can be deallocated by SLOG_Irec_Free().

  Include File Needed :
  slog.h
@*/
SLOG_intvlrec_t *SLOG_Irec_Copy( const SLOG_intvlrec_t *src )
{
          SLOG_intvlrec_t     *dest;
    const SLOG_vtrarg_lptr_t  *vptr;
    int ii;

    dest = SLOG_Irec_Create();
    if ( dest == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Irec_Copy() - "
                          "SLOG_Irec_Create() fails\n" );
        fflush( errfile );
        return NULL;
    }

    dest->bytesize  = src->bytesize;
    dest->rectype   = src->rectype;
    dest->intvltype = src->intvltype;
    dest->bebits[0] = src->bebits[0];
    dest->bebits[1] = src->bebits[1];
    dest->starttime = src->starttime;
    dest->duration  = src->duration;
    SLOG_TaskID_Copy( &( dest->origID ), &( src->origID ) );
    if ( SLOG_global_IsOffDiagRec( src->rectype ) )
        SLOG_TaskID_Copy( &( dest->destID ), &( src->destID ) );
    dest->where     = src->where;
    dest->N_assocs  = src->N_assocs;

    if ( dest->N_assocs == 0 )
        dest->assocs = NULL;
    else {
        dest->assocs = ( SLOG_assoc_t * ) malloc( dest->N_assocs
                                                * sizeof( SLOG_assoc_t ) );
        if ( dest->N_assocs > 0 && dest->assocs == NULL ) {
            fprintf( errfile, __FILE__":SLOG_Irec_Copy() - "
                              "malloc() for dest->assocs fails\n" );
            fflush( errfile );
            return NULL;
        }

        for ( ii = 0; ii < dest->N_assocs; ii++ )
            dest->assocs[ ii ] = src->assocs[ ii ];
    }

    if ( dest->vhead == NULL && dest->vtail == NULL ) {
        for( vptr = src->vhead; vptr != NULL; vptr = vptr->next )
            SLOG_Irec_AddVtrArgs( dest, vptr->vtr );
    }
    else {
        fprintf( errfile, __FILE__":SLOG_Irec_Copy() - "
                          "dest->vhead != NULL or dest->vtail != NULL\n" );
        fflush( errfile );
        return NULL;
    }
    dest->vptr      = src->vptr;

    return dest;
}



/*@C
    SLOG_Irec_Print - Print the content of interval record at the
                      specified file stream in ASCII format.  
                      (SLOG_intvlrec_t's Print operator)

  Unmodified Input Variables :
. irec - constant SLOG_intvlrec_t pointer to be printed to the specified
         file stream.

  Modified Output variables :
. outfd - pointer to the specified file stream.
. returned value - integer return status

  Usage Notes on this subroutine :
  This routine is mainly used for debugging purposes.

  Include File Needed :
  slog.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_Irec_Print( const SLOG_intvlrec_t *irec, FILE *outfd )
{
    SLOG_vtrarg_lptr_t *vptr;
    int                 ii;

    if ( irec == NULL ) {
        fprintf( outfd, __FILE__":SLOG_Irec_Print() - the input interval "
                        "record pointer is NULL\n" );
        fflush( outfd );
        return SLOG_FAIL;
    }

    fprintf( outfd, fmt_sz_t" ",     irec->bytesize );
    fprintf( outfd, fmt_rtype_t" ",  irec->rectype );
    fprintf( outfd, fmt_itype_t" ",  irec->intvltype );
    fprintf( outfd, "( "fmt_bebit_t", "fmt_bebit_t" ) ", 
                    irec->bebits[0], irec->bebits[1] );
    fprintf( outfd, fmt_stime_t" ",  irec->starttime );
    fprintf( outfd, fmt_dura_t" ",   irec->duration );
    SLOG_TaskID_Print( &( irec->origID ), outfd ); fprintf( outfd, " " );
    if ( SLOG_global_IsOffDiagRec( irec->rectype ) )
        SLOG_TaskID_Print( &( irec->destID ), outfd ); fprintf( outfd, " " );
#if ! defined( NOWHERE )
    fprintf( outfd, fmt_where_t" ",  irec->where );
#endif

    fprintf( outfd, "{ " );
    for ( ii = 0; ii < irec->N_assocs; ii++ )
        fprintf( outfd, fmt_assoc_t" ", irec->assocs[ ii ] );
    fprintf( outfd, "} " );

    for ( vptr = irec->vhead; vptr != NULL; vptr = vptr->next )
        SLOG_Varg_Print( vptr->vtr, outfd );

    return SLOG_SUCCESS;
}



/*@C
   SLOG_Irec_IsEmpty - Check if the specified Interval record is empty

  Unmodified Input Variables :
. irec - constant SLOG_intvlrec_t pointer to be checked

  Modified Output variables :
. returned value - integer return status

  Usage Notes on this subroutine :
  This routine is mainly used for debugging purposes.

  Include File Needed :
  slog.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_Irec_IsEmpty( const SLOG_intvlrec_t *irec )
{
    return( irec->bytesize == 0 );
}



/*@C
   SLOG_Irec_IsConsistent - Check if the specified Interval record's length, 
                            bytesize, is consistent with what it contains. 

  Unmodified Input Variables :
. irec - constant SLOG_intvlrec_t pointer to be checked

  Modified Output variables :
. returned value - integer return status

  Usage Notes on this subroutine :
  This routine is mainly used for debugging purposes.
  
  Include File Needed :
  slog.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_Irec_IsConsistent( const SLOG_intvlrec_t *irec )
{
    const SLOG_vtrarg_lptr_t  *vptr;
          SLOG_uint32          sizeof_irec;

    if ( irec->bytesize >= SLOG_typesz[ min_IntvlRec ] ) {

        if ( irec->bytesize == SLOG_typesz[ min_IntvlRec ] ) {
             if ( irec->N_assocs > 0 || irec->assocs != NULL )
                 return SLOG_FALSE;
             if ( irec->vhead != NULL || irec->vtail != NULL )
                 return SLOG_FALSE;

             return SLOG_TRUE;
        }

        if ( irec->N_assocs > 0 && irec->assocs == NULL ) 
            return SLOG_FALSE;
        if ( irec->N_assocs == 0 && irec->assocs != NULL ) 
            return SLOG_FALSE;
        /*
        if ( irec->N_assocs < 0 )
            return SLOG_FALSE;
        */

        sizeof_irec = SLOG_typesz[ min_IntvlRec ]
                    + irec->N_assocs * sizeof( SLOG_assoc_t );

        if ( SLOG_global_IsOffDiagRec( irec->rectype ) )
            sizeof_irec += SLOG_typesz[ taskID_t ];

        for ( vptr = irec->vhead; vptr != NULL; vptr = vptr->next ) {
            if ( SLOG_global_IsVarRec( irec->rectype ) )
                sizeof_irec += sizeof( SLOG_N_args_t );
            if ( vptr->vtr->size > 0 && vptr->vtr->values != NULL )
                sizeof_irec += vptr->vtr->size * sizeof( SLOG_arg_t );
            else
                return SLOG_FALSE;
        }

        if ( irec->bytesize != sizeof_irec )
            return SLOG_FALSE;

        return SLOG_TRUE;
    }
    else
        return SLOG_FAIL;
}



int SLOG_Irec_IsEqualTo( const SLOG_intvlrec_t *irec1, 
                         const SLOG_intvlrec_t *irec2 )
{
    return (    ( irec1->bytesize  == irec2->bytesize  )
             && ( irec1->rectype   == irec2->rectype   )
             && ( irec1->intvltype == irec2->intvltype )
             && ( irec1->bebits[0] == irec2->bebits[0] )
             && ( irec1->bebits[1] == irec2->bebits[1] )
             && ( irec1->starttime == irec2->starttime )
             && ( irec1->duration  == irec2->duration  )
             && SLOG_TaskID_IsEqualTo( &(irec1->origID), &(irec2->origID) )
             && ( SLOG_global_IsOffDiagRec( irec1->rectype )                      ?
                  SLOG_TaskID_IsEqualTo( &(irec1->destID), &(irec2->destID) ) :
                  1 )
             && ( irec1->where     == irec2->where     )
             && SLOG_Assoc_IsEqualTo( irec1->N_assocs, irec1->assocs, 
                                      irec2->N_assocs, irec2->assocs ) 
             && SLOG_Vargs_AreEqualTo( irec1->vhead, irec2->vhead ) );
}



        /*  Low level SLOG_Irec Methods  */

int SLOG_Irec_DepositToFbuf( const SLOG_intvlrec_t *irec,
                                   filebuf_t       *fbuf )
{
    SLOG_vtrarg_lptr_t *vptr;
    SLOG_bebits_t       bebits;
    SLOG_uint32         count;
    SLOG_uint32         Nbytes_written;
    int                 ierr;

    Nbytes_written = 0;

    ierr = fbuf_deposit( &( irec->bytesize ), SLOG_typesz[ sz_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - Cannot "
                          "deposit the ByteSize to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ sz_t ];

    ierr = fbuf_deposit( &( irec->rectype ), SLOG_typesz[ rtype_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - Cannot "
                          "deposit the EVENT_TYPE to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ rtype_t ];

    ierr = fbuf_deposit( &( irec->intvltype ), SLOG_typesz[ itype_t ],
                         1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - Cannot "
                          "deposit the RECORD_TYPE to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ itype_t ];

    bebits = SLOG_bebits_encode( irec->bebits[0], irec->bebits[1] );
    ierr = fbuf_deposit( &bebits, SLOG_typesz[ bebits_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - Cannot "
                          "deposit the BEBITS to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ bebits_t ];

    ierr = fbuf_deposit( &( irec->starttime ),
                         SLOG_typesz[ stime_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - Cannot "
                          "deposit the STARTtime to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ stime_t ];

    ierr = fbuf_deposit( &( irec->duration ), SLOG_typesz[ dura_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - Cannot "
                          "deposit the DURATION to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ dura_t ];

    ierr = SLOG_TaskID_DepositToFbuf( &( irec->origID ), fbuf );
    if ( ierr < SLOG_typesz[ taskID_t ] ) {
        fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - Cannot "
                          "deposit the Origin TASK IDs to the SLOG "
                          "filebuffer, "fmt_ui32" bytes written so far\n",
                          Nbytes_written );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr;

    if ( SLOG_global_IsOffDiagRec( irec->rectype ) ) {
        ierr = SLOG_TaskID_DepositToFbuf( &( irec->destID ), fbuf );
        if ( ierr < SLOG_typesz[ taskID_t ] ) {
            fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - Cannot "
                              "deposit the Destination TASK IDs to the SLOG "
                              "filebuffer, "fmt_ui32" bytes written so far\n",
                              Nbytes_written );
            fprintf( errfile, "      ""irec = " );
            SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
            fflush( errfile );
            return SLOG_FAIL;
        }
        Nbytes_written += ierr;
    }

#if ! defined( NOWHERE )
    ierr = fbuf_deposit( &( irec->where ), SLOG_typesz[ where_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - Cannot "
                          "deposit the WHERE to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ where_t ];
#endif

    ierr = fbuf_deposit( irec->assocs, 
                         SLOG_typesz[ assoc_t ], irec->N_assocs, fbuf );
    if ( ierr < irec->N_assocs ) {
        fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - Cannot "
                          "deposit the ASSOCIATIONs to the SLOG filebuffer, "
                          fmt_ui32" bytes written so far\n", Nbytes_written );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    Nbytes_written += ierr * SLOG_typesz[ assoc_t ];

    if ( SLOG_global_IsVarRec( irec->rectype ) ) {
        count = 0;
        for ( vptr = irec->vhead; vptr != NULL; vptr = vptr->next, count++ ) {
            if ( vptr->vtr == NULL ) {
                fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - "
                                  "the %d-th node of vector arguments has "
                                  "NULL vector\n", count );
                fprintf( errfile, "      ""irec = " );
                SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
                fflush( errfile );
                return SLOG_FAIL;
            }

            ierr = fbuf_deposit( &( vptr->vtr->size ),
                                 SLOG_typesz[ Nargs_t ], 1, fbuf );
            if ( ierr < 1 ) {
                fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - "
                                  "Cannot deposit %d-th ARGS's size, "
                                  fmt_Nargs_t ", to the SLOG filebuffer\n",
                                  count, vptr->vtr->size );
                fprintf( errfile, "      ""irec = " );
                SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
                fflush( errfile );
                return SLOG_FAIL;
            }
            Nbytes_written += ierr * SLOG_typesz[ Nargs_t ];

            ierr = fbuf_deposit( vptr->vtr->values,
                                 SLOG_typesz[ arg_t ], vptr->vtr->size, fbuf );
            if ( ierr < vptr->vtr->size ) {
                fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - "
                                  "Cannot deposit %d-th "
                                  "ARGS[0:"fmt_Nargs_t"-1] to the SLOG "
                                  "filebuffer\n", count, vptr->vtr->size );
                fprintf( errfile, "       "fmt_ui32" bytes written so far\n",
                                  Nbytes_written );
                fprintf( errfile, "      ""irec = " );
                SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
                fflush( errfile );
                return SLOG_FAIL;
            }
            Nbytes_written += ierr * SLOG_typesz[ arg_t ];
        }   /*  Endof { for ( vptr = ... ) }  */
    }
    else {   /*  if ( SLOG_global_IsVarRec( irec->rectype ) )  */
        vptr = irec->vhead;
        if ( vptr != NULL ) {
            if ( vptr->vtr == NULL ) {
                fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - "
                                  "the vector argument has NULL vector\n" );
                fprintf( errfile, "      ""irec = " );
                SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
                fflush( errfile );
                return SLOG_FAIL;
            }

            ierr = fbuf_deposit( vptr->vtr->values,
                                 SLOG_typesz[ arg_t ], vptr->vtr->size, fbuf );
            if ( ierr < vptr->vtr->size ) {
                fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - "
                                  "Cannot deposit "
                                  "ARGS[0:"fmt_Nargs_t"-1] to the SLOG "
                                  "filebuffer\n", vptr->vtr->size );
                fprintf( errfile, "       "fmt_ui32" bytes written so far\n",
                                  Nbytes_written );
                fprintf( errfile, "      ""irec = " );
                SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
                fflush( errfile );
                return SLOG_FAIL;
            }
            Nbytes_written += ierr * SLOG_typesz[ arg_t ];
        }   /*  Endof    if ( vptr != NULL )  */
    }   /*  Endof   if ( SLOG_global_IsVarRec( irec->rectype ) )  */

#if defined (DEBUG)
    /*   Consistency check if deposited number of bytes is as expected   */
    if ( Nbytes_written != irec->bytesize ) {
        fprintf( errfile, __FILE__":SLOG_Irec_DepositToFbuf() - "
                          "Nbytes_written = "fmt_ui32", but irec->bytesize = "
                          fmt_sz_t"\n", Nbytes_written, irec->bytesize );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    return Nbytes_written;
}



SLOG_intvlrec_t *
SLOG_Irec_WithdrawFromFbuf( const SLOG_recdefs_table_t  *slog_recdefs,
                                  filebuf_t             *fbuf )
{
    SLOG_intvlrec_t *irec;
    SLOG_vtrarg_t   *vtr_args;
    SLOG_N_args_t    N_args;
    SLOG_bebits_t    bebits;
    SLOG_uint32      count;
    SLOG_uint32      Nbytes_read;
    int              ierr;


    irec = SLOG_Irec_Create();
    if ( irec == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - "
                          "SLOG_Irec_Create() fails\n" );
        fflush( errfile );
        return NULL;
    }

    Nbytes_read = 0;

    ierr = fbuf_withdraw( &( irec->bytesize ),
                          SLOG_typesz[ sz_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - Cannot "
                          "withdraw the ByteSize from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fflush( errfile );
        free( irec );
        return NULL;
    }
    Nbytes_read += ierr * SLOG_typesz[ sz_t ];

    ierr = fbuf_withdraw( &( irec->rectype ), SLOG_typesz[ rtype_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - Cannot "
                          "withdraw the EVENT_TYPE from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        free( irec );
        return NULL;
    }
    Nbytes_read += ierr * SLOG_typesz[ rtype_t ];

    ierr = fbuf_withdraw( &( irec->intvltype ),
                          SLOG_typesz[ itype_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - Cannot "
                          "withdraw the RECORD_TYPE from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        free( irec );
        return NULL;
    }
    Nbytes_read += ierr * SLOG_typesz[ itype_t ];

    ierr = fbuf_withdraw( &bebits, SLOG_typesz[ bebits_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - Cannot "
                          "withdraw the BEBITS from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        free( irec );
        return NULL;
    }
    Nbytes_read += ierr * SLOG_typesz[ bebits_t ];
    SLOG_bebits_decode( bebits, irec->bebits );

    ierr = fbuf_withdraw( &( irec->starttime ),
                         SLOG_typesz[ stime_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - Cannot "
                          "withdraw the STARTtime from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        free( irec );
        return NULL;
    }
    Nbytes_read += ierr * SLOG_typesz[ stime_t ];

    ierr = fbuf_withdraw( &( irec->duration ),
                          SLOG_typesz[ dura_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - Cannot "
                          "withdraw the DURATION from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        free( irec );
        return NULL;
    }
    Nbytes_read += ierr * SLOG_typesz[ dura_t ];

    ierr = SLOG_TaskID_WithdrawFromFbuf( &( irec->origID ), fbuf );
    if ( ierr < SLOG_typesz[ taskID_t ] ) {
        fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - Cannot "
                          "withdraw the Origin Task IDs from the SLOG "
                          "filebuffer, "fmt_ui32" bytes read so far\n",
                          Nbytes_read );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        free( irec );
        return NULL;
    }
    Nbytes_read += ierr;

    if ( SLOG_global_IsOffDiagRec( irec->rectype ) ) {
        ierr = SLOG_TaskID_WithdrawFromFbuf( &( irec->destID ), fbuf );
        if ( ierr < SLOG_typesz[ taskID_t ] ) {
            fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - Cannot "
                              "withdraw the Destination Task IDs from the SLOG "
                              "filebuffer, "fmt_ui32" bytes read so far\n",
                              Nbytes_read );
            fprintf( errfile, "      ""irec = " );
            SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
            fflush( errfile );
            free( irec );
            return NULL;
        }
        Nbytes_read += ierr;
    }

#if ! defined( NOWHERE )
    ierr = fbuf_withdraw( &( irec->where ), SLOG_typesz[ where_t ], 1, fbuf );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - Cannot "
                          "withdraw the WHERE from the SLOG filebuffer, "
                          fmt_ui32" bytes read so far\n", Nbytes_read );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
        free( irec );
        return NULL;
    }
    Nbytes_read += ierr * SLOG_typesz[ where_t ];
#else
    irec->where = 0;
#endif

    ierr = SLOG_RecDef_NumOfAssocs( slog_recdefs, irec->intvltype,
                                    irec->bebits[0], irec->bebits[1] );
    if ( ierr == SLOG_FAIL ) {
        fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - "
                          "SLOG_RecDef_NumOfAssocs() fails !!\n" );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile );
        fprintf( errfile, "\n" );
        fflush( errfile );
        free( irec );
        return NULL;
    }

    if ( ierr > 0 )
        irec->N_assocs = ierr;
    else
        irec->N_assocs = 0;

    if ( irec->N_assocs > 0 ) {
        irec->assocs = ( SLOG_assoc_t * ) malloc( irec->N_assocs
                                                * sizeof( SLOG_assoc_t ) );
        if ( irec->N_assocs > 0 && irec->assocs == NULL ) {
            fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - "
                              "malloc() for irec->assocs fails\n" );
            fflush( errfile );
            free( irec );
            return NULL;
        }

        ierr = fbuf_withdraw( irec->assocs,
                              SLOG_typesz[ assoc_t ], irec->N_assocs, fbuf );
        if ( ierr < irec->N_assocs ) {
            fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - Cannot "
                              "withdraw the ASSOCIATIONs from the SLOG "
                              "filebuffer, "fmt_ui32" bytes read so far\n",
                              Nbytes_read );
            fprintf( errfile, "      ""irec = " );
            SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
            fflush( errfile );
            free( irec );
            return NULL;
        }
        Nbytes_read += ierr * SLOG_typesz[ assoc_t ];
    }   /*  Endof if ( irec->N_assocs > 0 )  */

    if ( SLOG_global_IsVarRec( irec->rectype ) ) {
        for ( count = 0; Nbytes_read < irec->bytesize; count ++ ) {
            ierr = fbuf_withdraw( &N_args,
                                  SLOG_typesz[ Nargs_t ], 1, fbuf );
            if ( ierr < 1 ) {
                fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - "
                                  "Cannot withdraw the %d-th ARGS's size "
                                  "to the SLOG filebuffer\n", count );
                fprintf( errfile, "       ""The Irec's bytesize = "fmt_sz_t"\n",
                                  irec->bytesize );
                fprintf( errfile, "       "fmt_ui32" bytes read so far\n",
                                  Nbytes_read );
                fprintf( errfile, "      ""irec = " );
                SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
                fflush( errfile );
                free( irec );
                return NULL;
            }
            Nbytes_read += ierr * SLOG_typesz[ Nargs_t ];

            vtr_args = SLOG_Varg_Create( N_args, NULL );

            if ( vtr_args != NULL ) {
                ierr = fbuf_withdraw( vtr_args->values, SLOG_typesz[ arg_t ],
                                      vtr_args->size, fbuf );
                if ( ierr < vtr_args->size ) {
                    fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - "
                                      "Cannot withdraw the %d-th "
                                      "ARGS[0:"fmt_Nargs_t"-1] to the SLOG "
                                      "filebuffer\n", count, vtr_args->size );
                    fprintf( errfile, "       ""The Irec's bytesize = "
                                      fmt_sz_t"\n", irec->bytesize );
                    fprintf( errfile, "       "fmt_ui32" bytes read so far\n",
                                      Nbytes_read );
                    fprintf( errfile, "      ""irec = " );
                    SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
                    fflush( errfile );
                    free( irec );
                    return NULL;
                }
                Nbytes_read += ierr * SLOG_typesz[ arg_t ];
            }

            if ( SLOG_Irec_AddVtrArgs( irec, vtr_args ) != SLOG_SUCCESS ) {
                fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - "
                                  "The %d-th SLOG_Irec_AddVtrArgs() fails\n",
                                  count );
                fprintf( errfile, "      ""irec = " );
                SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
                fflush( errfile );
                free( irec );
                return NULL;
            }
        }   /*   for ( count = 0; Nbytes_read < irec->bytesize; count ++ )   */
    }
    else {   /*  if ( SLOG_global_IsVarRec( irec->rectype ) )  */
        ierr = SLOG_RecDef_NumOfArgs( slog_recdefs, irec->intvltype,
                                      irec->bebits[0], irec->bebits[1] );
        if ( ierr == SLOG_FAIL ) {
            fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - "
                              "SLOG_RecDef_NumOfArgs() fails !!\n" );
            fprintf( errfile, "      ""irec = " );
            SLOG_Irec_Print( irec, errfile );
            fprintf( errfile, "\n" );
            fflush( errfile );
            free( irec );
            return NULL;
        }

        if ( ierr > 0 )
            N_args = ierr;
        else
            N_args = 0;
        
        vtr_args = SLOG_Varg_Create( N_args, NULL );

        if ( vtr_args != NULL ) {
            ierr = fbuf_withdraw( vtr_args->values,
                                  SLOG_typesz[ arg_t ], vtr_args->size, fbuf );
            if ( ierr < vtr_args->size ) {
                fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - "
                                  "Cannot withdraw "
                                  "ARGS[0:"fmt_Nargs_t"-1] to the SLOG "
                                  "filebuffer\n", vtr_args->size );
                fprintf( errfile, "       ""The Irec's bytesize = "
                                  fmt_sz_t"\n", irec->bytesize );
                fprintf( errfile, "       "fmt_ui32" bytes read so far\n",
                                  Nbytes_read );
                fprintf( errfile, "      ""irec = " );
                SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
                fflush( errfile );
                free( irec );
                return NULL;
            }
            Nbytes_read += ierr * SLOG_typesz[ arg_t ];
        }

        if ( SLOG_Irec_AddVtrArgs( irec, vtr_args ) != SLOG_SUCCESS ) {
            fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - "
                              "SLOG_Irec_AddVtrArgs() fails\n" );
            fprintf( errfile, "      ""irec = " );
            SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
            fflush( errfile );
            free( irec );
            return NULL;
        }
    }        /*  Endof   if ( SLOG_global_IsVarRec( irec->rectype ) )  */

#if defined (DEBUG)
    /*   Consistency check if withdrawn number of bytes is as expected   */
    if ( Nbytes_read != irec->bytesize ) {
        fprintf( errfile, __FILE__":SLOG_Irec_WithdrawFromFbuf() - "
                          "Nbytes_read = "fmt_ui32", but irec->bytesize = "
                          fmt_sz_t"\n", Nbytes_read, irec->bytesize );
        fprintf( errfile, "      ""irec = " );
        SLOG_Irec_Print( irec, errfile ); fprintf( errfile, "\n" );
        fflush( errfile );
    }
#endif

    return irec;
}



     /*  Routines which interact with SLOG_Varg(i.e. SLOG_vtrarg_t *) class  */

int SLOG_Irec_AddVtrArgs(       SLOG_intvlrec_t *irec,
                          const SLOG_vtrarg_t   *varg )
{
    SLOG_vtrarg_lptr_t  *cur_node;

    if ( varg == NULL )
        return SLOG_SUCCESS;

    cur_node = ( SLOG_vtrarg_lptr_t * ) malloc( sizeof( SLOG_vtrarg_lptr_t ) );
    if ( cur_node == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Irec_AddVtrArgs() - "
                          "malloc() for cur_node fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Copy the input vtr_arg, varg, to the newly allocated cur_node->vtr  */
    cur_node->vtr = SLOG_Varg_Copy( varg );
    if ( cur_node->vtr == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Irec_AddVtrArgs() - "
                          "SLOG_Varg_Copy() fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Attach the node to the bi-directional linked list  */
    cur_node->prev = irec->vtail;
    cur_node->next = NULL;

    /* this line gurantees bi-directional linked list */
    if ( cur_node->prev != NULL )
        /* Check if the cur_node is Irec's head, vhead->prev == NULL */
        ( cur_node->prev )->next = cur_node;

    irec->vtail = cur_node;

    /* this line gurantees that the lhead is pointing at the head  */
    if ( irec->vhead == NULL )
        irec->vhead = irec->vtail;

    return SLOG_SUCCESS;
}



/*
    For identical interval record, this routine returns the Vector Arguments
    of the interval record one by one.  The return sequence is as follows:

    return sequence = vhead, ..., vtail, NULL, vhead, ..., vtail, NULL, ...

    So it is crucial that the vptr in SLOG_intvlrec_t is initialized to NULL
    And the routine does NOT advance the vptr before returning.  The vptr is
    advanced when entering the routine.

    If the irec has NO vector arguments, i.e. vhead = NULL, the routine
    will always return NULL.
*/
const SLOG_vtrarg_t *SLOG_Irec_GetVtrArgs( SLOG_intvlrec_t  *irec )
{
    SLOG_vtrarg_t       *varg  = NULL;

    if ( irec->vptr == NULL )
        irec->vptr = irec->vhead;
    else
        irec->vptr = irec->vptr->next;

    if ( irec->vptr != NULL )
        varg = irec->vptr->vtr;
    else
        return NULL;

    return varg;
}



void SLOG_Irec_DelAllVtrArgs( SLOG_intvlrec_t  *irec )
{
    SLOG_vtrarg_lptr_t  *vptr;

    if ( irec != NULL ) {
        while ( irec->vtail != NULL ) {
            vptr = irec->vtail;
            irec->vtail = ( irec->vtail )->prev;
            SLOG_Varg_Free( vptr->vtr );
            free( vptr );
        }

        irec->vhead = NULL;
        irec->vtail = NULL;
    }
}
