/* Mountain-range view for Upshot

   rf Lsttr;d
   Rf Ow554pe
   #e Iq443ow
   Ed Karrels

   Sthpmmr Msyopms; :snptsyptu
   Argonne National Laboratory

This display show how many of the processes are in a given state
at a given time. For example, if there are three states, '.', 'X',
and '*', it would look something like this:


 Processes
    10|
      |
      |
      |           .......... ...
      |        ......................   ......
     5|     ......**********.***................
      |   .....**********************...******....
      |   ..*********XXXXXXX*XXXXXXXX***XXXXXX**...
      | ..********XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX**....
     1| ..*****XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX...
       ---------------------------------------------------
         1 sec     2 sec      3 sec      4 sec      5 sec

Only in color or with a stipple pattern.

*/


#include <stdio.h>
#if !defined(HAVE_STDLIB_H)
#include <stdlib.h>
#else
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#endif
#include "tcl.h"
#include "log.h"
#include "str_dup.h"
#include "cvt_args.h"
#include "heapsort.h"
#include "tcl_callargv.h"
#include "mtn.h"



  /* length to use for temporary Tcl commands */
#define TMP_CMD_LEN 200

/*
   The function that is called by the tcl routine "mtn_c_init".
   Widget creation command.

     mtn_c_init <cmd_name> <log> <canvas> <array name> <array index prefix>
           cmd_name - command to create
           log - command for the logfile this is attached to
           canvas - name of the canvas to draw the mountain ranges on
           array name - name of the array to link to
           array index prefix - prefix of the elements in the array to use

     set mtn($w,cmd) ${w}_c_cmd
     mtn_c_init $mtn($w,cmd) $log mtn $w
        
*/
static int Init ANSI_ARGS(( ClientData, Tcl_Interp *,
		       int argc, char **argv ));

/*
   Close the mountain record.
*/
static int Close ANSI_ARGS(( mtnInfo* ));


/*
   Used by min-heap routines to compare events and sort them.
*/
static int Mtn_EvtCompare ANSI_ARGS(( mtnEvt *evt_a, mtnEvt *evt_b ));

/*
   Called on each state.  data is mtnInfo*.
*/
/*
static int GrabState ANSI_ARGS(( void *data, int idx, int type, int proc,
			    double startTime, double endTime,
			    int parent, int firstChild,
			       int overlapLevel ));
*/

/*
   Called to draw the whole thing.
*/
static int Draw ANSI_ARGS(( mtnInfo* mtn ));

/*
   Called to use 'xview' and 'yview' commands to simply slide the display
   into position when redrawing isn't necessary.
*/
static int Slide ANSI_ARGS(( mtnInfo* mtn ));


/*
   Make sure the data in the minheap has been copied to
   a sorted array.
*/
static int InitRead ANSI_ARGS(( mtnInfo *mtn ));

/* 
   Used by Draw()
*/
static int NextTimeStep ANSI_ARGS(( mtnInfo *mtn, int *ht_diffs, int nstates,
			       double interval, int *idx_ptr,
			       double *time_ptr ));

/*
   The widget command that is created for each invocation.
*/
static int Cmd ANSI_ARGS(( ClientData, Tcl_Interp *, int argc, char **argv ));


/*
   Copy existing log data into the minheap.
*/
static int CopyLogData ANSI_ARGS(( mtnInfo *mtn ));


/*
   Link my structure to Tcl array.
*/
static int LinkVars ANSI_ARGS(( mtnInfo *mtn ));


/*
   Unlink my structure from Tcl array.
*/
static int UnlinkVars ANSI_ARGS(( mtnInfo *mtn ));



/*
   Register the 'mtn_c_init' command.
*/
int Mtn_Init( interp )
Tcl_Interp *interp;
{
  Tcl_CreateCommand( interp, "mtn_c_init", Init, (ClientData)0,
		     (Tcl_CmdDeleteProc*)0 );
  return TCL_OK;
}


