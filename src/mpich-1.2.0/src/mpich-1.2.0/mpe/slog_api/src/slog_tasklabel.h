#if !defined( _SLOG )
#include "slog.h"
#endif  /*  if ! defined ( _SLOG )  */

void SLOG_TaskID_Assign(       SLOG_tasklabel_t *task,
                         const SLOG_nodeID_t     in_node,
                         const SLOG_cpuID_t      in_cpu,
                         const SLOG_threadID_t   in_thread );

void SLOG_TaskID_Retrieve( const SLOG_tasklabel_t *task,
                                 SLOG_nodeID_t    *node,
                                 SLOG_cpuID_t     *cpu,
                                 SLOG_threadID_t  *thread );

void SLOG_TaskID_Copy(       SLOG_tasklabel_t *dest,
                       const SLOG_tasklabel_t *src );

int SLOG_TaskID_IsEqualTo( const SLOG_tasklabel_t *task1,
                           const SLOG_tasklabel_t *task2 );

void SLOG_TaskID_Print( const SLOG_tasklabel_t *task, FILE *fd );

int SLOG_TaskID_DepositToFbuf( const SLOG_tasklabel_t *task,
                                     filebuf_t        *fbuf );

int SLOG_TaskID_WithdrawFromFbuf( SLOG_tasklabel_t *task,
                                  filebuf_t        *fbuf );
