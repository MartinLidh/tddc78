/*
   Logfile-handling stuff for Upshot.

   Ed Karrels
   Argonne National Laboratory
*/

/*
 

 Tcl syntax:
   logfile <cmd> <filename> <format> [opts]
      The only format allowed (right now) is alog.

    opts:
       -pctdone <percent done command>
         The logfile will call the percent-done command with two numbers
         appended to it:
           <percent done command> <total units> <units complete>

 Create a command named <cmd>, with the following syntaxes (synti?):

 <logfile> close
    close the logfile

 <logfile> starttime
    return the start time of the logfile

 <logfile> endtime
    return the end time of the logfile

 <logfile> np
    return the # of processes defined in a logfile

 <logfile> neventdefs
    return the # of event definitions

 <logfile> geteventdef <idx>
    return the definition of the indicated event type (indexed starting at 0)
    in the form {name}.

 <logfile> seteventdef <idx> <name>
    change the given event type definition

 <logfile> nevents
    return the number of events in the logfile

 <logfile> getevent <idx>
    return all the info on a given event in the form:
      {<type> <proc> <time>}

 <logfile> nstatedefs
    return the # of state type definitions

 <logfile> getstatedef <idx>
    return the definition of the indicated state type (indexed starting at 0)
    in the form {name color bitmap}.

 <logfile> setstatedef <idx> <name> <color> <bitmap>
    change the given state type definition

 <logfile> nstates
    return the number of states in the logfile

 <logfile> nstatetypeinst <idx>
    return the number of instances of a given type in the logfile

 <logfile> getstate
    return all the info on a given state in the form:
      {<type> <proc> <starttime> <endtime> <parent> <firstChild>
       <overlapLevel>}

 <logfile> nmsgdefs
    return the # of message type definitions

 <logfile> getmsgdef <idx>
    return the definition of the indicated message type (indexed starting at 0)
    in the form {name tag color}.

 <logfile> setmsgdef <idx> <name> <tag> <color>
    change the given message type definition

 <logfile> nmsgs
    return the # of messages in the logfile

 <logfile> getmsg
    return all the info on a given message in the form:
      {<type> <sender> <receiver> <sendTime> <recvTime> <size>}

 <logfile> getprocessdef <idx>
    return the definition of the indicated process type (indexed starting at 0)
    in the form {name}.

 <logfile> setprocessdef <idx> <name>
    change the given message type definition

 I would like to add:

 For adjusting for process-clock difference errors:
 <logfile> offset <proc> <time offset>
    Shift all the events of the given process by the given time offset.
    <proc> is an integer or "all", for all processes.

 <logfile> skew <proc> <fixed point> <skew>
    Stretch or squeeze all events relative to the given point.
    <skew> of >1 streches.
    <proc> is an integer or "all", for all processes.

 <logfile> ischanged
    Returns 1 if any logfile data has been changed since it was last
    saved.  Returns 0 if not.

 <logfile> write [<filename> <format>]
    Write out the logfile.  If <filename> and <format> are specified,
    write it out to that file in that format.

*/

#ifndef _LOG_H_
#define _LOG_H_

#include "tcl.h"
#include "events.h"
#include "states.h"
#include "msgs.h"
#include "procs.h"


typedef enum logFormat_ {
  unknown_format,
  alog_format,
  blog_format,
  old_picl_format,
  new_picl_format,
  sddf_format,
  vt_format
} logFormat;


