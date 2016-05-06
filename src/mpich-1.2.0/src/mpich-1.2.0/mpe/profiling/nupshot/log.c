/*
   Format-unspecific logfile stuff for Upshot.
*/

#include <stdio.h>
#include "log.h"
#include "alog.h"
#include "picl.h"
#include "str_dup.h"
#include "cvt_args.h"

#ifdef NEEDS_STDLIB_PROTOTYPES
#include "protofix.h"
#endif



static int Load ANSI_ARGS(( Tcl_Interp*, logFile* ));
static int CreateCmd ANSI_ARGS(( ClientData, Tcl_Interp*, int, char** ));
static int Cmd ANSI_ARGS(( ClientData, Tcl_Interp*, int, char** ));
static logFormat FormatStr2Enum ANSI_ARGS(( char *str ));
static int ReturnDouble ANSI_ARGS(( Tcl_Interp*, double ));
static int ReturnInt ANSI_ARGS(( Tcl_Interp*, int ));



#define DEBUG 0

logFile *Log_Open( interp, cmdname, filename, format, pct_done )
Tcl_Interp *interp;
char *cmdname;
char *filename;
logFormat format;
char *pct_done;
{
  logFile *log;
  int load_status;

  log = (logFile*)malloc( sizeof( logFile ) );
  if (!log) {
    Tcl_AppendResult( interp, "out of memory opening logfile", (char*)0 );
    return 0;
  }

  log->interp = interp;
  log->format = format;
  log->filename = STRDUP( filename );
  log->cmdname = STRDUP( cmdname );
  log->creator = 0;

  Tcl_CreateCommand( interp, log->cmdname, Cmd, (ClientData)log,
		     (Tcl_CmdDeleteProc*)0 );

  log->states = State_Create();
  log->events = Event_Create();
  log->msgs   = Msg_Create();
  log->processes = Process_Create();  
  log->pct_done = pct_done;

  if (!log->filename || !log->cmdname || !log->states || !log->events ||
      !log->msgs || !log->processes) {
    Tcl_AppendResult( interp, "out of memory opening logfile", (char*)0 );
    return 0;
  }

  log->loaded = 0;
  log->halt = 0;

    /* load the logfile */
  log->is_reading = 1;
    /* Load(), or whomever it calls, should take care of opening
       and closing the FILE* */
  load_status = Load( interp, log );
  log->is_reading = 0;
  log->loaded = 1;

/* For this to be effective, the window title must NOT be set in mainwin.tcl 
  if( log->creator ) {
      int code;
      code = Tcl_VarEval(interp, "wm title . ", log->creator, (char *) NULL);
  }
  */
  if (load_status != TCL_OK) {
    Log_Close( log );
    return 0;
  }

  return log;
}



/*
   Close the logfile.  If the logfile is currently being read, just mark
   it for closing.
*/
int Log_Close( log )
logFile *log;
{

  /* if the logfile is in use being read, mark it for deletion. */
  /* the reading thread should close it when it notices */
  if (log->is_reading) {
    log->halt = 1;
    return 0;
  }

  Tcl_DeleteCommand( log->interp, log->cmdname );
  free( log->filename );
  free( log->cmdname );
  if (log->creator) free( log->creator );
  Event_Close( log->events );
  State_Close( log->states );
  Msg_Close( log->msgs );
  Process_Close( log->processes );
  free( (char*)log );

  return TCL_OK;
}


static int Load( interp, log )
Tcl_Interp *interp;
logFile *log;
{
  switch (log->format) {

  case alog_format:
  case blog_format:

    return Alog_Open( log );

  case old_picl_format:

    return Picl_Open( log );

  case new_picl_format:
  case sddf_format:
  case vt_format:
  case unknown_format:
  default:
    Tcl_AppendResult( interp, "unknown logfile format", (char*)0 );
    return TCL_ERROR;
  }
}


static logFormat FormatStr2Enum( str )
char *str;
{
  if (!strcmp( str, "alog" )) return alog_format;
  if (!strcmp( str, "picl_1" )) return old_picl_format;
  else return unknown_format;
}



static int ReturnDouble( interp, num )
Tcl_Interp *interp;
double num;
{
  Tcl_PrintDouble( interp, num, interp->result );
  return TCL_OK;
}



static int ReturnInt( interp, num )
Tcl_Interp *interp;
int num;
{
  sprintf( interp->result, "%d", num );
  return TCL_OK;
}