static int Init( clientData, interp, argc, argv )
ClientData clientData;
Tcl_Interp *interp;
int argc;
char *argv[];
{
  char *array_name, *idx_prefix;
  char *cmd_name, *log_cmd;
  char *canvas_name;
  mtnInfo *mtn;

  if (TCL_OK != ConvertArgs( interp, "mtn_c_init <cmd_name> <log> <canvas> <array name> <array index prefix>",
	       "1 sssss", argc, argv, &cmd_name, &log_cmd,
	       &canvas_name, &array_name, &idx_prefix )) {
    return TCL_ERROR;
  }

  if (!(mtn = (mtnInfo*)malloc( sizeof(mtnInfo) ))) {
    Tcl_AppendResult( interp, "Out of memory create mountain range",
		      (char*)0 );
    return TCL_ERROR;
  }

    /* convert log%d string to logfile token */
  mtn->interp = interp;
  if (!(mtn->log = LogCmd2Ptr( interp, log_cmd ))) {
    return TCL_ERROR;
  }
  
  mtn->canvasName = STRDUP( canvas_name );

    /* clear any string members */
  mtn->bg = 0;
  mtn->outline = 0;
  mtn->bw = 0;
    /* Tcl side will set bg, outline, and bw */

    /* get name of the Tcl array and array prefix I should link with */
  mtn->array_name = STRDUP( array_name );
  mtn->idx_prefix = STRDUP( idx_prefix );

    /* link members of my structure to a Tcl array */
/*
  fprintf( stderr, "&mtn->startTime = %p, &mtn->endTime = %p\n",
	   (void*)&mtn->startTime, &mtn->endTime );
*/

  mtn->startTime = Log_StartTime( mtn->log );
  mtn->endTime = Log_EndTime( mtn->log );
  mtn->totalTime = mtn->endTime - mtn->startTime;
  mtn->np = Log_Np( mtn->log );

    /* mark with invalid initial values */
  mtn->width = mtn->height = -1;
  mtn->visWidth = mtn->visHeight = mtn->lastVisWidth = mtn->lastVisHeight = -1;
  mtn->xleft = mtn->xspan = mtn->lastXspan = -1;
  mtn->ytop = mtn->yspan = mtn->lastYspan = -1;

  LinkVars( mtn );

    /* minheap used to gather all the states, when needed, they
       are pulled out into the array mtn->evtList */
  HeapCreate( mtn->evtHeap, mtnEvt, 100, Mtn_EvtCompare );
  mtn->evtList = 0;

    /* get all the data present in the logfile data structures,
       putting it in mtn->evtHeap, it will be copied into mtn->evtList
       and mtn->nevents
       when needed.  (actually, this function should put it straight
       into mtn->evtList, using a private heap if necessary) */
  CopyLogData( mtn );

    /* create a command by which the functions int mtn.tcl can
       call me directly (more convenient to put mtnInfo* in ClientData
       than in some TclPtr) */
  Tcl_CreateCommand( interp, cmd_name, Cmd, (ClientData)mtn,
		     (Tcl_CmdDeleteProc*)0 );

  return TCL_OK;
}


static int Cmd( clientData, interp, argc, argv )
ClientData clientData;
Tcl_Interp *interp;
int argc;
char *argv[];
{
  mtnInfo *mtn;

  mtn = (mtnInfo*)clientData;

  if (argc==1) {
    argv[1] = "(none)";
    goto cmdErr;
  }

  if (!strcmp( argv[1], "draw" )) {
    return Draw( mtn );
  }

  if (!strcmp( argv[1], "close" )) {
    return Close( mtn );
  }

 cmdErr:
  Tcl_AppendResult( interp, "Unrecognized command for ", argv[0], ": ",
		    argv[1], ".  Must be one of isloaded or draw.",
		    (char*)0 );
  return TCL_ERROR;
}



/*
   Compare two event times, used to keep heap sorted.
*/

static int Mtn_EvtCompare( evt_a, evt_b )
mtnEvt *evt_a, *evt_b;
{
  if (evt_a->time < evt_b->time) {
    return -1;
  } else if (evt_a->time == evt_b->time) {
    return 0;
  } else {
    return 1;
  }
}


/* Not used anymore */
/*
   Callback for gathering state info.
*/
#if 0
static int GrabState( data, idx, type, proc, startTime, endTime,
		      parent, firstChild, overlapLevel )
void *data;
int idx, type, proc, parent, firstChild, overlapLevel;
double startTime, endTime;
{
  mtnInfo *mtn;
  mtnEvt evt;

  mtn = (mtnInfo*)data;

    /* if this is the last state, draw the stuff */
  if (idx == -1) return Draw( mtn );
  
    /* add the start and end times of this state to the minheap */
  evt.time = startTime;
  evt.isStartEvt = 1;
  evt.stateNo = type;
  HeapAdd( mtn->evtHeap, mtnEvt, evt );
  evt.time = endTime;
  evt.isStartEvt = 0;
  HeapAdd( mtn->evtHeap, mtnEvt, evt );

  return TCL_OK;
}
#endif



