#include <stdlib.h>
#include <stdio.h>
#include "bswp_fileio.h"
#include "slog_fileio.h"
#include "slog_header.h"
#include "slog_ttable.h"              /*I "slog_ttable.h" I*/



int SLOG_TTAB_IsDuplicated( const SLOG_threadinfo_table_t  *ttable,
                            const SLOG_threadinfo_t        *in_thread )
{
    SLOG_threadinfo_t  *cur_thread;
    int                 ii;
    int                 NotMatched = 1;

    for ( ii = 0; NotMatched && (ii < ttable->Nentries); ii++ ) {
        cur_thread = &( ttable->entries[ ii ] );
        NotMatched = ! SLOG_ThInfo_IsKeyEqualTo( cur_thread, in_thread );
    }

    return ! NotMatched;
}

/*@C
    SLOG_TTAB_Open - Creates the SLOG Thread Table, SLOG_threadinfo_table_t,
                     and prepare it to be written to the disk.

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM where SLOG Thread Table is located.

  Modified Output Variables :
. returned value - integer return status.

  Include file needed : 
  slog_ttable.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_TTAB_Open( SLOG_STREAM  *slog )
{
    slog->thread_tab = ( SLOG_threadinfo_table_t * )
                       malloc( sizeof( SLOG_threadinfo_table_t ) );
    if ( slog->thread_tab == NULL ) {
        fprintf( errfile, __FILE__":SLOG_TTAB_Open() - malloc() fails,"
                          "Cannot allocate SLOG_threadinfo_table_t\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    slog->thread_tab->increment  = 10;
    slog->thread_tab->capacity   = slog->thread_tab->increment;
    slog->thread_tab->Nentries   = 0;
    slog->thread_tab->entries    = ( SLOG_threadinfo_t * )
                                   malloc(   slog->thread_tab->capacity
                                           * sizeof( SLOG_threadinfo_t ) );

    return SLOG_SUCCESS;
}



/*@C
    SLOG_TTAB_AddThreadInfo - Add thread info to the SLOG Thread Table.

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM where SLOG Thread Table is located.

  Unmodified Input Variables :
. in_node_id - node ID.
. in_thread_id - logic thread ID.
. in_OSprocess_id - OS process ID.
. in_OSthread_id - OS thread ID.
. in_app_id - application ID.

 Modified Output Variables :
. returned value - integer return status.

  Include file needed : 
  slog_ttable.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_TTAB_AddThreadInfo(       SLOG_STREAM       *slog,
                             const SLOG_nodeID_t      in_node_id,
                             const SLOG_threadID_t    in_thread_id,
                             const SLOG_OSprocessID_t in_OSprocess_id,
                             const SLOG_OSthreadID_t  in_OSthread_id,
                             const SLOG_appID_t       in_app_id )
{
    SLOG_threadinfo_table_t  *table;
    SLOG_uint32               idx;

    table = slog->thread_tab;

#if defined( DEBUG )
    if ( table == NULL ) {
        fprintf( errfile, __FILE__":SLOG_TTAB_AddThreadInfo() - "
                          "the SLOG_threadinfo_table_t pointer in slog "
                          "is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
#endif

    /*  Check if there is any space left for a new record definition */
    if ( table->Nentries + 1 > table->capacity ) {
        table->capacity += table->increment;
        table->entries = ( SLOG_threadinfo_t * )
                           realloc( table->entries,
                                      table->capacity
                                    * sizeof( SLOG_threadinfo_t )
                                  );
        if ( table->entries == NULL ) {
            fprintf( errfile, __FILE__":SLOG_TTAB_AddThreadInfo() - "
                              "realloc fails, cannot increase the size "
                              "of table->entries[]\n" );
            fprintf( errfile, "\t""The input thread info = [ ( "
                              fmt_nodeID_t", "fmt_thID_t" ), "
                              fmt_OSprocID_t", "fmt_OSthID_t", "fmt_appID_t
                              " ] \n",
                              in_node_id, in_thread_id,
                              in_OSprocess_id, in_OSthread_id, in_app_id );
            fflush( errfile );
            return SLOG_FAIL;
        }
    }

    idx = table->Nentries;
    SLOG_ThInfo_Assign( &( table->entries[ idx ] ),
                        in_node_id, in_thread_id,
                        in_OSprocess_id, in_OSthread_id, in_app_id );
    if ( SLOG_TTAB_IsDuplicated( table, &(table->entries[ idx ]) ) ) {
        fprintf( errfile, __FILE__":SLOG_TTAB_AddThreadInfo() - "
                          "there is a copy of input Thread\n " );
        fprintf( errfile, "\t""info with identical keys "
                          "in the SLOG thread table\n" );
        fprintf( errfile, "\t""The input thread info = [ ( "
                          fmt_nodeID_t", "fmt_thID_t" ), "
                          fmt_OSprocID_t", "fmt_OSthID_t", "fmt_appID_t
                          " ] \n",
                          in_node_id, in_thread_id,
                          in_OSprocess_id, in_OSthread_id, in_app_id );
        fflush( errfile );
        return SLOG_FAIL;
    }
    table->Nentries += 1;

    return SLOG_SUCCESS;
}



