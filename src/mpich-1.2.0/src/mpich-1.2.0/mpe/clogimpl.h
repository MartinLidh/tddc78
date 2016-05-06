#ifndef _CLOGIMPL
#define _CLOGIMPL

#if defined(NEEDS_STDLIB_PROTOTYPES) && !defined ( malloc )
#include "protofix.h"
#endif

#include "clog.h"

#if defined(MPIR_MEMDEBUG)
/* Enable memory tracing.  This requires MPICH's mpid/util/tr2.c codes */
#include "mpimem.h"		/* Chameleon memory debugging stuff */
#define MALLOC(a)    MPID_trmalloc((unsigned)(a),__LINE__,__FILE__)
#define FREE(a)      MPID_trfree(a,__LINE__,__FILE__)
#else
#define MALLOC(a)    malloc(a)
#define FREE(a)      free(a)
#define MPID_trvalid(a)
#endif


void CLOG_dumplog ANSI_ARGS(( void ));
void CLOG_outblock ANSI_ARGS((double *));
void CLOG_dumpblock ANSI_ARGS(( double * ));
int  CLOG_reclen ANSI_ARGS(( int ));
void CLOG_msgtype ANSI_ARGS(( int ));
void CLOG_commtype ANSI_ARGS(( int ));
void CLOG_colltype ANSI_ARGS(( int ));
void CLOG_rectype ANSI_ARGS(( int ));

void adjust_CLOG_HEADER ANSI_ARGS(( CLOG_HEADER * ));
void adjust_CLOG_MSG ANSI_ARGS(( CLOG_MSG * ));
void adjust_CLOG_COLL ANSI_ARGS(( CLOG_COLL * ));
void adjust_CLOG_COMM ANSI_ARGS(( CLOG_COMM * ));
void adjust_CLOG_STATE ANSI_ARGS(( CLOG_STATE * ));
void adjust_CLOG_EVENT ANSI_ARGS(( CLOG_EVENT * ));
void adjust_CLOG_SRC ANSI_ARGS(( CLOG_SRC * ));
void adjust_CLOG_RAW ANSI_ARGS(( CLOG_RAW * ));

void CLOGByteSwapDouble ANSI_ARGS((double *, int));
void CLOGByteSwapInt ANSI_ARGS((int *, int));

#endif