double Log_StartTime( log )
logFile *log;
{
  return log->starttime;
}


double Log_EndTime( log )
logFile *log;
{
  return log->endtime;
}


int Log_Np( log )
logFile *log;
{
  return log->np;
}


  /* Event-handling stuff */

int Log_NeventDefs( log )
logFile *log;
{
  return Event_Ndefs( log->events );
}

int Log_GetEventDef( log, def_num, name )
logFile *log;
int def_num;
char **name;
{
  return Event_GetDef( log->events, def_num, name );
}

int Log_SetEventDef( log, def_num, name )
logFile *log;
int def_num;
char *name;
{
  return Event_SetDef( log->events, def_num, name );
}

int Log_Nevents( log )
logFile *log;
{
  return Event_N( log->events );
}

int Log_GetEvent( log, event_num, type, proc, time )
logFile *log;
int event_num, *type, *proc;
double *time;
{
  return Event_Get( log->events, event_num, type, proc, time );
}


  /* State-handling stuff */

int Log_NstateDefs( log )
logFile *log;
{
  return State_Ndefs( log->states );
}

int Log_GetStateDef( log, def_num, name, color, bitmap )
logFile *log;
int def_num;
char **name, **color, **bitmap;
{
  return State_GetDef( log->states, def_num, name, color, bitmap );
}

int Log_SetStateDef( log, def_num, name, color, bitmap )
logFile *log;
int def_num;
char *name, *color, *bitmap;
{
  return State_SetDef( log->states, def_num, name, color, bitmap );
}

int Log_Nstates( log )
logFile *log;
{
  return State_N( log->states );
}

/*
   Return the # of instances of the given state type.
*/
int Log_NstateTypeInst( log, idx )
logFile *log;
int idx;
{
  return State_TypeNinst( log->states, idx );
}


int Log_GetState( log, state_num, type, proc, startTime, endTime,
		  parent, firstChild, overlapLevel )
logFile *log;
int state_num, *type, *proc, *parent, *firstChild, *overlapLevel;
double *startTime, *endTime;
{
  return State_Get( log->states, state_num, type, proc, startTime, endTime,
		    parent, firstChild, overlapLevel );
}


   /* Message-handling stuff */

int Log_NmsgDefs( log )
logFile *log;
{
  return Msg_Ndefs( log->msgs );
}

int Log_GetMsgDef( log, def_num, name, tag, color )
logFile *log;
int def_num, *tag;
char **name, **color;
{
  return Msg_GetDef( log->msgs, def_num, name, tag, color );
}

int Log_SetMsgDef( log, def_num, name, tag, color )
logFile *log;
int def_num, tag;
char *name, *color;
{
  return Msg_SetDef( log->msgs, def_num, name, tag, color );
}

int Log_Nmsgs( log )
logFile *log;
{
  return Msg_N( log->msgs );
}

int Log_GetMsg( log, msg_num, type, sender, recver,
		     sendTime, recvTime, size )
logFile *log;
int msg_num;
int *type, *sender, *recver, *size;
double *sendTime, *recvTime;
{
  return Msg_Get( log->msgs, msg_num, type, sender, recver,
		  sendTime, recvTime, size );
}


  /* Process-handling stuff */

int Log_GetProcessDef( log, def_num, name )
logFile *log;
int def_num;
char **name;
{
  return Process_GetDef( log->processes, def_num, name );
}


int Log_SetProcessDef( log, def_num, name )
logFile *log;
int def_num;
char *name;
{
  return Process_SetDef( log->processes, def_num, name );
}


/* Tcl hooks */


/*
   Register the 'logfile' command with the Tcl interpreter.
*/
int Log_Init( interp )
Tcl_Interp *interp;
{
  Tcl_CreateCommand( interp, "logfile", CreateCmd, (ClientData)0,
		     (Tcl_CmdDeleteProc*)0 );
  return TCL_OK;
}