/*
   Add any states already gathered to the minheap.
*/
static int CopyLogData( mtn )
mtnInfo *mtn;
{
  int i, n;
  int proc, parent, firstChild, overlapLevel;
  mtnEvt start, end;

  start.isStartEvt = 1;
  end.isStartEvt = 0;

  n = Log_Nstates( mtn->log );
  for (i=0; i<n; i++) {
    Log_GetState( mtn->log, i, &start.stateNo, &proc, &start.time, &end.time,
		  &parent, &firstChild, &overlapLevel );
    end.stateNo = start.stateNo;
    HeapAdd( mtn->evtHeap, mtnEvt, start );
    HeapAdd( mtn->evtHeap, mtnEvt, end );
  }

  return 0;
}


/*
   Convert time to x coordinate
*/
static double Time2X( mtn, time )
mtnInfo *mtn;
double time;
{
  return (time - mtn->startTime) * mtn->width / mtn->totalTime;
}



/*
   Convert height factor to y coordinate
*/
static double Ht2Y( mtn, ht )
mtnInfo *mtn;
int ht;
{
  return mtn->height - (ht * mtn->height / Log_Np( mtn->log ));
}



/*
   Draw the mountain range
*/
static int Draw( mtn )
mtnInfo *mtn;
{
  xpandList *coordLists;	/* one list of coords for each state type */
  int nstates, i;
  int *ht;			/* current height of each state; number */
				/* of processes in this state */
  int *ht_diffs;		/* change in height of each state at this */
				/* timestep */
  typedef char dblstr[25];
  dblstr *coordStr;		/* array of doubles converted to strings */
  char **argv;			/* for calling the canvas command manually */
  int max_n_coords;		/* most number of coordinates any state has */
  char tags[25];		/* tags string (color_%d) */
  Tcl_CmdInfo cmd_info;		/* command info (for calling manually) */
  double time;			/* time of current timestep */
  double interval;		/* interval between timesteps */
  int evt_idx;			/* index of the next event to read */
  char *cmd;			/* temporary string for building Tcl command */

  if (mtn->visWidth == -1 || mtn->visHeight == -1 ||
      mtn->xspan == -1 || mtn->yspan == -1) {
      /* Cannot draw--no dimensions yet */
    return TCL_OK;
  }

    /* If the display hasn't been stretched or shrunk or anything,
       just slide it a bit. */
  if (mtn->visWidth == mtn->lastVisWidth &&
      mtn->visHeight == mtn->lastVisHeight &&
      mtn->xspan == mtn->lastXspan &&
      mtn->yspan == mtn->lastYspan) {
    return Slide( mtn );
  }

  mtn->lastVisWidth = mtn->visWidth;
  mtn->lastVisHeight = mtn->visHeight;
  mtn->lastXspan = mtn->xspan;
  mtn->lastYspan = mtn->yspan;

  mtn->width  = mtn->visWidth  * mtn->totalTime / mtn->xspan;
  mtn->height = mtn->visHeight * mtn->np        / mtn->yspan;

    /* set the canvas scroll region to match what I want */

    /* alloc space for"%s config -scrollregion {0 0 %d %d}", mtn->canvasName */
  cmd = malloc( strlen( mtn->canvasName ) + 150 );
  sprintf( cmd, "%s config -scrollregion {0 0 %d %d}",
	   mtn->canvasName, (int)mtn->width, (int)mtn->height );
  if (Tcl_Eval( mtn->interp, cmd )!=TCL_OK) {
    free( cmd );
    return TCL_ERROR;
  }
  free( cmd );

    /* create coordinate lists and temporary working arrays */
  nstates = Log_NstateDefs( mtn->log );
  coordLists = (xpandList*) malloc( sizeof(xpandList) * nstates );
  ht = (int*) malloc( sizeof(int) * nstates );
  ht_diffs = (int*) malloc( sizeof(int) * nstates );
  for (i=0; i<nstates; i++) {
    ListCreate( coordLists[i], double, 10 );
    ht[i] = 0;
    ht_diffs[i] = 0;
  }

    /* one interval for every pixel */
  interval = mtn->totalTime / mtn->width;

  InitRead( mtn );
  evt_idx = 0;
  time = mtn->startTime;

    /* Get the next point where events take place.  If multiple events
       take place at a given point, gather them all.  Fill the
       ht_diffs array with the changes of numbers of processes
       in each states. */
  while (NextTimeStep( mtn, ht_diffs, nstates, interval, &evt_idx, &time )) {
    for (i=0; i<nstates; i++) {
        /* if a state on the bottom is increased, everyone else gets
	   shoved up, too */
      if (i) {
	ht_diffs[i] += ht_diffs[i-1];
      }
      
        /* if this state changed, add new coordinates to get to the
	   new height */
      if (ht_diffs[i]) {
	ListAddItem( coordLists[i], double, Time2X( mtn, time ) );
	ListAddItem( coordLists[i], double, Ht2Y( mtn, ht[i] ) );
	ht[i] += ht_diffs[i];
	ListAddItem( coordLists[i], double, Time2X( mtn, time ) );
	ListAddItem( coordLists[i], double, Ht2Y( mtn, ht[i] ) );
      }
    }
  }

  free( (char*)ht );
  free( (char*)ht_diffs );

    /* Cap off each coordinate list with the first coordinate,
       to make a closed polygon.  If no points have been added,
       don't cap it off. */
  max_n_coords = ListSize( coordLists[0], double );
  for (i=0; i<nstates; i++) {
    if (ListSize( coordLists[i], double )) {
      ListAddItem( coordLists[i], double,
		   ListItem( coordLists[i], double, 0 ) );
      ListAddItem( coordLists[i], double,
		   ListItem( coordLists[i], double, 1 ) );
        /* get length of longest coordinate list */
      if (ListSize( coordLists[i], double ) > max_n_coords) {
	max_n_coords = ListSize( coordLists[i], double );
      }
    }
  }

    /* allocate array of char[25]'s into which I'll convert the coordinates */
  coordStr = (dblstr*) malloc( sizeof(dblstr) * max_n_coords );
    /* allocate argv array:
          .c create polygon <n coords> -fill x -outline x -stipple x \
                 -tags x <null>= (max_n_coords) + 12 words */
  argv = (char**) malloc( sizeof(char*) * (max_n_coords + 12) );

  argv[0] = mtn->canvasName;
  argv[1] = "create";
  argv[2] = "polygon";

    /* get direct-calling info on the canvas */
  Tcl_GetCommandInfo( mtn->interp, mtn->canvasName, &cmd_info );

    /* build the command for drawing each polygon */
  for (i=nstates-1; i>=0; i--) {
    int j;
    char *name, *color, *bitmap;

      /* if there are no coords, don't draw anything */
    if (ListSize( coordLists[i], double )) {
        /* convert each coord into a string */
      for (j=0; j<ListSize( coordLists[i], double ); j++) {
	argv[j+3] = coordStr[j];
	sprintf( coordStr[j], "%f", ListItem( coordLists[i], double, j ) );
      }
        /* we don't need the list anymore */
      ListDestroy( coordLists[i], double );

      Log_GetStateDef( mtn->log, i, &name, &color, &bitmap );
      argv[j+3] = "-fill";
/*
      argv[j+5] = "-outline";
      argv[j+6] = mtn->outline;
*/
      argv[j+5] = "-tags";
      argv[j+6] = tags;
      argv[j+7] = "-stipple";
      argv[j+9] = 0;

      /*
      {
	int k;
	printf( "Creating polygon: " );
	for (k=0; k<j+9; k++) {
	  printf( " %s", argv[k] );
	}
	putchar( '\n' );
      }
      */

      /* draw background polygon */

        /* set tag to background color tag */
      sprintf( tags, "color_bg" /* , i */ );
        /* set fill to background color */
      argv[j+4] = mtn->bg;
        /* set stipple to none */
      argv[j+8] = "";
        /* draw the polygon */
      Tcl_CallArgv( cmd_info, mtn->interp, j+9, argv );

      /* draw main polygon */

        /* set tag to state color tag */
      sprintf( tags, "color_%d", i );
        /* set fill to background color */
      argv[j+4] = mtn->bw ? mtn->outline : color;
        /* set stipple to none */
      argv[j+8] = mtn->bw ? bitmap : "";
        /* draw the polygon */
      Tcl_CallArgv( cmd_info, mtn->interp, j+9, argv );
    }
  }

  free( (char*) argv );
  free( (char*) coordStr );
  free( (char*) coordLists );
  
  /*
  while (HeapSize( mtn->evtHeap, mtnEvt )) {
    HeapPop( mtn->evtHeap, mtnEvt, evt );
    if (first || time != evt.time) {
      printf( "time: %f\n", evt.time );
      first = 0;
      time = evt.time;
    }
    printf( "  state %d %s\n", evt.stateNo, 
	    evt.isStartEvt ? "start" : "end" );
  }
  */

    /* Well, everything is drawn where it should be, now slide it
       to be in the right place. */
  return Slide( mtn );
}



