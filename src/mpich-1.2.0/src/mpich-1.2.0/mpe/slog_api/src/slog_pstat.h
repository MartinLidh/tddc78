#if !defined ( _SLOG )
#include "slog.h"
#endif  /*  if !defined ( _SLOG )  */
#ifdef HAVE_SLOGCONF_H
#include "slog_config.h"
#endif
#if defined( HAVE_UNISTD_H )
#    include <unistd.h>
#endif


int SLOG_SetPreviewStatisticsSize(       SLOG_STREAM      *slog,
                                   const SLOG_uint32      stat_bin_size );

SLOG_pstat_t *SLOG_PSTAT_Create( const SLOG_uint32   stat_set_size );

SLOG_pstat_t *SLOG_PSTAT_CreateContent(       SLOG_pstat_t *slog_pstat,
                                        const SLOG_uint32   N_stat_set );

void SLOG_PSTAT_Free( SLOG_pstat_t  *slog_pstat );

int SLOG_PSTAT_Open( SLOG_STREAM  *slog );

int SLOG_PSTAT_Init( SLOG_STREAM  *slog );

int SLOG_PSTAT_Write( SLOG_STREAM  *slog );

int SLOG_PSTAT_Read( SLOG_STREAM  *slog );

int SLOG_PSTAT_SetBinWidthInTime( SLOG_STREAM  *slog );

int SLOG_PSTAT_Update(       SLOG_pstat_t           *pstat,
                       const SLOG_intvlrec_blist_t  *src_bbuf );

int SLOG_PSTAT_Close( SLOG_STREAM  *slog );

SLOG_uint32 SLOG_PSTAT_SizeOfStatSetInFile( const SLOG_uint32  pstat_Nbin );

int SLOG_PSTAT_GetSeqIdx( const SLOG_pstat_t     *pstat, 
                          const SLOG_intvltype_t  in_intvltype );

void SLOG_PSTAT_Print( FILE *outfd, const SLOG_pstat_t  *pstat );



SLOG_statset_t *SLOG_StatSet_Create( const SLOG_uint32 stat_bin_size );

void SLOG_StatSet_Free( SLOG_statset_t *statset );

int SLOG_StatSet_SetRectype(       SLOG_statset_t   *statset,
                             const SLOG_rectype_t    in_rectype );

int SLOG_StatSet_SetIntvltype(       SLOG_statset_t   *statset,
                               const SLOG_intvltype_t  in_intvltype );

int SLOG_StatSet_SetLabel(       SLOG_statset_t   *statset,
                           const char             *in_label );

SLOG_uint32 SLOG_StatSet_ByteSizeInFile( const SLOG_statset_t   *statset );

int SLOG_StatSet_DepositToFbuf( const SLOG_statset_t   *statset,
                                      filebuf_t        *fbuf );

int SLOG_StatSet_WithdrawFromFbuf( SLOG_statset_t   *statset,
                                   filebuf_t        *fbuf );

void SLOG_StatSet_Print( FILE *outfd, const SLOG_statset_t  *statset );