/*@C
    SLOG_TTAB_Close - Write the SLOG_Thread Table, SLOG_threadinfo_table_t,
                      onto the disk

  Modified Input Variables :
. slog - pointer to the SLOG_STREAM where SLOG Thread Table is located.

  Modifies Output Variables :
. returned value - integer status code.

  Include file needed : 
  slog_ttable.h

.N SLOG_RETURN_STATUS
@*/
int SLOG_TTAB_Close( SLOG_STREAM  *slog )
{
    SLOG_threadinfo_table_t    *ttable;
    int                         ierr;
    int                         ii;

    /*  Update the content of SLOG_hdr_t to both memory and disk  */
    slog->hdr->fptr2threadtable = slog_ftell( slog->fd );
    ierr = SLOG_WriteUpdatedHeader( slog );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile,  __FILE__":SLOG_TTAB_Close() - "
                                   "SLOG_WriteUpdatedHeader() fails\n" );
        fflush( errfile );
        return ierr;
    }

    /*  Write the content of SLOG Thread Table to the disk  */
    ttable = slog->thread_tab;

    ierr = bswp_fwrite( &( ttable->Nentries ),
                        sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile,  __FILE__":SLOG_TTAB_Close() - \n"
                           "\t""SLOG_ThInfo_WriteToFile() fails at "
                           "writing the number of threadinfos to "
                           "table section of logfile\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    for ( ii = 0; ii < ttable->Nentries; ii++ ) {
        ierr = SLOG_ThInfo_WriteToFile( &( ttable->entries[ ii ] ),
                                        slog->fd );
        if ( ierr != SLOG_TRUE ) {
            fprintf( errfile,  __FILE__":SLOG_TTAB_Close() - \n"
                               "\t""SLOG_ThInfo_WriteToFile() fails at "
                               "%d-th addition of the threadinfo to "
                               "table section of logfile\n", ii );
            fflush( errfile );
            return SLOG_FAIL;
        }
    }

    return SLOG_SUCCESS;
}



void SLOG_TTAB_Free( SLOG_threadinfo_table_t *ttable )
{
    if ( ttable != NULL ) {
        if ( ttable->Nentries > 0 && ttable->entries != NULL ) {
            free( ttable->entries );
            ttable->entries = NULL;
        }
        free( ttable );
        ttable = NULL;
    }
}



