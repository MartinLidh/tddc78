/*
   Alog-specific stuff for upshot
*/

#ifndef _ALOG_INT_H_
#define _ALOG_INT_H_


#include "expandingList.h"
#include "log.h"



#define ALOG_MAX_COMMENT_LEN 100
#define ALOG_PROCESS_VS_PRE_RATIO 6

#define ALOG_MAX_HEADER_EVT    -1
#define ALOG_MIN_HEADER_EVT    -100
#define ALOG_CREATOR       -1
#define ALOG_NEVENTS       -2
#define ALOG_NP            -3
#define ALOG_NTASKS        -4
#define ALOG_NEVENT_TYPES  -5
#define ALOG_START_TIME    -6
#define ALOG_END_TIME      -7
#define ALOG_NTIMER_CYCLES -8
#define ALOG_EVT_DEF       -9
#define ALOG_EVT_FORMAT    -10
#define ALOG_ROLLOVER_PT   -11
#define ALOG_UNUSED0       -12
  /* what happened to -12? */
#define ALOG_STATE_DEF     -13
 /* define msg type as -14 */
#define ALOG_PROCESS_DEF   -15
#define ALOG_SYNC          -100
#define ALOG_MESG_SEND     -101
#define ALOG_MESG_RECV     -102


typedef struct alogStateDefInfo {
  int startEvt, endEvt;
} alogStateDefInfo;

typedef struct alogStateDefData {
  xpandList /*alogStateDefInfo*/ list;
  xpandList /*int*/ startEvents;
  xpandList /*int*/ endEvents;
} alogStateDefData;

typedef struct alogEventDefData {
  xpandList /*int*/ list;
} alogEventDefData;

typedef struct alogLineData {
  int type, process, task, data, cycle;
  double timestamp;
  char comment[ALOG_MAX_COMMENT_LEN];
} alogLineData;


typedef struct alogData {
  Tcl_Interp *interp;
  logFile *log;
  int lineNo;
  char *lastLine;
  double rolloverPt;
  alogStateDefData stateDefs;
  alogEventDefData eventDefs;
  alogLineData leftOverLine;
  FILE *leftOver_fp;
} alogData;


#endif
/* _ALOG_INT_H_ */
