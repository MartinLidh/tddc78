#if !defined ( _SLOG )
#include "slog.h"
#endif  /*  if !defined ( _SLOG )  */
#ifdef HAVE_SLOGCONF_H
#include "slog_config.h"
#endif
#if defined( HAVE_UNISTD_H )
#    include <unistd.h>
#endif


int SLOG_TTAB_IsDuplicated( const SLOG_threadinfo_table_t  *ttable,
                            const SLOG_threadinfo_t        *in_thread );

int SLOG_TTAB_Open( SLOG_STREAM  *slog );

int SLOG_TTAB_AddThreadInfo(       SLOG_STREAM       *slog,
                             const SLOG_nodeID_t      in_node_id,
                             const SLOG_threadID_t    in_thread_id,
                             const SLOG_OSprocessID_t in_OSprocess_id,
                             const SLOG_OSthreadID_t  in_OSthread_id,
                             const SLOG_appID_t       in_app_id );

int SLOG_TTAB_Close( SLOG_STREAM  *slog );

void SLOG_TTAB_Free( SLOG_threadinfo_table_t *ttable );

int SLOG_TTAB_ReadThreadInfos( SLOG_STREAM  *slog );

void SLOG_TTAB_Print( FILE* fd, const SLOG_threadinfo_table_t *ttable );



void SLOG_ThInfo_Assign(       SLOG_threadinfo_t   *threadinfo,
                         const SLOG_nodeID_t        in_node_id,
                         const SLOG_threadID_t      in_thread_id,
                         const SLOG_OSprocessID_t   in_OSprocess_id,
                         const SLOG_OSthreadID_t    in_OSthread_id,
                         const SLOG_appID_t         in_app_id );

void SLOG_ThInfo_Print( const SLOG_threadinfo_t *thread, FILE *fd );

int SLOG_ThInfo_WriteToFile( const SLOG_threadinfo_t *thread, FILE *fd );

int SLOG_ThInfo_ReadFromFile( SLOG_threadinfo_t *thread, FILE *fd );

int SLOG_ThInfo_IsKeyEqualTo( const SLOG_threadinfo_t *thread1,
                              const SLOG_threadinfo_t *thread2 );

int SLOG_ThInfo_IsValueEqualTo( const SLOG_threadinfo_t *thread1,
                                const SLOG_threadinfo_t *thread2 );

int SLOG_ThInfo_IsEqualTo( const SLOG_threadinfo_t *thread1,
                           const SLOG_threadinfo_t *thread2 );