/*
   If the data is still in the minheap, copy it out to an array.
*/
static int InitRead( mtn )
mtnInfo *mtn;
{
  int i;

    /* if the sorted list hasn't been gathered yet, create it */
  if (!mtn->evtList) {
    mtn->nevents = HeapSize( mtn->evtHeap, mtnEvt );
    mtn->evtList = (mtnEvt*)malloc( sizeof(mtnInfo) * mtn->nevents );
    for (i=0; i < mtn->nevents ; i++) {
      HeapPop( mtn->evtHeap, mtnEvt, mtn->evtList[i] );
    }
    HeapDestroy( mtn->evtHeap, mtnEvt );
    mtn->evtHeap = 0;
  }


  return 0;
}




static int NextTimeStep( mtn, ht_diffs, nstates, interval, idx_ptr, time_ptr )
mtnInfo *mtn;
int *ht_diffs;
int nstates, *idx_ptr;
double interval, *time_ptr;
{
  double time;
  int i;

    /* clear the diff array */
  for (i=0; i<nstates; i++) {
    ht_diffs[i] = 0;
  }

  i = *idx_ptr;
  if (i >= mtn->nevents) return 0;

    /* get the time of the first event in line */
  time = mtn->evtList[i].time;

    /* keep popping more events off as long as they occurred
       within the specified interval */
  do {
    if (mtn->evtList[i].isStartEvt) {
      ht_diffs[mtn->evtList[i].stateNo]++;
    } else {
      ht_diffs[mtn->evtList[i].stateNo]--;
    }
    i++;
  } while (i < mtn->nevents &&
	   mtn->evtList[i].time <= time + interval);

  *idx_ptr = i;

/*
  printf( "Timestep %f\n", time );
  for (i=0; i<nstates; i++) {
    printf( "  [%d] %d\n", i, ht_diffs[i] );
  }
*/

  *time_ptr = time;
    
  return 1;
}



