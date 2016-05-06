#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#define TESTING 0


/* Sorry about this kludge, but our ANSI C compiler on our suns has broken
   header files */
#ifdef GCC_WALL
int sscanf( char *, const char *, ... );
#endif


#include "tcl.h"
#include "events.h"
#include "states.h"
#include "msgs.h"
#include "procs.h"
#include "log.h"
#include "alog.h"
#include "alog_int.h"
#include "str_dup.h"

#ifdef NEEDS_STDLIB_PROTOTYPES
#include "protofix.h"
#endif


#define DEBUG 0

#define MAX_LINE_LEN 1024

static alogData *Create ANSI_ARGS(( logFile * ));
static int Alog_Load ANSI_ARGS(( logFile *log, alogData *alog ));
static int Alog_Close ANSI_ARGS(( logFile *log, alogData *alog ));
static int GetAlogLine ANSI_ARGS(( FILE *inf, alogLineData *lineData ));
static int StateDef ANSI_ARGS(( stateData *state_data, alogData *alog,
		     alogLineData *line_data ));
static int AlogStateDef ANSI_ARGS(( alogData *alog_data,
		         int startEvent, int endEvent ));
static int EventDef ANSI_ARGS(( eventData *event_data, alogData *alog_data,
		     int eventNum, char *eventName ));
static int AlogMsg ANSI_ARGS(( logFile *log, alogData *alog,
                    alogLineData *line_data, int type ));
static int IsStateEvent ANSI_ARGS(( alogData *alog_data, int eventType,
                         int *stateType, int *isStartEvent ));
static int AlogEventNum ANSI_ARGS(( logFile *log_data, alogData *alog_data, int type ));



/*
   Read one line from an alog-format file, returning the data in an
   alogLineData structure.  Return -1 if EOF reached.  Return -2 if
   a format error is encountered.
*/
static int GetAlogLine( inf, lineData )
FILE *inf;
alogLineData *lineData;
{
  static char line[MAX_LINE_LEN], comment[MAX_LINE_LEN];
  char *p;

  comment[0] = 0;

  /* Skip blank lines (not strictly alog, but allows comments and accidental
     additional newlines */
  while (1) {
      if (!fgets( line, MAX_LINE_LEN, inf )) {
	  return -1; /* EOF */
      }
      p = line;
      /* Skip leading blanks */
      while (*p == ' ') p++;
      /* If not at newline or comment, we're ready */
      if (*p != '\n' && *p != '#') {
	  break;
      }
  }

  if (sscanf( line, "%d %d %d %d %d %lf %[^\n]",
	      &lineData->type, &lineData->process, &lineData->task,
	      &lineData->data, &lineData->cycle, &lineData->timestamp,
	      comment ) < 6) {
    return -2;  /* format error */
  }

    /* convert from microseconds to seconds */
  lineData->timestamp *= .000001;

  strncpy( lineData->comment, comment, ALOG_MAX_COMMENT_LEN );

  return strlen(line);
}



static alogData *Create( log )
logFile *log;
{
  alogData *alog;

  alog = (alogData*)malloc( sizeof( alogData ) );
  if (!alog) goto oom;

  alog->lastLine = 0;
  alog->interp	 = log->interp;
  alog->log	 = log;

  /* create lists for alog state definitions */
  ListCreate( alog->stateDefs.list, alogStateDefInfo, 5 );
  ListCreate( alog->stateDefs.startEvents, int, 5 );
  ListCreate( alog->stateDefs.endEvents, int, 5 );

  /* create lists for alog event definitions */
  ListCreate( alog->eventDefs.list, int, 5 );

  if (!alog->stateDefs.list || !alog->stateDefs.startEvents ||
      !alog->stateDefs.endEvents || !alog->eventDefs.list) {
    goto oom;
  }

  return alog;

 oom:
  Tcl_AppendResult( alog->interp, "Out of memory open getting ready ",
		    "to open alog file", (char*)0 );
  return 0;
}




