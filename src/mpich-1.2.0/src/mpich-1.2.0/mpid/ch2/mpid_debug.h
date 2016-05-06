#ifndef _MPID_DEBUG_H
#define _MPID_DEBUG_H
#include <stdio.h>

#ifndef ANSI_ARGS
#if defined(__STDC__) || defined(__cplusplus) || defined(HAVE_PROTOTYPES)
#define ANSI_ARGS(a) a
#else
#define ANSI_ARGS(a) ()
#endif
#endif

int  MPID_Print_mode   ANSI_ARGS(( FILE *, MPID_PKT_T * ));
int  MPID_Print_packet ANSI_ARGS(( FILE *, MPID_PKT_T * ));
void MPID_Print_pkt_data ANSI_ARGS(( char *, char *, int ));
void MPID_SetMsgDebugFlag ANSI_ARGS((int));
int  MPID_GetMsgDebugFlag ANSI_ARGS((void));
void MPID_PrintMsgDebug   ANSI_ARGS((void));
void MPID_Print_rhandle   ANSI_ARGS(( FILE *, MPIR_RHANDLE * ));
void MPID_Print_shandle   ANSI_ARGS(( FILE *, MPIR_SHANDLE * ));

#ifdef MPID_DEBUG_ALL  
/***************************************************************************/
/* This variable controls debugging output                                 */
/***************************************************************************/
extern int MPID_DebugFlag;
extern FILE *MPID_DEBUG_FILE;

/* Use these instead of printf to simplify finding stray error messages */
#ifndef FPRINTF
#define FPRINTF fprintf
#define PRINTF printf
#define SPRINTF sprintf
#endif

#ifdef MEMCPY
#undef MEMCPY
#endif
#define MEMCPY(a,b,c)\
{if (MPID_DebugFlag) {\
    FPRINTF( MPID_DEBUG_FILE, \
	    "[%d]R About to copy to %lx from %lx (n=%d) (%s:%d)...\n", \
	MPID_MyWorldRank, (long)a, (long)b, c, __FILE__, __LINE__ );\
    fflush( MPID_DEBUG_FILE ); }\
memcpy( a, b, c );}

/* Print standard send/recv args */
#define DEBUG_PRINT_ARGS(msg) \
if (MPID_DebugFlag) {\
    FPRINTF( MPID_DEBUG_FILE,\
   "[%d]%s for tag = %d, source = %d, ctx = %d, (%s:%d)\n", \
	    MPID_MyWorldRank, msg, tag, src_lrank, context_id, \
__FILE__, __LINE__ );\
    fflush( MPID_DEBUG_FILE );}

#define DEBUG_PRINT_SEND_PKT(msg,pkt)\
    {if (MPID_DebugFlag) {\
	FPRINTF( MPID_DEBUG_FILE,\
"[%d]%s of tag = %d, dest = %d, ctx = %d, len = %d, mode = ", \
	       MPID_MyWorldRank, msg, (pkt)->tag, dest, \
	       (pkt)->context_id, (pkt)->len );\
	MPID_Print_mode( MPID_DEBUG_FILE, (MPID_PKT_T *)(pkt) );\
	FPRINTF( MPID_DEBUG_FILE, "(%s:%d)\n", __FILE__, __LINE__ );\
	fflush( MPID_DEBUG_FILE );\
	}}

#define DEBUG_PRINT_BASIC_SEND_PKT(msg,pkt)\
    {if (MPID_DebugFlag) {\
	FPRINTF( MPID_DEBUG_FILE,\
		"[%d]%s ", MPID_MyWorldRank, msg );\
	MPID_Print_packet( MPID_DEBUG_FILE, \
			  (MPID_PKT_T *)(pkt) );\
	FPRINTF( MPID_DEBUG_FILE, "(%s:%d)\n", __FILE__, __LINE__ );\
	fflush( MPID_DEBUG_FILE );\
	}}

#define DEBUG_PRINT_FULL_SEND_PKT(msg,pkt)\
    {if (MPID_DebugFlag) {\
	FPRINTF( MPID_DEBUG_FILE,\
"[%d]%s of tag = %d, dest = %d, ctx = %d, len = %d, mode = ", \
	       MPID_MyWorldRank, msg, (pkt)->tag, dest, \
	       (pkt)->context_id, \
	       (pkt)->len );\
	MPID_Print_mode( MPID_DEBUG_FILE, (MPID_PKT_T *)(pkt) );\
	FPRINTF( MPID_DEBUG_FILE, "(%s:%d)\n", __FILE__, __LINE__ );\
	MPID_Print_packet( MPID_DEBUG_FILE,\
			  (MPID_PKT_T *)(pkt) );\
	fflush( MPID_DEBUG_FILE );\
	}}

#define DEBUG_PRINT_MSG(msg)\
{if (MPID_DebugFlag) {\
    FPRINTF( MPID_DEBUG_FILE, "[%d]%s (%s:%d)\n", \
	    MPID_MyWorldRank, msg, __FILE__, __LINE__ );\
    fflush( MPID_DEBUG_FILE );}}
	    