int SLOG_TTAB_ReadThreadInfos( SLOG_STREAM  *slog )
{
    SLOG_threadinfo_table_t    *ttable;
    int                         ii;
    int                         ierr;

    slog->thread_tab = ( SLOG_threadinfo_table_t * )
                       malloc( sizeof( SLOG_threadinfo_table_t ) );
    if ( slog->thread_tab == NULL ) {
        fprintf( errfile, __FILE__":SLOG_TTAB_ReadThreadInfos() - "
                          "malloc() fails, Cannot allocate memory for"
                          "SLOG_threadinfo_table_t\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Initialize the Thread Table data structure  */
    slog->thread_tab->increment  = 10;
    slog->thread_tab->capacity   = slog->thread_tab->increment;
    slog->thread_tab->Nentries   = 0;
    slog->thread_tab->entries    = NULL;

    /*  Seek to the beginning of the Thread Table in the SLOG file  */
    if ( slog->hdr->fptr2threadtable == SLOG_fptr_NULL ) {
        fprintf( errfile, __FILE__":SLOG_ReadThreadInfos() - "
                          "slog->hdr->fptr2threadtable == SLOG_fptr_NULL\n" );
        fprintf( errfile, "\t""SLOG_TTAB_Close() may NOT be called\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    ierr = slog_fseek( slog->fd, slog->hdr->fptr2threadtable, SEEK_SET );
    if ( ierr != 0 ) {
        fprintf( errfile, __FILE__":SLOG_TTAB_ReadThreadInfos() - "
                          "slog_fseek( fptr2threadtable ) fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Read in the data structure  */
    ttable = slog->thread_tab;

    ierr = bswp_fread( &( ttable->Nentries ),
                       sizeof( SLOG_uint32 ), 1, slog->fd );
    if ( ierr < 1 ) {
        fprintf( errfile, __FILE__":SLOG_TTAB_ReadThreadInfos() - \n"
                          "\t""Fails at reading the number of entries "
                          "of the thread table\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
    ttable->capacity = ttable->Nentries;
    ttable->entries  = ( SLOG_threadinfo_t * )
                       malloc( ttable->capacity * sizeof( SLOG_threadinfo_t ) );
    if ( ttable->Nentries > 0 && ttable->entries == NULL ) {
        fprintf( errfile, __FILE__":SLOG_TTAB_ReadThreadInfos() - "
                          "malloc fails for ttable->entries[]\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    for ( ii = 0; ii < ttable->Nentries; ii++ ) {
        ierr = SLOG_ThInfo_ReadFromFile( &( ttable->entries[ ii ] ),
                                         slog->fd );
        if ( ierr != SLOG_TRUE ) {
            fprintf( errfile, __FILE__":SLOG_TTAB_ReadThreadInfos() - \n"
                              "\t""SLOG_ThInfo_ReadFromFile() fails at "
                              "%d-th reading of threadinfo from "
                              "the table section of logfile\n", ii );
            fflush( errfile ); 
            return SLOG_FAIL;
        }
    }

    return SLOG_SUCCESS;
}



void SLOG_TTAB_Print( FILE* fd, const SLOG_threadinfo_table_t *ttable )
{
    int                         ii;

    for ( ii = 0; ii < ttable->Nentries; ii++ ) {
        fprintf( fd, " threadinfo[%i] = ", ii );
        SLOG_ThInfo_Print( &( ttable->entries[ ii ] ), fd );
        fprintf( fd, "\n" );
    }
    fflush( fd );
}



/*  -- Component level supporting subroutines  -- */

        /* SLOG_ThInfo_xxxx methods */

void SLOG_ThInfo_Assign(       SLOG_threadinfo_t   *threadinfo,
                         const SLOG_nodeID_t        in_node_id,
                         const SLOG_threadID_t      in_thread_id,
                         const SLOG_OSprocessID_t   in_OSprocess_id,
                         const SLOG_OSthreadID_t    in_OSthread_id,
                         const SLOG_appID_t         in_app_id )
{
    threadinfo->node_id      = in_node_id;
    threadinfo->thread_id    = in_thread_id;
    threadinfo->OSprocess_id = in_OSprocess_id;
    threadinfo->OSthread_id  = in_OSthread_id;
    threadinfo->app_id       = in_app_id;
}



void SLOG_ThInfo_Print( const SLOG_threadinfo_t *thread, FILE *fd )
{
    fprintf( fd, "[ ( "fmt_nodeID_t", "fmt_thID_t" ), "
                 fmt_OSprocID_t", "fmt_OSthID_t", "fmt_appID_t" ]",
                 thread->node_id, thread->thread_id,
                 thread->OSprocess_id, thread->OSthread_id,
                 thread->app_id );
}


int SLOG_ThInfo_WriteToFile( const SLOG_threadinfo_t *thread, FILE *fd )
{
    int     ierr;

    ierr = bswp_fwrite( &( thread->node_id ),
                        sizeof( SLOG_nodeID_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    ierr = bswp_fwrite( &( thread->thread_id ),
                        sizeof( SLOG_threadID_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    ierr = bswp_fwrite( &( thread->OSprocess_id ),
                        sizeof( SLOG_OSprocessID_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    ierr = bswp_fwrite( &( thread->OSthread_id ),
                        sizeof( SLOG_OSthreadID_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    ierr = bswp_fwrite( &( thread->app_id ),
                        sizeof( SLOG_appID_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    return SLOG_TRUE;
}



int SLOG_ThInfo_ReadFromFile( SLOG_threadinfo_t *thread, FILE *fd )
{
    int     ierr;

    ierr = bswp_fread( &( thread->node_id ),
                       sizeof( SLOG_nodeID_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    ierr = bswp_fread( &( thread->thread_id ),
                       sizeof( SLOG_threadID_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;
                        
    ierr = bswp_fread( &( thread->OSprocess_id ),
                       sizeof( SLOG_OSprocessID_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;
                        
    ierr = bswp_fread( &( thread->OSthread_id ),
                       sizeof( SLOG_OSthreadID_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;
    
    ierr = bswp_fread( &( thread->app_id ),
                       sizeof( SLOG_appID_t ), 1, fd );
    if ( ierr < 1 ) return SLOG_FALSE;

    return SLOG_TRUE;
}



int SLOG_ThInfo_IsKeyEqualTo( const SLOG_threadinfo_t *thread1,
                              const SLOG_threadinfo_t *thread2 )
{
    return (    ( thread1->node_id   == thread2->node_id )
             && ( thread1->thread_id == thread2->thread_id ) );
}

int SLOG_ThInfo_IsValueEqualTo( const SLOG_threadinfo_t *thread1,
                                const SLOG_threadinfo_t *thread2 )
{
    return (    ( thread1->OSprocess_id == thread2->OSprocess_id )
             && ( thread1->OSthread_id  == thread2->OSthread_id )
             && ( thread1->app_id       == thread2->app_id ) );
}

int SLOG_ThInfo_IsEqualTo( const SLOG_threadinfo_t *thread1,
                           const SLOG_threadinfo_t *thread2 )
{
    return(    SLOG_ThInfo_IsKeyEqualTo( thread1, thread2 )
            && SLOG_ThInfo_IsValueEqualTo( thread1, thread2 ) );
}