/*
   This impements the Tcl 'logfile' command.  When called,
   it creates a command for using this logfile instance and loads
   the logfile.
*/
static int CreateCmd( clientData, interp, argc, argv )
ClientData clientData;
Tcl_Interp *interp;
int argc;
char **argv;
{
  logFile *log;
  logFormat format;
  char *pct_done;
  int i;
  FILE *fp;

/*
 Tcl syntax:
   logfile <cmd> <filename> <format> [opts]
*/

  if (argc < 4) {
    Tcl_AppendResult( interp, "wrong # of args: logfile <cmd> <filename> ",
		      "<format> [opts]", (char*)0 );
    return TCL_ERROR;
  }

  fp = fopen( argv[2], "r" );
  if (!fp) {
    Tcl_AppendResult( interp, "could not open ", argv[2], (char*)0 );
    return TCL_ERROR;
  }
  fclose( fp );

  format = FormatStr2Enum( argv[3] );
  if (format == unknown_format) {
    Tcl_AppendResult( interp, "unrecognized format: ", argv[3], (char*)0 );
    return TCL_ERROR;
  }

    /* look for a -pctdone option */
  pct_done = 0;
  for (i=4; i<argc && !pct_done; i++) {
    if (!strcmp( argv[i], "-pctdone" )) {
      pct_done = argv[i+1];
    }
  }

  log = Log_Open( interp, argv[1], argv[2], format, pct_done );
  if (!log) return TCL_ERROR; else return TCL_OK;
}