#define DEBUG_PRINT_MSG2(msg,val)\
{if (MPID_DebugFlag) {\
    char localbuf[1024]; sprintf( localbuf, msg, val );\
    DEBUG_PRINT_MSG(localbuf);}}
	    
#define DEBUG_PRINT_RECV_PKT(msg,pkt)\
    {if (MPID_DebugFlag) {\
	FPRINTF( MPID_DEBUG_FILE,\
"[%d]%s for tag = %d, source = %d, ctx = %d, len = %d, mode = ", \
	       MPID_MyWorldRank, msg, (pkt)->head.tag, from_grank, \
	       (pkt)->head.context_id, \
	       (pkt)->head.len );\
	MPID_Print_mode( MPID_DEBUG_FILE, (MPID_PKT_T *)(pkt) );\
	FPRINTF( MPID_DEBUG_FILE, "(%s:%d)\n", __FILE__, __LINE__ );\
	fflush( MPID_DEBUG_FILE );\
	}}

#define DEBUG_PRINT_FULL_RECV_PKT(msg,pkt)\
    {if (MPID_DebugFlag) {\
	FPRINTF( MPID_DEBUG_FILE,\
"[%d]%s for tag = %d, source = %d, ctx = %d, len = %d, mode = ", \
	       MPID_MyWorldRank, msg, (pkt)->head.tag, from, \
	       (pkt)->head.context_id, \
	       (pkt)->head.len );\
	MPID_Print_mode( MPID_DEBUG_FILE, (MPID_PKT_T *)(pkt) );\
	FPRINTF( MPID_DEBUG_FILE, "(%s:%d)\n", __FILE__, __LINE__ );\
	MPID_Print_packet( MPID_DEBUG_FILE,\
			  (MPID_PKT_T *)(pkt) );\
	fflush( MPID_DEBUG_FILE );\
	}}

#define DEBUG_PRINT_PKT(msg,pkt)    \
if (MPID_DebugFlag) {\
    FPRINTF( MPID_DEBUG_FILE,\
   "[%d]%s (%s:%d)\n", MPID_MyWorldRank, msg, __FILE__, __LINE__ );\
    MPID_Print_packet( MPID_DEBUG_FILE, (pkt) );\
    }

#define DEBUG_PRINT_PKT_DATA(msg,pkt)\
    if (MPID_DebugFlag) {\
	MPID_Print_pkt_data( msg, (pkt)->buffer, len );\
	}

#define DEBUG_PRINT_LONG_MSG(msg,pkt)     \
if (MPID_DebugFlag) {\
    FPRINTF( MPID_DEBUG_FILE, \
	   "[%d]S Getting data from mpid->start, first int is %d (%s:%d)\n",\
	   MPID_MyWorldRank, *(int *)mpid_send_handle->start, \
	   __FILE__, __LINE__ );\
    FPRINTF( MPID_DEBUG_FILE, "[%d]%s (%s:%d)...\n", \
	    MPID_MyWorldRank, msg, __FILE__, __LINE__ );\
    MPID_Print_packet( MPID_DEBUG_FILE, \
		      (MPID_PKT_T*)(pkt) );\
    fflush( MPID_DEBUG_FILE );\
    }

#define DEBUG_TEST_FCN(fcn,msg) {\
    if (!fcn) {\
      FPRINTF( stderr, "Bad function pointer (%s) in %s at %d\n",\
	       msg, __FILE__, __LINE__);\
      MPID_Abort( (struct MPIR_COMMUNICATOR *)0, 1, "MPI internal", "Bad function pointer" );\
      }}
/* This is pretty expensive.  It should be an option ... */
#ifdef DEBUG_INIT_MEM
#define DEBUG_INIT_STRUCT(s,size) memset(s,0xfa,size)		
#else
#define DEBUG_INIT_STRUCT(s,size)
#endif

#else
#define DEBUG_PRINT_PKT(msg,pkt)
#define DEBUG_PRINT_MSG(msg)
#define DEBUG_PRINT_MSG2(msg,val)
#define DEBUG_PRINT_ARGS(msg) 
#define DEBUG_PRINT_SEND_PKT(msg,pkt)
#define DEBUG_PRINT_BASIC_SEND_PKT(msg,pkt)
#define DEBUG_PRINT_FULL_SEND_PKT(msg,pkt)
#define DEBUG_PRINT_RECV_PKT(msg,pkt)
#define DEBUG_PRINT_FULL_RECV_PKT(msg,pkt)
#define DEBUG_PRINT_PKT_DATA(msg,pkt)
#define DEBUG_PRINT_LONG_MSG(msg,pkt)     
#define DEBUG_TEST_FCN(fcn,msg)
#define DEBUG_INIT_STRUCT(s,size)
#endif


#endif
