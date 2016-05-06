#if !defined ( _SLOG )
#include "slog.h"
#endif  /*  if !defined ( _SLOG )  */
#ifdef HAVE_SLOGCONF_H
#include "slog_config.h"
#endif
#if defined( HAVE_UNISTD_H )
#    include <unistd.h>
#endif

int SLOG_PROF_IsDuplicated( const SLOG_prof_t       *profile,
                            const SLOG_intvlinfo_t  *in_info );

int SLOG_PROF_Open( SLOG_STREAM  *slog );

int SLOG_PROF_AddIntvlInfo(       SLOG_STREAM      *slog,
                            const SLOG_intvltype_t  intvltype,
                            const char *            classtype,
                            const char *            label,
                            const char *            color );

int SLOG_PROF_Close( SLOG_STREAM  *slog );

int SLOG_PROF_SetExtraNumOfIntvlInfos(       SLOG_STREAM  *slog,
                                       const SLOG_uint32   Nentries_extra );

int SLOG_PROF_AddExtraIntvlInfo(       SLOG_STREAM      *slog,
                                 const SLOG_intvltype_t  intvltype,
                                 const char *            classtype,
                                 const char *            label,
                                 const char *            color );

SLOG_intvlinfo_t *SLOG_PROF_GetIntvlInfo(       SLOG_prof_t       *profile,
                                          const SLOG_intvltype_t   intvltype );

int SLOG_PROF_CompressTable( SLOG_prof_t  *profile );

int SLOG_PROF_CompressedTableToFile( SLOG_STREAM  *slog );

void SLOG_PROF_Free( SLOG_prof_t *profile );

int SLOG_PROF_ReadIntvlInfos( SLOG_STREAM  *slog );

void SLOG_PROF_Print( FILE* fd, const SLOG_prof_t *profile );



void SLOG_IntvlInfo_Assign(       SLOG_intvlinfo_t        *intvlinfo,
                            const SLOG_intvltype_t         in_intvltype,
                            const char *                   in_classtype,
                            const char *                   in_label,
                            const char *                   in_color );

void SLOG_IntvlInfo_Print( const SLOG_intvlinfo_t *info, FILE *fd );

int SLOG_IntvlInfo_WriteToFile( SLOG_intvlinfo_t *info, FILE *fd );

int SLOG_IntvlInfo_ReadFromFile( SLOG_intvlinfo_t *info, FILE *fd );



int SLOG_IntvlInfo_IsKeyEqualTo( const SLOG_intvlinfo_t *info1,
                                 const SLOG_intvlinfo_t *info2 );

int SLOG_IntvlInfo_IsValueEqualTo( const SLOG_intvlinfo_t *info1,
                                   const SLOG_intvlinfo_t *info2 );

int SLOG_IntvlInfo_IsEqualTo( const SLOG_intvlinfo_t *info1,
                              const SLOG_intvlinfo_t *info2 );

void SLOG_IntvlInfo_SetUsed(       SLOG_intvlinfo_t *info,
                             const int               boolean_flag );