static int Slide( mtn )
mtnInfo *mtn;
{
  int xview;
  int yview;
  char *cmd;

    /* alloc space for "%s xview %d", mtn->canvasName, xview */
  cmd = malloc( strlen( mtn->canvasName ) + 50 );

  xview = (mtn->xleft - mtn->startTime) / mtn->totalTime * mtn->width;

  /* if a change in the x-position has been requested, give the canvas
     an 'xview' command to shift it */
  if (xview != mtn->xview) {
    mtn->xview = xview;
    sprintf( cmd, "%s xview %d", mtn->canvasName, xview );
    /* fprintf( stderr, "%s\n", cmd ); */
    if (Tcl_Eval( mtn->interp, cmd ) != TCL_OK) goto err;
  }


  yview = mtn->ytop / mtn->np * mtn->height;

  /* if a change in the y-position has been requested, give the canvas
     a 'yview' command to shift it */
  if (yview != mtn->yview) {
    mtn->yview = yview;
    sprintf( cmd, "%s yview %d", mtn->canvasName, yview );
    if (Tcl_Eval( mtn->interp, cmd ) != TCL_OK) goto err;
  }

  return TCL_OK;

 err:
  free( cmd );
  return TCL_ERROR;
}
    



#define LINK_ELEMENT( str_name, name, type ) \
  sprintf( tmp, "%s(%s,%s)", mtn->array_name, mtn->idx_prefix, str_name ); \
  Tcl_LinkVar( mtn->interp, tmp, (char*)&mtn->name, type ); \
/* \
  fprintf( stderr, "Linking hist->%s at address %p.\n", \
	   str_name, (char*)&hist->name ); \
*/