typedef struct logFile {
  Tcl_Interp *interp;		/* Tcl interpreter for this logfile instance */

  logFormat format;
  char *filename;
  char *cmdname;		/* The name of the Tcl command created for */
				/* this logfile command. */

  double starttime, endtime;	/* start and end time of the logfile */
  int np;			/* number of processors */

  stateData *states;		/* info about states */
  eventData *events;		/* info about standalone events */
  msgData *msgs;		/* info about messages */
  char *creator;                /* info about the creator */
  processData *processes;	/* info about processes */
  char *pct_done;		/* percent-done widget name */
				/* The percent-done widget will be set */
				/* up before the logfile is opened */
				/* and will be closed after the initial */
				/* logfile loading.  Thus, the logfile */
				/* instance should not worry about closing */
				/* it or anything. */

  int loaded;			/* whether the logfile has been loaded yet */
  int is_reading;		/* whether the logfile is now being read */
  int halt;			/* set this if reading should cease */
} logFile;


#ifdef __STDC__
#define ARGS(x) x
#else
#define ARGS(x) ()
#endif


/*
   Register my stuff with Tcl, creating the 'logfile' command.
*/
int Log_Init ARGS(( Tcl_Interp *interp ));


/*
   Open a logfile, returning a pointer to the logfile.  If an error
   occurs, store it in interp->result and return NULL.
   Send percent-done info to the pct_done widget specified.  If
   pct_done is NULL, doesn't send percent-done info.

   Load the logfile into events, states, messages, and whatever
   else may pop up of interest.
*/
logFile *Log_Open ARGS(( Tcl_Interp *interp,
                         char *cmdname,
			 char *filename,
			 logFormat format,
			 char *pct_done ));

/* include the loading and preprocessing phases in the open() call */


/*
   Close the logfile, free memory, etc.
*/
int Log_Close ARGS(( logFile *log ));

/*
   Given the name of a logfile command, return its logFile*.
   If the command is not a vaild logfile (actually only checks if
   the command exists at all), return NULL.
*/
logFile *LogCmd2Ptr ARGS(( Tcl_Interp *interp, char *cmd ));


/*
   Note that for all the following calls, the logfile must be loaded.
*/


/*
   Return the start time of the logfile.
*/
double Log_StartTime ARGS(( logFile * ));

/*
   Return the end time of the logfile.
*/
double Log_EndTime ARGS(( logFile * ));

/*
   Return the # of processes defined in a logfile
*/
int Log_Np ARGS(( logFile * ));

/*
   Map these data collection calls to Event_, State_, Msg_, or Process_
   calls. If char*'s are returned, don't free them and don't change them.
*/
int Log_NeventDefs    ARGS(( logFile *log ));
int Log_GetEventDef   ARGS(( logFile *log, int def_num, char **name ));
int Log_SetEventDef   ARGS(( logFile *log, int def_num, char *name ));
int Log_Nevents       ARGS(( logFile *log ));
int Log_GetEvent      ARGS(( logFile *log, int n, int *type,
			     int *proc, double *time ));

int Log_NstateDefs    ARGS(( logFile *log ));
int Log_GetStateDef   ARGS(( logFile *log, int def_num, char **name,
			     char **color, char **bitmap ));
int Log_SetStateDef   ARGS(( logFile *log, int def_num, char *name,
			     char *color, char *bitmap ));
int Log_Nstates       ARGS(( logFile *log ));
int Log_NstateTypeInst ARGS(( logFile *log, int type_no ));
int Log_GetState      ARGS(( logFile *log, int n, int *type, int *proc,
			     double *startTime, double *endTime,
			     int *parent, int *firstChild,
			     int *overlapLevel ));

int Log_NmsgDefs      ARGS(( logFile *log ));
int Log_GetMsgDef     ARGS(( logFile *log, int n, char **name, int *tag,
			     char **color ));
int Log_SetMsgDef     ARGS(( logFile *log, int n, char *name, int tag,
			     char *color ));
int Log_Nmsgs         ARGS(( logFile *log ));
int Log_GetMsg        ARGS(( logFile *log, int n, int *type, int *sender,
			     int *recver, double *sendTime,
			     double *recvTime, int *size ));
int Log_GetProcessDef ARGS(( logFile *log, int n, char **name ));
int Log_SetProcessDef ARGS(( logFile *log, int n, char *name ));


#endif