int Alog_Open( log )
logFile *log;
{
  FILE *fp;
  alogData *alog;
  alogLineData lineData;
  int readStatus, allDone, readError = 0;

  alog = Create( log );
  if (!alog) return TCL_ERROR;

  fp = fopen( log->filename, "rt" );
    /* is is assumed that the file can be read */
  alog->lineNo = 0;
  allDone = 0;

    /* read all header events */
  while (!allDone &&
	 (readStatus = GetAlogLine( fp, &lineData ))>=0 ) {
    alog->lineNo++;
    if (lineData.type > ALOG_MAX_HEADER_EVT ||
	lineData.type < ALOG_MIN_HEADER_EVT) {
      allDone = 1;
    } else {
#if DEBUG > 1
      fprintf (stderr, "Event %d\n", lineData.type );
#endif
      switch (lineData.type) {
      case ALOG_NP:
	log->np = lineData.data;
	/* initialize process name table now that we know # processes */
        Process_DataInit( log->processes, log->np );
	break;
      case ALOG_START_TIME:
	log->starttime = lineData.timestamp;
	break;
      case ALOG_END_TIME:
	log->endtime = lineData.timestamp;
	break;
      case ALOG_EVT_DEF:
	EventDef( log->events, alog, lineData.data,
		  lineData.comment );
	break;
      case ALOG_ROLLOVER_PT:
	alog->rolloverPt = lineData.timestamp;
	break;
      case ALOG_STATE_DEF:
	  if (StateDef( log->states, alog, &lineData )) {
	      readError = 1;
	  }
	break;
      case ALOG_PROCESS_DEF:
 	Process_SetDef( log->processes, lineData.process, lineData.comment );
  	break;
      case ALOG_CREATOR:
	log->creator = (char *) malloc( strlen( lineData.comment ) + 1 );
	strcpy( log->creator, lineData.comment );
        }  
    }
  }


#if TESTING
  State_PrintDefs( log_data->states );
#endif

  if (readStatus == -2 || readError) {
    char tmp[50];
    sprintf( tmp, "%d", alog->lineNo );
    Tcl_AppendResult( log->interp, "alog format error in ", log->filename,
		      ", line ", tmp, ".  Format should be '",
		      "type process task data cycle timestamp [comment]'",
		      (char*)0 );
    return TCL_ERROR;
  }

  alog->leftOverLine = lineData;
  alog->leftOver_fp = fp;

  return Alog_Load( log, alog );
}


static int Alog_Load( log, alog )
logFile *log;
alogData *alog;
{
  int firstLine;
  alogLineData lineData;
  int eventType, stateType, isStartEvent;
  FILE *inf;
/*  int isStartEvent, stateType, eventType; */

  Event_DataInit ( log->events, log->np );
  State_DataInit ( log->states, log->np );
  Msg_DataInit   ( log->msgs, log->np );
  
  firstLine = 1;
    /* get leftover line from the preprocessing phase */
  lineData = alog->leftOverLine;

    /* copy stream pointer */
  inf = alog->leftOver_fp;

  while ( !log->halt &&
	  (firstLine ||
	  GetAlogLine( inf, &lineData ) >= 0) ) {
    firstLine = 0;

#if DEBUG >1
    fprintf( stderr, "line: %d %d %d %d %f %s\n", lineData.type,
	     lineData.process,
	     lineData.task, lineData.data, lineData.timestamp,
	     lineData.comment );
#endif

    if (lineData.type == ALOG_MESG_SEND ||
	lineData.type == ALOG_MESG_RECV ) {
      if (TCL_OK != AlogMsg( log, alog, &lineData, lineData.type )) {
	return TCL_ERROR;
      }
    } else {
      if (IsStateEvent( alog, lineData.type,
		        &stateType, &isStartEvent )) {

	if (isStartEvent) {
	  State_Start( log->states, stateType, lineData.process,
		       lineData.timestamp );
	} else {
	  State_End( log->states, stateType, lineData.process,
		     lineData.timestamp );
	}

      } else {
	eventType = AlogEventNum( log, alog, lineData.type );
	Event_Add( log->events, eventType, lineData.process,
		   lineData.timestamp );
      }
    }
  }

  if (log->halt) {
    Tcl_SetResult( log->interp, "logfile loading halted", TCL_STATIC );
    Alog_Close( log, alog );
    return TCL_ERROR;
  }

#if TESTING
  State_PrintAll( log_data->states );
  Msg_PrintAll( log_data->msgs );
#endif

    /* alog's job is done.  Go home.  Get some sleep. */
  Alog_Close( log, alog );

  return 0;
}



static int Alog_Close( log, alog )
logFile *log;
alogData *alog;
{
  State_DoneAdding( log->states );
  Event_DoneAdding( log->events );
  Msg_DoneAdding  ( log->msgs );

  ListDestroy( alog->stateDefs.list, alogStateDefInfo* );
  ListDestroy( alog->stateDefs.startEvents, int );
  ListDestroy( alog->stateDefs.endEvents, int );
  ListDestroy( alog->eventDefs.list, int );
  
  free( alog );
  return 0;
}

/*
 * Colors must be a single word if no colon found
 */