static int LinkVars( mtn )
mtnInfo *mtn;
{
  char *tmp;
  int i = TCL_LINK_INT;
  int f = TCL_LINK_DOUBLE;
  int s = TCL_LINK_STRING;

  /* tmp will get filled with something like "mtn(.win.0,stuff)",
     or ("%s(%s,%s)", mtn->array_name, mtn->idx_prefix, element_name ), to
     be exact.
  */
  int max_element_len = 10;  /* "canvasName" */

  tmp = malloc( strlen(mtn->array_name) + 1 + strlen(mtn->idx_prefix) + 1 +
		       max_element_len + 1 + 1 );

    /* set by Tcl */
  LINK_ELEMENT( "canvasName", canvasName, s );
  LINK_ELEMENT( "bg", bg, s );
  LINK_ELEMENT( "outline", outline, s );
  LINK_ELEMENT( "bw", bw, i );
    /* set by C */
  LINK_ELEMENT( "startTime", startTime, f );
  LINK_ELEMENT( "endTime", endTime, f );
  LINK_ELEMENT( "totalTime", totalTime, f );
  LINK_ELEMENT( "np", np, i );
  LINK_ELEMENT( "width", width, f );
  LINK_ELEMENT( "height", height, f );
  LINK_ELEMENT( "visWidth", visWidth, i );
  LINK_ELEMENT( "visHeight", visHeight, i );
  LINK_ELEMENT( "lastVisWidth", lastVisWidth, i );
  LINK_ELEMENT( "lastVisHeight", lastVisHeight, i );
  LINK_ELEMENT( "xleft", xleft, f );
  LINK_ELEMENT( "xspan", xspan, f );
  LINK_ELEMENT( "lastXspan", lastXspan, f );
  LINK_ELEMENT( "ytop", ytop, f );
  LINK_ELEMENT( "yspan", yspan, f );
  LINK_ELEMENT( "lastYspan", lastYspan, f );

  free( tmp );

  return TCL_OK;
}




#define UNLINK_ELEMENT( str_name ) \
  sprintf( tmp, "%s(%s,%s)", mtn->array_name, mtn->idx_prefix, str_name ); \
  Tcl_UnlinkVar( mtn->interp, tmp );

static int UnlinkVars( mtn )
mtnInfo *mtn;
{
  char *tmp;

  /* tmp will get filled with something like "mtn(.win.0,stuff)",
     or ("%s(%s,%s)", mtn->array_name, mtn->idx_prefix, element_name ), to
     be exact.
  */
  int max_element_len = 10;  /* "canvasName" */

  tmp = malloc( strlen(mtn->array_name) + 1 + strlen(mtn->idx_prefix) + 1 +
	        max_element_len + 1 + 1 );

  UNLINK_ELEMENT( "canvasName" );
  UNLINK_ELEMENT( "bg" );
  UNLINK_ELEMENT( "outline" );
  UNLINK_ELEMENT( "bw" );
  UNLINK_ELEMENT( "startTime" );
  UNLINK_ELEMENT( "endTime" );
  UNLINK_ELEMENT( "totalTime" );
  UNLINK_ELEMENT( "np" );
  UNLINK_ELEMENT( "width" );
  UNLINK_ELEMENT( "height" );
  UNLINK_ELEMENT( "visWidth" );
  UNLINK_ELEMENT( "visHeight" );
  UNLINK_ELEMENT( "lastVisWidth" );
  UNLINK_ELEMENT( "lastVisHeight" );
  UNLINK_ELEMENT( "xleft" );
  UNLINK_ELEMENT( "xspan" );
  UNLINK_ELEMENT( "lastXspan" );
  UNLINK_ELEMENT( "ytop" );
  UNLINK_ELEMENT( "yspan" );
  UNLINK_ELEMENT( "lastYspan" );

  free( tmp );

  return 0;
}



/*
   Close off the mountain range data structure.
*/
static int Close( mtn )
mtnInfo *mtn;
{
  UnlinkVars( mtn );

  if (mtn->canvasName) free( mtn->canvasName );
  if (mtn->bg) free( mtn->bg );
  if (mtn->outline) free( mtn->outline );
  if (mtn->array_name) free( mtn->array_name );
  if (mtn->idx_prefix) free( mtn->idx_prefix );

  if (mtn->evtHeap) ListDestroy( mtn->evtHeap, mtnEvt );
  if (mtn->evtList) free( (char*)mtn->evtList );
  free( (char*) mtn );

  return TCL_OK;
}
