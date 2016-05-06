#include <stdlib.h>
#include <stdio.h>
#include "slog_bbuf.h"                 /*I  "slog_bbuf.h"  I*/

/*
    Move Intvl Rec from src_bbuf and Append it to dest_bbuf 
    through the use of src_bbuf->lptr which points at the Intvl
    Rec Node to be moved.  The routine does NOT advance the
    src_bbuf->lptr, so the calling routine should do the bookkeeping.
    After the call, the calling routine should NOT assume that 
    src_bbuf->lptr still points at anything meaningful after the call.

    src_bbuf->lptr can be set by SLOG_Bbuf_SetPointerAt()
*/
int SLOG_Bbufs_MoveIntvlRec( SLOG_intvlrec_blist_t *src_bbuf,
                             SLOG_intvlrec_blist_t *dest_bbuf )
{
          int                    ierr;

    if ( src_bbuf->lptr == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbufs_MoveIntvlRec() - "
                          "the src_bbuf->lptr is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    ierr = SLOG_Bbuf_RemoveNodeLinks( src_bbuf );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_Bbufs_MoveIntvlRec() - "
                          "SLOG_Bbuf_RemoveNodeLinks() fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    ierr = SLOG_Bbuf_AddMovedIrecNode( dest_bbuf, src_bbuf->lptr );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_Bbufs_MoveIntvlRec() - "
                          "SLOG_Bbuf_AddMovedIrecNode() fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    return ierr;
}



/*
    Copy Intvl Rec from src_bbuf and Append it to dest_bbuf
    through the use of src_bbuf->lptr which points at the Intvl
    Rec to be copied.  The routine does NOT advance the
    src_bbuf->lptr, so the calling routine should do the bookkeeping.

    src_bbuf->lptr can be set by SLOG_Bbuf_SetPointerAt()
*/
int SLOG_Bbufs_CopyIntvlRec( const SLOG_intvlrec_blist_t *src_bbuf,
                                   SLOG_intvlrec_blist_t *dest_bbuf )
{
          int                    ierr;

    if ( src_bbuf->lptr == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbufs_CopyIntvlRec() - "
                          "the src_bbuf->lptr is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( src_bbuf->lptr->irec == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbufs_CopyIntvlRec() - "
                          "the src_bbuf->lptr->irec is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    ierr = SLOG_Bbuf_AddCopiedIntvlRec( dest_bbuf, src_bbuf->lptr->irec );
    if ( ierr != SLOG_SUCCESS ) {
        fprintf( errfile, __FILE__":SLOG_Bbufs_CopyIntvlRec() - "
                          "SLOG_Bbuf_AddCopiedIntvlRec() fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }
        
    return ierr;
}



/*
    The routine modifies both the src_bbuf and dest_bbuf in order
    to achieve the process of concatenation of the 2 bbuf.
    The head of the concatenated bbuf will the the head of the dest_bbuf.
    Both the src_bbuf and dest_bbuf will be the same bbuf.

    There is no malloc() involved to create the concatenated bbuf.
    The concatenated bbuf is created from the merging the src_buff and
    dest_bbuf.
*/
int SLOG_Bbufs_Concatenate( SLOG_intvlrec_blist_t *src_bbuf,
                            SLOG_intvlrec_blist_t *dest_bbuf )
{
    if ( src_bbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbufs_Concatenate() - "
                          "the input src_bbuf is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( dest_bbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbufs_Concatenate() - "
                          "the input dest_bbuf is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( ! SLOG_Bbuf_IsConsistent( src_bbuf ) ) {
        fprintf( errfile, __FILE__":SLOG_Bbufs_Concatenate() - "
                          "the input src_bbuf is Inconsistent\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( ! SLOG_Bbuf_IsConsistent( dest_bbuf ) ) {
        fprintf( errfile, __FILE__":SLOG_Bbufs_Concatenate() - "
                          "the input dest_bbuf is Inconsistent\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    dest_bbuf->Nbytes_in_file += src_bbuf->Nbytes_in_file;
    dest_bbuf->count_irec     += src_bbuf->count_irec;
    if ( dest_bbuf->ltail != NULL )
        dest_bbuf->ltail->next     = src_bbuf->lhead;
    else
        dest_bbuf->lhead           = src_bbuf->lhead;

    if ( src_bbuf->lhead != NULL )
        src_bbuf->lhead->prev      = dest_bbuf->ltail;
    else
        src_bbuf->ltail            = dest_bbuf->ltail;

    dest_bbuf->ltail           = src_bbuf->ltail;

    src_bbuf->Nbytes_in_file   = dest_bbuf->Nbytes_in_file;
    src_bbuf->count_irec       = dest_bbuf->count_irec;
    src_bbuf->lhead            = dest_bbuf->lhead;
    
    return SLOG_SUCCESS;
}



SLOG_time 
SLOG_Bbuf_EarliestStarttime( const SLOG_intvlrec_blist_t *slog_bbuf )
{
    const SLOG_intvlrec_lptr_t  *lptr;
          SLOG_time              irec_starttime;
          SLOG_time              earliest;

    if ( slog_bbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_EarliestStarttime() - "
                          "Input slog_bbuf is NULL\n" );
        fflush( errfile );
        return SLOG_time_NULL;
    }

    if ( slog_bbuf->lhead == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_EarliestStarttime() - "
                          "Input slog_bbuf contains No element\n" );
        fflush( errfile );
        return SLOG_time_NULL;
    }

    earliest = slog_bbuf->lhead->irec->starttime;
    for ( lptr = slog_bbuf->lhead; lptr != NULL; lptr = lptr->next ) {
        irec_starttime = lptr->irec->starttime;
        if ( irec_starttime < earliest )
            earliest = irec_starttime;
    }

    return earliest;
}



SLOG_time 
SLOG_Bbuf_LatestEndtime( const SLOG_intvlrec_blist_t *slog_bbuf )
{
    const SLOG_intvlrec_lptr_t  *lptr;
          SLOG_time              irec_endtime;
          SLOG_time              latest;
    
    if ( slog_bbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_LatestEndtime() - "
                          "Input slog_bbuf is NULL\n" );
        fflush( errfile );
        return SLOG_time_NULL;
    }
    
    if ( slog_bbuf->lhead == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_LatestEndtime() - "
                          "Input slog_bbuf contains No element\n" );
        fflush( errfile );
        return SLOG_time_NULL;
    }

    latest = slog_bbuf->lhead->irec->starttime
           + slog_bbuf->lhead->irec->duration;
    for ( lptr = slog_bbuf->lhead; lptr != NULL; lptr = lptr->next ) {
        irec_endtime = lptr->irec->starttime + lptr->irec->duration; 
        if ( irec_endtime > latest )
            latest = irec_endtime;
    }

    return latest;
}



int SLOG_Bbuf_IsPointerAtNULL( const SLOG_intvlrec_blist_t *slog_bbuf )
{
    return ( slog_bbuf->lptr == NULL );
}



int SLOG_Bbuf_SetPointerAtHead( SLOG_intvlrec_blist_t *slog_bbuf )
{
    if ( slog_bbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_SetPointerAtHead() - "
                          "the input slog_bbuf is a NULL pointer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    slog_bbuf->lptr = slog_bbuf->lhead;
    return SLOG_SUCCESS;
}



int SLOG_Bbuf_SetPointerAtTail( SLOG_intvlrec_blist_t *slog_bbuf )
{
    if ( slog_bbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_SetPointerAtTail() - "
                          "the input slog_bbuf is a NULL pointer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    slog_bbuf->lptr = slog_bbuf->ltail;
    return SLOG_SUCCESS;
}



/*
    const SLOG_intvlrec_lptr_t   *lptr
*/
int SLOG_Bbuf_SetPointerAt( SLOG_intvlrec_blist_t  *slog_bbuf,
                            SLOG_intvlrec_lptr_t   *lptr )
{
    if ( slog_bbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_SetPointerAt() - "
                          "the input slog_bbuf is a NULL pointer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    slog_bbuf->lptr = lptr;
    return SLOG_SUCCESS;
}



SLOG_intvlrec_lptr_t *
SLOG_Bbuf_GetPointerAtHead( const SLOG_intvlrec_blist_t *slog_bbuf )
{
    if ( slog_bbuf != NULL )
        return slog_bbuf->lhead;
    else
        return NULL;
}



SLOG_intvlrec_lptr_t *
SLOG_Bbuf_GetPointerAtTail( const SLOG_intvlrec_blist_t *slog_bbuf )
{
    if ( slog_bbuf != NULL ) 
        return slog_bbuf->ltail;
    else
        return NULL;
}



SLOG_intvlrec_lptr_t *
SLOG_Bbuf_GetPointerAt( const SLOG_intvlrec_blist_t *slog_bbuf )
{
    if ( slog_bbuf != NULL )
        return slog_bbuf->lptr;
    else
        return NULL;
}



/*
    The routine returns a pointer to the interval record of the current
    node in the bi-directional linked list buffer given.  The routine makes no
    attempt to create new node/irec.  It just simply returns a pointer
    to the existing one in the bi-directional linked list.  If one
    wants to modify the content of the interval record without modification
    of the specified bi-directional linked, one should use
    SLOG_Irec_Copy() to copy the returned record content to a new
    record.  The routine also ADVANCES the internal pointer of the specified
    linked list buffer.
*/
const SLOG_intvlrec_t *SLOG_Bbuf_GetIntvlRec( SLOG_intvlrec_blist_t *slog_bbuf )
{
    SLOG_intvlrec_t *irec;

    if ( slog_bbuf == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_GetIntvlRec() - "
                          "the input slog_bbuf is a NULL pointer\n" );
        fflush( errfile );
        return NULL;
    }

    if ( slog_bbuf->lptr == NULL ) return NULL;

    /*  Extraction  */
    irec = slog_bbuf->lptr->irec;

    /*  Move the internal pointer to the next one  */
    slog_bbuf->lptr = slog_bbuf->lptr->next;

    return irec;
}
 


SLOG_intvlrec_lptr_t *
SLOG_Bbuf_LocateIntvlRec(       SLOG_intvlrec_blist_t *slog_bbuf,
                          const SLOG_intvlrec_t       *irec )
{
    SLOG_intvlrec_lptr_t *lptr;

    for ( lptr = slog_bbuf->lhead; lptr != NULL; lptr = lptr->next ) {
         if ( SLOG_Irec_IsEqualTo( lptr->irec, irec ) )
             return lptr;
    }

    return NULL;
}



void SLOG_Bbuf_Init( SLOG_intvlrec_blist_t *slog_bbuf )
{
    if ( slog_bbuf != NULL ) {
        slog_bbuf->Nbytes_in_file = 0;
        slog_bbuf->count_irec     = 0;
        slog_bbuf->lhead          = NULL;
        slog_bbuf->ltail          = NULL;
        slog_bbuf->lptr           = NULL;
    }
}



/*
    This routine moves( i.e. copy pointer of ) the interval record _NODE_
    to the specified slog_bbuf.  It makes no attempt to allocate any
    memory for the new interval record node and its association and argument 
    vector.  That means even if the input irec has zero bytesize in
    file.  This routine will still move the irec as requested.
    Garbage in Garbage out. 
*/
int SLOG_Bbuf_AddMovedIrecNode( SLOG_intvlrec_blist_t *slog_bbuf,
                                SLOG_intvlrec_lptr_t  *cur_node )
{ 
    SLOG_uint32            sizeof_IntvlRec_in_file;


    if ( cur_node == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_AddMovedIrecNode() - "
                          "the input irec node is a NULL pointer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( cur_node->irec == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_AddMovedIrecNode() - "
                          "the input irec node contains NULL irec pointer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Update the USED space & count in SLOG_intvlrec_blist_t  */
    sizeof_IntvlRec_in_file     = ( cur_node->irec )->bytesize;
    slog_bbuf->Nbytes_in_file  += sizeof_IntvlRec_in_file;
    slog_bbuf->count_irec++;

    /*  Attach the node to the bi-directional linked list, SLOG_BiDirBuffer  */
    cur_node->prev = slog_bbuf->ltail;
    cur_node->next = NULL;

    /* this line gurantees bi-directional linked list */
    if ( cur_node->prev != NULL )   
        /* Check if the cur_node is Blist's head, lhead->prev == NULL */
        ( cur_node->prev )->next = cur_node;            

    slog_bbuf->ltail = cur_node;

    /* this line gurantees that the lhead is pointing at the head  */
    if ( slog_bbuf->lhead == NULL )
        slog_bbuf->lhead = slog_bbuf->ltail;

    return SLOG_SUCCESS;
}



/*
    To avoid memory leak, the caller of SLOG_Bbuf_AddCopiedIntvlRec()
    _MAY_ need to deallocate the memory for intvlrec.args[].
    Deallocation is needed when copied intvlrec is temporary/local
    variable in the calling routine.
    Because SLOG_Bbuf_AddCopiedIntvlRec() calls SLOG_Irec_Copy()
    which has allocated memory for another copy of (destination)
    intvlrec.args[].  Doing so will make memory management easier, 
    since all copying are done by creation of another object with 
    identical values.  So memory cleanup can be done simply by 
    removing old ones
*/
int SLOG_Bbuf_AddCopiedIntvlRec(       SLOG_intvlrec_blist_t *slog_bbuf,
                                 const SLOG_intvlrec_t       *irec )
{
    SLOG_intvlrec_lptr_t  *cur_node;
    SLOG_uint32            sizeof_IntvlRec_in_file;


    if ( irec == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_AddCopiedIntvlRec() - "
                          "the input irec is a NULL pointer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Create & Update the node in the SLOG_BiDirBuffer  */
    cur_node = ( SLOG_intvlrec_lptr_t * ) 
               malloc( sizeof( SLOG_intvlrec_lptr_t ) );
    if ( cur_node == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_AddCopiedIntvlRec() - "
                          "malloc() fails, "
                          "Cannot allocate a node for SLOG_Bbuf\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    cur_node->irec = SLOG_Irec_Copy( irec );
    if ( cur_node->irec == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_AddCopiedIntvlRec() - "
                          "SLOG_Irec_Copy() fails\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Update the USED space & count in SLOG_intvlrec_blist_t  */
    sizeof_IntvlRec_in_file     = ( cur_node->irec )->bytesize;
    slog_bbuf->Nbytes_in_file  += sizeof_IntvlRec_in_file;
    slog_bbuf->count_irec++;

    /*  Attach the node to the bi-directional linked list, SLOG_BiDirBuffer  */
    cur_node->prev = slog_bbuf->ltail;
    cur_node->next = NULL;

    /* this line gurantees bi-directional linked list */
    if ( cur_node->prev != NULL )   
        /* Check if the cur_node is Blist's head, lhead->prev == NULL */
        ( cur_node->prev )->next = cur_node;            

    slog_bbuf->ltail = cur_node;

    /* this line gurantees that the lhead is pointing at the head  */
    if ( slog_bbuf->lhead == NULL )
        slog_bbuf->lhead = slog_bbuf->ltail;

    return SLOG_SUCCESS;
}



/*
    This routine only moves( i.e. copy pointer of ) the interval record
    to the specified slog_bbuf.  It makes no attempt to allocate any
    memory for the new interval record and its association and argument 
    vector.  That means even if the input irec has zero bytesize in
    file.  This routine will still move the irec as requested.
    Garbage in Garbage out. 

    const SLOG_intvlrec_t        *irec
*/
int SLOG_Bbuf_AddMovedIntvlRec( SLOG_intvlrec_blist_t  *slog_bbuf,
                                SLOG_intvlrec_t        *irec )
{
    SLOG_intvlrec_lptr_t  *cur_node;
    SLOG_uint32            sizeof_IntvlRec_in_file;


    if ( irec == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_AddMovedIntvlRec() - "
                          "the input irec is a NULL pointer\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    /*  Create & Update the node in the SLOG_BiDirBuffer  */
    cur_node = ( SLOG_intvlrec_lptr_t * ) 
               malloc( sizeof( SLOG_intvlrec_lptr_t ) );
    if ( cur_node == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_AddMovedIntvlRec() - "
                          "malloc() fails, "
                          "Cannot allocate a node for SLOG_Bbuf\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    cur_node->irec = irec;

    /*  Update the USED space & count in SLOG_intvlrec_blist_t  */
    sizeof_IntvlRec_in_file     = ( cur_node->irec )->bytesize;
    slog_bbuf->Nbytes_in_file  += sizeof_IntvlRec_in_file;
    slog_bbuf->count_irec++;

    /*  Attach the node to the bi-directional linked list, SLOG_BiDirBuffer  */
    cur_node->prev = slog_bbuf->ltail;
    cur_node->next = NULL;

    /* this line gurantees bi-directional linked list */
    if ( cur_node->prev != NULL )   
        /* Check if the cur_node is Blist's head, lhead->prev == NULL */
        ( cur_node->prev )->next = cur_node;            

    slog_bbuf->ltail = cur_node;

    /* this line gurantees that the lhead is pointing at the head  */
    if ( slog_bbuf->lhead == NULL )
        slog_bbuf->lhead = slog_bbuf->ltail;

    return SLOG_SUCCESS;
}



/*
    Remove or Rearrange the _prev_ and _next_ pointer links before
    removal of the node pointed by slog_bbuf->lptr
*/
int SLOG_Bbuf_RemoveNodeLinks( SLOG_intvlrec_blist_t *slog_bbuf )
{
    if ( slog_bbuf->lptr == NULL ) {
        fprintf( errfile, __FILE__":SLOG_Bbuf_RemoveNodeLinks() - "
                          "slog_bbuf->lptr is NULL\n" );
        fflush( errfile );
        return SLOG_FAIL;
    }

    if ( ( slog_bbuf->lptr )->prev != NULL )
        ( ( slog_bbuf->lptr )->prev )->next = ( slog_bbuf->lptr )->next;
    else {
        if ( ( slog_bbuf->lptr )->next != NULL ) {
            slog_bbuf->lhead = ( slog_bbuf->lptr )->next;
            ( slog_bbuf->lhead )->prev = NULL;
        }
        else
            slog_bbuf->lhead = NULL;
    }

    if ( ( slog_bbuf->lptr )->next != NULL )
        ( ( slog_bbuf->lptr )->next )->prev = ( slog_bbuf->lptr )->prev;
    else {
        if ( ( slog_bbuf->lptr )->prev != NULL ) {
            slog_bbuf->ltail = ( slog_bbuf->lptr )->prev;
            ( slog_bbuf->ltail )->next = NULL;
        }
        else
            slog_bbuf->ltail = NULL;
    }

    /*  Update the slog_bbuf's bookkeeping data  */
    slog_bbuf->Nbytes_in_file -= ( ( slog_bbuf->lptr )->irec )->bytesize;
    slog_bbuf->count_irec--;

    return SLOG_SUCCESS;
}



/*
   Free up the memory allocated to the various internal data structure
*/
void SLOG_Bbuf_FreeNode( SLOG_intvlrec_blist_t *slog_bbuf )
{
    if ( slog_bbuf->lptr != NULL ) {
        SLOG_Irec_Free( ( slog_bbuf->lptr )->irec );
        free( slog_bbuf->lptr );
    }
}



void SLOG_Bbuf_DelAllNodes( SLOG_intvlrec_blist_t *slog_bbuf )
{
    SLOG_intvlrec_lptr_t  *lptr;

    while ( slog_bbuf->ltail != NULL ) {
        lptr = slog_bbuf->ltail;
        slog_bbuf->ltail = ( slog_bbuf->ltail )->prev;
        SLOG_Irec_Free( lptr->irec );
        free( lptr );
    }

    slog_bbuf->Nbytes_in_file = 0;
    slog_bbuf->count_irec     = 0;
    slog_bbuf->lhead          = NULL;
    slog_bbuf->ltail          = NULL;
    slog_bbuf->lptr           = NULL;
    /*
    slog_bbuf->lptr = slog_bbuf->ltail;
    while ( slog_bbuf->ltail != NULL ) {
        SLOG_Bbuf_RemoveNodeLinks( slog_bbuf );
        SLOG_Bbuf_FreeNode( slog_bbuf );
        slog_bbuf->lptr = slog_bbuf->ltail;
    }
    */
}



void SLOG_Bbuf_Free( SLOG_intvlrec_blist_t *slog_bbuf )
{
    if ( slog_bbuf != NULL ) {
        SLOG_Bbuf_DelAllNodes( slog_bbuf );
        free( slog_bbuf );
        slog_bbuf = NULL;
    }
}



void SLOG_Bbuf_CopyBBufVal( const SLOG_intvlrec_blist_t *src,
                                  SLOG_intvlrec_blist_t *dest )
{
    dest->Nbytes_in_file = src->Nbytes_in_file;
    dest->count_irec     = src->count_irec;
    dest->lhead          = src->lhead;
    dest->ltail          = src->ltail;
    dest->lptr           = src->lptr;
}



int SLOG_Bbuf_IsConsistent( const SLOG_intvlrec_blist_t *slog_bbuf )
{
    int  flag1, flag2, flag3, flag4, flag;

    flag1 = ( slog_bbuf->lhead           == NULL );
    flag2 = ( slog_bbuf->ltail           == NULL );
    flag3 = ( slog_bbuf->Nbytes_in_file  == 0 );
    flag4 = ( slog_bbuf->count_irec      == 0 );

    flag  = flag1 + flag2 + flag3 + flag4;

    if ( flag == 4 || flag == 0 ) 
        return SLOG_TRUE;
    else {
        fprintf( errfile, __FILE__":SLOG_Bbuf_IsConsistent() - "
                          "Internal Inconsistency in input slog_bbuf "
                          "is detected\n" );
        fprintf( errfile, "\t""slog_bbuf->Nbytes_in_file = "fmt_ui32"\n",
                          slog_bbuf->Nbytes_in_file );
        fprintf( errfile, "\t""slog_bbuf->count_irec     = "fmt_ui32"\n",
                          slog_bbuf->count_irec );
        fprintf( errfile, "\t""slog_bbuf->lhead          = %p\n",
                          slog_bbuf->lhead );
        fprintf( errfile, "\t""slog_bbuf->ltail          = %p\n",
                          slog_bbuf->ltail );
        fflush( errfile );
        return SLOG_FALSE;
    }
}



int SLOG_Bbuf_IsEmpty( const SLOG_intvlrec_blist_t *slog_bbuf )
{
    if ( SLOG_Bbuf_IsConsistent( slog_bbuf ) ) {
        return ( slog_bbuf->count_irec == 0 );
    }
    else {
        fprintf( errfile, __FILE__":SLOG_Bbuf_IsEmpty() - "
                          "SLOG_Bbuf_IsConsistent() fails\n" );
        fflush( errfile );
        exit( 1 );
        return SLOG_FALSE;
    }
}



void SLOG_Bbuf_Print( const SLOG_intvlrec_blist_t *slog_bbuf, FILE *outfd )
{
    const SLOG_intvlrec_lptr_t  *lptr;
          int                    count;

    count = 0;
    for ( lptr = slog_bbuf->lhead; lptr != NULL; lptr = lptr->next, count++ ) {
        fprintf( outfd, "%d: ", count );
        SLOG_Irec_Print( lptr->irec, outfd );
        fprintf( outfd, "\n" );
    }
}
