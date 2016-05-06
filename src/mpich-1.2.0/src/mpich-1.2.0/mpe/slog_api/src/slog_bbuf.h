#if !defined ( _SLOG )
#include "slog.h"
#endif  /*  if !defined ( _SLOG )  */


int SLOG_Bbufs_MoveIntvlRec( SLOG_intvlrec_blist_t *src_bbuf,
                             SLOG_intvlrec_blist_t *dest_bbuf );

int SLOG_Bbufs_CopyIntvlRec( const SLOG_intvlrec_blist_t *src_bbuf,
                                   SLOG_intvlrec_blist_t *dest_bbuf );

int SLOG_Bbufs_Concatenate( SLOG_intvlrec_blist_t *src_bbuf,
                            SLOG_intvlrec_blist_t *dest_bbuf );

SLOG_time
SLOG_Bbuf_EarliestStarttime( const SLOG_intvlrec_blist_t *slog_bbuf );

SLOG_time
SLOG_Bbuf_LatestEndtime( const SLOG_intvlrec_blist_t *slog_bbuf );

int SLOG_Bbuf_IsPointerAtNULL( const SLOG_intvlrec_blist_t *slog_bbuf );

int SLOG_Bbuf_SetPointerAtHead( SLOG_intvlrec_blist_t *slog_bbuf );

int SLOG_Bbuf_SetPointerAtTail( SLOG_intvlrec_blist_t *slog_bbuf );

int SLOG_Bbuf_SetPointerAt( SLOG_intvlrec_blist_t  *slog_bbuf,
                            SLOG_intvlrec_lptr_t   *lptr );

SLOG_intvlrec_lptr_t *
SLOG_Bbuf_GetPointerAtHead( const SLOG_intvlrec_blist_t *slog_bbuf );

SLOG_intvlrec_lptr_t *
SLOG_Bbuf_GetPointerAtTail( const SLOG_intvlrec_blist_t *slog_bbuf );

SLOG_intvlrec_lptr_t *
SLOG_Bbuf_GetPointerAt( const SLOG_intvlrec_blist_t *slog_bbuf );

const SLOG_intvlrec_t *
SLOG_Bbuf_GetIntvlRec( SLOG_intvlrec_blist_t *slog_bbuf );

SLOG_intvlrec_lptr_t *
SLOG_Bbuf_LocateIntvlRec(       SLOG_intvlrec_blist_t *slog_bbuf,
                          const SLOG_intvlrec_t       *irec );

void SLOG_Bbuf_Init( SLOG_intvlrec_blist_t *slog_bbuf );

int SLOG_Bbuf_AddMovedIrecNode( SLOG_intvlrec_blist_t *slog_bbuf,
                                SLOG_intvlrec_lptr_t  *cur_node );

int SLOG_Bbuf_AddCopiedIntvlRec(       SLOG_intvlrec_blist_t *slog_bbuf,
                                 const SLOG_intvlrec_t       *irec );

int SLOG_Bbuf_AddMovedIntvlRec( SLOG_intvlrec_blist_t  *slog_bbuf,
                                SLOG_intvlrec_t        *irec );

int SLOG_Bbuf_RemoveNodeLinks( SLOG_intvlrec_blist_t *slog_bbuf );

void SLOG_Bbuf_FreeNode( SLOG_intvlrec_blist_t *slog_bbuf );

void SLOG_Bbuf_DelAllNodes( SLOG_intvlrec_blist_t *slog_bbuf );

void SLOG_Bbuf_Free( SLOG_intvlrec_blist_t *slog_bbuf );

void SLOG_Bbuf_CopyBBufVal( const SLOG_intvlrec_blist_t *src,
                                  SLOG_intvlrec_blist_t *dest );

int SLOG_Bbuf_IsConsistent( const SLOG_intvlrec_blist_t *slog_bbuf );

int SLOG_Bbuf_IsEmpty( const SLOG_intvlrec_blist_t *slog_bbuf );

void SLOG_Bbuf_Print( const SLOG_intvlrec_blist_t *slog_bbuf, FILE *outfd );
