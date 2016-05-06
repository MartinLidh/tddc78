#include "clog.h"
#include "mpi.h"
#include <fcntl.h>

#define CMERGE_MAXPROCS   128
#define CMERGE_LOGBUFTYPE 777
#define MASTER_READY      801
#define SLAVE_READY       802
#define TIME_QUERY        803
#define TIME_ANSWER       804
#define CMERGE_SHIFT        1
#define CMERGE_NOSHIFT      0

void CLOG_mergelogs ANSI_ARGS(( int, char *, int ));
void CLOG_treesetup ANSI_ARGS(( int, int, int *, int *, int * ));
void CLOG_procbuf ANSI_ARGS(( double * ));
void CLOG_mergend ANSI_ARGS(( void ));
void CLOG_output ANSI_ARGS(( double * ));
void CLOG_cput ANSI_ARGS(( double ** ));
void CLOG_csync ANSI_ARGS(( int, double * ));
void CLOG_printdiffs ANSI_ARGS(( double * ));
void CLOG_reinit_buff ANSI_ARGS(( void ));
void clog2alog ANSI_ARGS(( char * ));
