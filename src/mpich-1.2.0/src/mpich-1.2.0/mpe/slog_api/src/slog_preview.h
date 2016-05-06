#if !defined ( _SLOG )
#include "slog.h"
#endif  /*  if !defined ( _SLOG )  */
#ifdef HAVE_SLOGCONF_H
#include "slog_config.h"
#endif
#if defined( HAVE_UNISTD_H )
#    include <unistd.h>
#endif


/* static int filecopy( FILE *infd, FILE *outfd ); */

int SLOG_SetPreviewName( SLOG_STREAM *slog, const char *prof_filename );

SLOG_pview_t *SLOG_PVIEW_Create( const char *pview_filename,
                                 const char *pview_data );

void SLOG_PVIEW_Free( SLOG_pview_t  *slog_pview );

int SLOG_PVIEW_Write( SLOG_STREAM *slog );

int SLOG_PVIEW_Read( SLOG_STREAM *slog );

void SLOG_PVIEW_Print( FILE* fd, const SLOG_pview_t *slog_pview );