static int StateDef( state_data, alog, lineData )
stateData *state_data;
alogData *alog;
alogLineData *lineData;
{
  char *color=0, *bitmap=0, *name=0;
  int stateType, isStartEvent;

  /* break the line_data->comment string up into bitmap:color name */

  if (IsStateEvent( alog, lineData->task, &stateType, &isStartEvent )) {
      /* if the state being defined is already defined, skip it */
    return 0;
  }

  bitmap = color = lineData->comment;

  while (*bitmap && *bitmap!=':') bitmap++;
  /* look for separating colon */

  if (*bitmap == 0) {
      bitmap = "gray";
      /* Move bitmap back to first blank after color */
      name   = color;
      while (*name && isspace(*name)) name++;
      while (*name && !isspace(*name)) name++;
      if (*name) 
	  *name++ = 0;
      while (*name && isspace(*name)) name++;
  }
  else {
    *bitmap = '\0';
    name = ++bitmap;
    while (*name && !isspace(*name)) name++;
      /* skip over the bitmap */

    while (*name && isspace(*name)) *name++ = '\0';
      /* fill space between bitmap and name with terminators */

    if (!*name) {
        /* if there is no name */
      char tmp[50];
      sprintf( tmp, "%d", alog->lineNo );
      Tcl_AppendResult( alog->log->interp, "Alog format error in ",
		        alog->log->filename,
		        ", line ", tmp, ".  No state name found.  Format: ",
		        "-13 0 <start event> <end event> 0 0 ",
		        "<color>:<bitmap> <state name>", (char*)0 );
      return TCL_ERROR;
    }
  }
#ifdef FOO
  } else {
      /* no colon separator */
      char tmp[50];
      sprintf( tmp, "%d", alog->lineNo );
      Tcl_AppendResult( alog->log->interp, "Alog format error in ",
		        alog->log->filename,
		        ", line ", tmp, ".  No color-bitmap separator ",
		        "found.  Format: ",
		        "-13 0 <start event> <end event> 0 0 ",
		        "<color>:<bitmap> <state name>", (char*)0 );
      return TCL_ERROR;
  }
#endif
  if (!(color && bitmap && name)) {
      /* imcomplete state info */
      char tmp[50];
      sprintf( tmp, "%d", alog->lineNo );
      Tcl_AppendResult( alog->log->interp, "Alog format error in ",
		        alog->log->filename,
		        ", line ", tmp, ".  Incomplete state description.",
		        "  Format: ",
		        "-13 0 <start event> <end event> 0 0 ",
		        "<color>:<bitmap> <state name>", (char*)0 );
      return TCL_ERROR;
  } else {
    State_AddDef( state_data, color, bitmap, name );
    AlogStateDef( alog, lineData->task, lineData->data );
  }
  return 0;
}



static int AlogStateDef( alog, startEvent, endEvent )
alogData *alog;
int startEvent, endEvent;
{
  alogStateDefInfo info;

  info.startEvt = startEvent;
  info.endEvt = endEvent;

  ListAddItem( alog->stateDefs.list, alogStateDefInfo, info );
  ListAddItem( alog->stateDefs.startEvents, int, startEvent );
  ListAddItem( alog->stateDefs.endEvents, int, endEvent );

  return 0;
}


static int IsStateEvent( alog, eventType, stateType, isStartEvent )
alogData *alog;
int eventType, *stateType, *isStartEvent;
{
  int ndefs, *startEvt, *endEvt, i;

  ndefs = ListSize( alog->stateDefs.startEvents, int );
  startEvt = ListHeadPtr( alog->stateDefs.startEvents, int );
  endEvt = ListHeadPtr( alog->stateDefs.endEvents, int );

  for (i=0; i<ndefs; i++,startEvt++,endEvt++) {
    if (*startEvt == eventType) {
        /* event is a start event */
      *stateType = i;
      *isStartEvent = 1;
      return 1;
    } else if (*endEvt == eventType) {
        /* event is an end event */
      *stateType = i;
      *isStartEvent = 0;
      return 1;
    }
  }
    /* event is not a start nor end event--standalone */
  return 0;
}


static int EventDef( event_data, alog, eventNum, eventName )
eventData *event_data;
alogData *alog;
int eventNum;
char *eventName;
{
  Event_AddDef( event_data, eventName );
  ListAddItem( alog->eventDefs.list, int, eventNum );
  return 0;
}


static int AlogEventNum( log, alog, event )
logFile *log;
alogData *alog;
int event;
{
  int nevents, i, *ptr;

  ptr = ListHeadPtr( alog->eventDefs.list, int );
  nevents = ListSize( alog->eventDefs.list, int );

    /* look for event definition */
  for (i=0; i<nevents; i++,ptr++) {
    if (*ptr == event) {
      return i;
    }
  }

    /* if not found, create new one */
  EventDef( log->events, alog, event, "" );

  return i;
}


static int AlogMsg( log, alog, line_data, type )
logFile *log;
alogData *alog;
alogLineData *line_data;
int type;
{
  int tag, size;

  if (sscanf( line_data->comment, "%d %d", &tag, &size ) != 2) {
    char tmp[50];
    sprintf( tmp, "%d", alog->lineNo );
    Tcl_AppendResult( log->interp, "Alog format error in ", log->filename,
		      ", line ", tmp, ".  Comment should contain ",
		      "message tag and size.", (char*)0 );
    return TCL_ERROR;
  }

  if (type == ALOG_MESG_SEND) {
    Msg_Send( log->msgs, line_data->process, line_data->data,
	      line_data->timestamp, tag, size );
  } else {
    Msg_Recv( log->msgs, line_data->process, line_data->data,
	      line_data->timestamp, tag, size );
  }

  return 0;
}
