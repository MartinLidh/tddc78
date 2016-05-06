#if !defined ( _SLOG )
#include "slog.h"
#endif  /*  if !defined ( _SLOG )  */
#ifdef HAVE_SLOGCONF_H
#include "slog_config.h"
#endif
#if defined( HAVE_UNISTD_H )
#    include <unistd.h>
#endif



/*  Global Static methods  */
int SLOG_global_IsVarRec( const SLOG_rectype_t  rectype );

int SLOG_global_IsOffDiagRec( const SLOG_rectype_t  rectype );

/*  LOW Level API  */

void SLOG_InitGlobalData( void );

SLOG_STREAM *SLOG_OpenStream( const char *path, const char *mode );

int SLOG_CloseStream( SLOG_STREAM *slog );

int SLOG_STM_CreateBbufs( SLOG_STREAM  *slog );

void SLOG_STM_FreeBbufs( SLOG_STREAM  *slog );

int SLOG_WriteInitFrameDir( SLOG_STREAM *slog );

int SLOG_ReadFrameDir( SLOG_STREAM *slog );

int SLOG_WriteFrameDirEntryGiven(       FILE             *slog_fd,
                                  const SLOG_dir_entry_t *framedir_entry );

int SLOG_ReadFrameDirEntry( FILE             *slog_fd,
                            SLOG_dir_entry_t *framedir_entry );

int SLOG_WriteUpdatedFrameDirEntry( SLOG_STREAM  *slog );



/*  LOWER Level API  */

int SLOG_STM_IdxOfCurFrameDirEntry( const SLOG_STREAM *slog );

void SLOG_STM_Init( SLOG_STREAM *slog );

void SLOG_STM_UpdateFrameDirHdrGiven(       SLOG_STREAM       *slog,
                                      const SLOG_fptr          prevdir,
                                      const SLOG_fptr          nextdir,
                                      const SLOG_uint32        Nframe );

int SLOG_STM_IncrementFrameDirHdr( SLOG_STREAM       *slog );

int SLOG_STM_WriteFrameDirHdr( SLOG_STREAM       *slog );

int SLOG_STM_UpdateFrameDirEntry_Forward(       SLOG_STREAM  *slog,
                                          const int           IsLastFrame );

int SLOG_STM_UpdateFrameDirEntry_Backward(       SLOG_STREAM *slog,
                                           const SLOG_uint32 idx_of_cur_frame );

void SLOG_STM_UpdateFrameHdrGiven(       SLOG_STREAM  *slog,
                                   const SLOG_uint32   Nbytes_cur_i,
                                   const SLOG_uint32   Nrec_cur_i,
                                   const SLOG_uint32   Nbytes_inc_i,
                                   const SLOG_uint32   Nrec_inc_i,
                                   const SLOG_uint32   Nbytes_pas_i,
                                   const SLOG_uint32   Nrec_pas_i,
                                   const SLOG_uint32   Nbytes_out_i,
                                   const SLOG_uint32   Nrec_out_i );

void SLOG_STM_UpdateFrameHdr( SLOG_STREAM  *slog );

int SLOG_STM_DepositFrameHdr( const SLOG_framehdr_t *frame_hdr,
                                    filebuf_t       *fbuf );

int SLOG_STM_WithdrawFrameHdr( SLOG_framehdr_t *frame_hdr,
                               filebuf_t       *fbuf );

/* obsoleted */
int SLOG_STM_UpdateBBuf_endtime(       SLOG_intvlrec_blist_t *out_bbuf,
                                 const SLOG_intvlrec_blist_t *in_bbuf,
                                 const SLOG_dir_entry_t       frame_dir_entry );

int SLOG_STM_DepositBBuf( const SLOG_intvlrec_blist_t *Bbuf,
                                filebuf_t             *fbuf );

int SLOG_STM_WithdrawBBuf(       SLOG_intvlrec_blist_t *Bbuf,
                           const SLOG_recdefs_table_t  *slog_recdefs,
                                 filebuf_t             *fbuf );

int SLOG_STM_UpdateFRAME_Forward( SLOG_STREAM *slog );

int SLOG_STM_UpdateFRAME_Backward( SLOG_STREAM *slog );

int SLOG_STM_WriteFRAMEatIdx(       SLOG_STREAM *slog,
                              const SLOG_uint32  idx_of_cur_frame );

int SLOG_STM_WriteFRAME( SLOG_STREAM *slog );

int SLOG_STM_ReadFRAME( SLOG_STREAM *slog );

int SLOG_STM_ReadFRAMEatIdx(       SLOG_STREAM *slog,
                             const SLOG_uint32  idx_of_cur_frame );

int SLOG_STM_UpdateWriteFRAMEDIR( SLOG_STREAM *slog );

void SLOG_STM_InitAgainBeforeAddRec( SLOG_STREAM *slog );

void SLOG_STM_InitAgainBeforePatchFrame( SLOG_STREAM *slog );

void SLOG_STM_PrintRecDefs( const SLOG_STREAM *slog, FILE *outfd );

void SLOG_STM_PrintFrameDirHdr( const SLOG_STREAM *slog, FILE *outfd );

void SLOG_STM_PrintFrameDirEntries( const SLOG_STREAM *slog, FILE *outfd );
