/* where timestamps come from */
#include "mpi.h"

#ifdef ANSI_ARGS
#undef ANSI_ARGS
#endif

#if defined(__STDC__) || defined(__cplusplus)
#define ANSI_ARGS(x) x
#else
#define ANSI_ARGS(x) ()
#endif

void   CLOG_timeinit ANSI_ARGS(( void ));
double CLOG_timestamp ANSI_ARGS(( void ));