static int Cmd( clientData, interp, argc, argv )
ClientData clientData;
Tcl_Interp *interp;
int argc;
char **argv;
{
  logFile *log;
  int idx, tag, parent, firstChild, overlapLevel;
  int len, type, proc1, proc2, size;
  double time1, time2;
  char *name, *color, *bitmap, tmpStr[50];

  log = (logFile*)clientData;

  if (argc < 2) {
    Tcl_AppendResult( interp, "wrong # of args: ", argv[0], " <cmd> ...",
		      (char*)0 );
    return TCL_ERROR;
  }

  len = strlen(argv[1]);
  if (len<2) goto unrecognized_cmd;

  switch (argv[1][0]) {

    /* first letter of the command is 'c' */
  case 'c':
    if (strcmp( argv[1], "close" ) == 0) 
	return Log_Close( log );
    if (strcmp( argv[1], "creator" ) == 0) {
	if (log->creator) {
	    Tcl_AppendElement( interp, log->creator );
	}
	else
	    Tcl_AppendElement( interp, "" );
	return TCL_OK;
    }
    goto unrecognized_cmd;
    /* first letter of the command is 'e' */
  case 'e':
    if (strcmp( argv[1], "endtime" )) goto unrecognized_cmd;
    return ReturnDouble( interp, Log_EndTime( log ) );

    /* first letter of the command is 'n' */
  case 'n':
    switch (argv[1][1]) {

    case 'p':
      if (strcmp( argv[1], "np" )) goto unrecognized_cmd;
      return ReturnInt( interp, Log_Np( log ) );

    case 'e':
      if (len<7) goto unrecognized_cmd;
      if (argv[1][6] == 's') {
	if (strcmp( argv[1], "nevents" )) goto unrecognized_cmd;
	return ReturnInt( interp, Log_Nevents( log ) );
      } else {
	if (strcmp( argv[1], "neventdefs" )) goto unrecognized_cmd;
	return ReturnInt( interp, Log_NeventDefs( log ) );
      }

    case 's':
      if (len<7) goto unrecognized_cmd;
      switch (argv[1][6]) {
      case 'd':
	if (strcmp( argv[1], "nstatedefs" )) goto unrecognized_cmd;
	return ReturnInt( interp, Log_NstateDefs( log ) );

      case 's':
	if (strcmp( argv[1], "nstates" )) goto unrecognized_cmd;
	return ReturnInt( interp, Log_Nstates( log ) );

      case 't':
	if (strcmp( argv[1], "nstatetypeinst" )) goto unrecognized_cmd;
	if (ConvertArgs( interp, "<logfile> nstatetypeinst <idx>",
			 "2 d", argc, argv, &idx )
	    != TCL_OK) {
	  return TCL_ERROR;
	}
	return ReturnInt( interp, Log_NstateTypeInst( log, idx ) );

      default:
	goto unrecognized_cmd;
      } /* switch (argv[1][6]) */

    case 'm':
      if (len<5) goto unrecognized_cmd;
      if (argv[1][6] == 's') {
	if (strcmp( argv[1], "nmsgs" )) goto unrecognized_cmd;
	return ReturnInt( interp, Log_Nmsgs( log ) );
      } else {
	if (strcmp( argv[1], "nmsgdefs" )) goto unrecognized_cmd;
	return ReturnInt( interp, Log_NmsgDefs( log ) );
      }

    default:
      goto unrecognized_cmd;
    } /* switch (argv[1][1]) { */

    /* first letter of the command is 'g' */
  case 'g':
    if (len<4) goto unrecognized_cmd;
    switch (argv[1][3]) {

    case 'e':
      if (len<8) goto unrecognized_cmd;
      if (argv[1][8] == '\0') {
	if (strcmp( argv[1], "getevent" )) goto unrecognized_cmd;
	if (ConvertArgs( interp, "<logfile> getevent <idx>",
			 "2 d", argc, argv, &idx )
	    != TCL_OK) {
	  return TCL_ERROR;
	}
	Log_GetEvent( log, idx, &type, &proc1, &time1 );
	sprintf( tmpStr, "%d", type );
	Tcl_AppendElement( interp, tmpStr );
	sprintf( tmpStr, "%d", proc1 );
	Tcl_AppendElement( interp, tmpStr );
	Tcl_PrintDouble( interp, time1, tmpStr );
	Tcl_AppendElement( interp, tmpStr );
	return TCL_OK;
      } else {
	if (strcmp( argv[1], "geteventdef" )) goto unrecognized_cmd;
	if (ConvertArgs( interp, "<logfile> geteventdef <idx>",
			 "2 i", argc, argv, &idx )
	    != TCL_OK) {
	  return TCL_ERROR;
	}
	Log_GetEventDef( log, idx, &name );
	Tcl_AppendResult( interp, name, (char*)0 );
	return TCL_OK;
      }

    case 's':
      if (len<8) goto unrecognized_cmd;
      if (argv[1][8] == '\0') {
	if (strcmp( argv[1], "getstate" )) goto unrecognized_cmd;
	if (ConvertArgs( interp, "<logfile> getstate <idx>",
			 "2 d", argc, argv, &idx )
	    != TCL_OK) {
	  return TCL_ERROR;
	}
	Log_GetState( log, idx, &type, &proc1, &time1, &time2,
		      &parent, &firstChild, &overlapLevel );
	sprintf( tmpStr, "%d", type );
	Tcl_AppendElement( interp, tmpStr );
	sprintf( tmpStr, "%d", proc1 );
	Tcl_AppendElement( interp, tmpStr );
	Tcl_PrintDouble( interp, time1, tmpStr );
	Tcl_AppendElement( interp, tmpStr );
	Tcl_PrintDouble( interp, time2, tmpStr );
	Tcl_AppendElement( interp, tmpStr );
	sprintf( tmpStr, "%d", parent );
	Tcl_AppendElement( interp, tmpStr );
	sprintf( tmpStr, "%d", firstChild );
	Tcl_AppendElement( interp, tmpStr );
	sprintf( tmpStr, "%d", overlapLevel );
	Tcl_AppendElement( interp, tmpStr );
	return TCL_OK;
      } else {
	if (strcmp( argv[1], "getstatedef" )) goto unrecognized_cmd;
	if (ConvertArgs( interp, "<logfile> getstatedef <idx>",
			 "2 d", argc, argv, &idx )
	    != TCL_OK) {
	  return TCL_ERROR;
	}
	/* If the state is not active, don't do anythin */
	if (!Log_GetStateDef( log, idx, &name, &color, &bitmap )) {
	    Tcl_AppendElement( interp, name );
	    Tcl_AppendElement( interp, color );
	    Tcl_AppendElement( interp, bitmap );
	}
	return TCL_OK;
      }

    case 'm':
      if (len<6) goto unrecognized_cmd;
      if (argv[1][6] == '\0') {
	if (strcmp( argv[1], "getmsg" )) goto unrecognized_cmd;
	if (ConvertArgs( interp, "<logfile> getmsg <idx>",
			 "2 i", argc, argv, &idx )
	    != TCL_OK) {
	  return TCL_ERROR;
	}
	Log_GetMsg( log, idx, &type, &proc1, &proc2, &time1,
		    &time2, &size );
	sprintf( tmpStr, "%d", type );
	Tcl_AppendElement( interp, tmpStr );
	sprintf( tmpStr, "%d", proc1 );
	Tcl_AppendElement( interp, tmpStr );
	sprintf( tmpStr, "%d", proc2 );
	Tcl_AppendElement( interp, tmpStr );
	Tcl_PrintDouble( interp, time1, tmpStr );
	Tcl_AppendElement( interp, tmpStr );
	Tcl_PrintDouble( interp, time2, tmpStr );
	Tcl_AppendElement( interp, tmpStr );
	sprintf( tmpStr, "%d", size );
	Tcl_AppendElement( interp, tmpStr );
	return TCL_OK;
      } else {
	if (strcmp( argv[1], "getmsgdef" )) goto unrecognized_cmd;
	if (ConvertArgs( interp, "<logfile> getmsgdef <idx>",
			"2 i", argc, argv, &idx )
	    != TCL_OK) {
	  return TCL_ERROR;
	}
	Log_GetMsgDef( log, idx, &name, &tag, &color );
	
	Tcl_AppendElement( interp, name );
	sprintf( tmpStr, "%d", tag );
	Tcl_AppendElement( interp, tmpStr );
      }
      Tcl_AppendElement( interp, color );
      return TCL_OK;

    case 'p':
      if (strcmp( argv[1], "getprocessdef" ) == 0) {
        if (ConvertArgs( interp, "<logfile> getprocessdef <idx>",
			"2 d", argc, argv, &idx )
	    != TCL_OK) {
	  return TCL_ERROR;
        }
        Log_GetProcessDef( log, idx, &name );
        Tcl_AppendResult( interp, name, (char *) NULL ); /* not a list elt */
        return TCL_OK;
      } else
        goto unrecognized_cmd;

    default:
      goto unrecognized_cmd;
    } /* switch argv[1][3] */

    /* first letter of the command is 's' */
  case 's':

    if (argv[1][1] == 't') {
      if (strcmp( argv[1], "starttime" )) goto unrecognized_cmd;
      return ReturnDouble( interp, Log_StartTime( log ) );
    } else {
      if (len<4) goto unrecognized_cmd;
      switch (argv[1][3]) {

      case 'e':
	if (strcmp( argv[1], "seteventdef" )) goto unrecognized_cmd;
	if (ConvertArgs( interp, "<logfile> seteventdef <idx> <name>",
			 "2 is", argc, argv, &idx, &name )
	    != TCL_OK) {
	  return TCL_ERROR;
	}
	Log_SetEventDef( log, idx, name );
	return TCL_OK;

      case 's':
	if (strcmp( argv[1], "setstatedef" )) goto unrecognized_cmd;
	if (ConvertArgs( interp, "<logfile> setstatedef <idx> <name> <color> <bitmap>",
			 "2 isss", argc, argv, &idx, &name, &color,
			 &bitmap )
	    != TCL_OK) {
	  return TCL_ERROR;
	}
	Log_SetStateDef( log, idx, name, color, bitmap );
	return TCL_OK;

      case 'm':
	if (strcmp( argv[1], "setmsgdef" )) goto unrecognized_cmd;
	if (ConvertArgs( interp, "<logfile> setmsgdef <idx> <name> <tag> <color>",
			 "2 isds", argc, argv, &idx, &name, &tag, &color )
	    != TCL_OK) {
	  return TCL_ERROR;
	}
	Log_SetMsgDef( log, idx, name, tag, color );
	return TCL_OK;
  
      case 'p':
	if (strcmp( argv[1], "setprocessdef" )) goto unrecognized_cmd;
	if (ConvertArgs( interp, "<logfile> setprocessdef <idx> <name>",
			 "2 is", argc, argv, &idx, &name )
	    != TCL_OK) {
	  return TCL_ERROR;
	}
	Log_SetProcessDef( log, idx, name );
	return TCL_OK;

      default:
	goto unrecognized_cmd;
      }
    }

  default:
    goto unrecognized_cmd;
  }

 unrecognized_cmd:
  Tcl_AppendResult( interp, argv[1], "--unrecognized logfile command.  ",
		    "Must be one ",
		    "of: close, np, starttime, endtime, ",
		    "neventdefs, geteventdef, seteventdef, ",
		    "nevents, getevent, ",
		    "nstatedefs, getstatedef, setstatedef, ",
 		    "getprocessdef, setprocessdef, ",
		    "nstates, nstatetypeinst, getstate, ",
		    "nmsgdefs, getmsgdef, setmsgdef, ",
		    "nmsgs, getmsg.", (char*)0 );
  return TCL_ERROR;
}


logFile *LogCmd2Ptr( interp, cmd )
Tcl_Interp *interp;
char *cmd;
{
  Tcl_CmdInfo cmdInfo;

  Tcl_GetCommandInfo( interp, cmd, &cmdInfo );
  return (logFile*)cmdInfo.clientData;
}
