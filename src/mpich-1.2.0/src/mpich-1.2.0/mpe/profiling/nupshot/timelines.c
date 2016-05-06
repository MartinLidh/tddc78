#if !defined(HAVE_STDLIB_H)
#include <stdlib.h>
#else
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#endif
#include "tcl.h"
#include "tk.h"
#include "log.h"
#include "timelines.h"
#include "tclptr.h"
#include "str_dup.h"
#include "tcl_callargv.h"
#include "cvt_args.h"
#include "feather.h"


#ifdef NEEDS_STDLIB_PROTOTYPES
#include "protofix.h"
#endif


#define DEBUG 0
#define DEBUG_ZOOM 0
#define DEBUG_VIS 0
#define DEBUG_TK_DRAWS 0
#define DEBUG_UPDATE 0
#define DEBUG_UPDATE_GOOFINESS 0
#define DEBUG_NO_CONFIG 0
#define DEBUG_EVENTS 0
#define DEBUG_DESTROY 0
#define DEBUG_RESIZE 1

  /* length to use for temporary Tcl commands */
#define TMP_CMD_LEN 200

#define Pix2Time( tl, pix )  ((pix)  * (tl)->totalTime / (tl)->width \
                              + (tl)->startTime)
#define Time2Pix( tl, time ) (((time) - (tl)->startTime) / (tl)->totalTime * \
                              (tl)->width )
#define Pix2Proc( tl, pix )  ((pix)  * (tl)->np / (tl)->height)
#define Proc2Pix( tl, proc ) ((proc) / (tl)->np * (tl)->height)

/*
   Link all the variable in the tl structure to Tcl variables.
*/
static int LinkVars ANSI_ARGS(( Tcl_Interp *interp, timeLineInfo *tl ));

/*
   Link all the variable in the tl structure to Tcl variables.
*/
static int UnlinkVars ANSI_ARGS(( timeLineInfo *tl ));

/*
   Grab the ClientData from a logfile command as a logFile*.
*/
logFile *logCmd2Ptr ANSI_ARGS(( Tcl_Interp *interp, char *logCmd ));

static int SetOverlap ANSI_ARGS(( xpandList /*int*/ *list2 ));

/*
   Draw one state.
   The void *data is a timeLineInfo*.
*/
/*  Obsolete
static int TimeLineDrawState ANSI_ARGS(( void *data, int idx, int type, int proc,
				    double startTime, double endTime,
				    int parent, int firstChild,
				    int overlapLevel ));
*/


/*
   Do the C side of a new timeline initialization.
*/
static int Timeline_Open ANSI_ARGS(( ClientData  clientData, Tcl_Interp *interp,
			  int argc, char *argv[] ));

/*
   Tcl hook for calling C canvas commands.
*/
static int Cmd ANSI_ARGS(( ClientData clientData, Tcl_Interp *interp,
		      int argc, char *argv[] ));

/*
   Allocate Feather_Color's and stuff.  Needed before any drawing
   is done.  Should only be called once.
*/
static int AllocColors ANSI_ARGS(( timeLineInfo* ));


/*
   Free colors allocated by AllocColors.
*/
static int FreeColors ANSI_ARGS(( timeLineInfo* ));


/*
   Draw all states/messages/etc. that have not been drawn yet.
*/
static int Draw ANSI_ARGS(( timeLineInfo* ));


/*
   Draw background lines.
*/
static int DrawBgLines ANSI_ARGS(( timeLineInfo* ));


/*
   Draw events.
*/
static int DrawEvents ANSI_ARGS(( timeLineInfo *tl, void *canvasPtr,
			     Tk_Window canvasWin ));
/*
   Draw states.
*/
static int DrawStates ANSI_ARGS(( timeLineInfo *tl, void *canvasPtr,
			     Tk_Window canvasWin ));
/*
   Draw messages.
*/
static int DrawMsgs ANSI_ARGS(( timeLineInfo *tl, void *canvasPtr,
			   Tk_Window canvasWin ));



/*
   Get timeLineInfo* from a window name.  $timeline($win,tl)
*/
/*
static timeLineInfo *TlPtrFromWindow ANSI_ARGS(( Tcl_Interp *interp,
					    char *win ));
*/

/*
   Return descripion of current item.
*/
static int CurrentItem ANSI_ARGS(( timeLineInfo *tl ));


/*
   Passed to Tk_EventuallyFree to close the widget record.
*/
static int Destroy ANSI_ARGS(( timeLineInfo *tl ));


#if DEBUG
static int PrintStates ANSI_ARGS(( timeLineInfo* ));
#endif



#define UPDATES_EVERY_N_STATES 200



static int SegFaultCmd( clientData, interp, argc, argv )
ClientData clientData;
Tcl_Interp *interp;
int argc;
char *argv[];
{
  strcpy( (char*)0, "My baloney has a first name, it's O-S-C-A-R." );
  return 0;
}


static int UnsquishCmd( clientData, interp, argc, argv )
ClientData clientData;
Tcl_Interp *interp;
int argc;
char **argv;
{
  void *canvasPtr;
  Tk_Window canvasWin;
  
  if (argc != 2) {
    Tcl_SetResult( interp, "unsquish <canvas>", TCL_STATIC );
    return TCL_ERROR;
  }

  if (Feather_GetCanvasPtr( interp, argv[1], &canvasPtr, &canvasWin )
      != TCL_OK) {
    return TCL_ERROR;
  }

  return Feather_ReconfigArrows( canvasPtr );
}



int Timeline_Init( interp )
Tcl_Interp *interp;
{
  Tcl_CreateCommand( interp, "Timeline_C_Init", Timeline_Open,
		     (ClientData)0, (Tcl_CmdDeleteProc*) 0 );

  Tcl_CreateCommand( interp, "SegFault", SegFaultCmd,
		     (ClientData)0, (Tcl_CmdDeleteProc*) 0 );

  Tcl_CreateCommand( interp, "Timeline_C_Draw", Cmd,
		     (ClientData)0, (Tcl_CmdDeleteProc*) 0 );

  Tcl_CreateCommand( interp, "unsquish", UnsquishCmd,
		     (ClientData)0, (Tcl_CmdDeleteProc*) 0 );
  return 0;
}




static int Timeline_Open( clientData, interp, argc, argv )
ClientData clientData;
Tcl_Interp *interp;
int argc;
char *argv[];
{
  int i, n;
  timeLineInfo *tl;
  char *logCmd, *canvas_name, *array_name, *idx_prefix;
  char *cmd_name;

    /* convert the arguments from strings, put them in the widget record */
  if (TCL_OK != ConvertArgs( interp, "timeline <cmd_name> <log> <canvas> <array name> <array index prefix>", 
		   "1 sssss", argc, argv, &cmd_name, &logCmd, &canvas_name,
			     &array_name, &idx_prefix )) {
    goto failed_1;
  }

  tl = (timeLineInfo*) malloc( sizeof(timeLineInfo) );
  if (!tl) {
    Tcl_AppendResult( interp, "Failed to allocate memory for timeline ",
		      "widget record.", (char *)0 );
    return TCL_ERROR;
  }

    /* grab pointer to interpreter */
  tl->interp = interp;

    /* save copy of the canvas name */
  tl->canvasName = STRDUP( canvas_name );

  tl->array_name = STRDUP( array_name );
  tl->idx_prefix = STRDUP( idx_prefix );

    /* convert the logfile token to a string, get the logFile* */
  if (!(tl->log = LogCmd2Ptr( interp, logCmd ))) {
      /* LogCmd2Ptr sets interp->result */
    goto failed_2;
  }

  tl->width = tl->height = -1;
  tl->visWidth = tl->visHeight = -1;
  tl->lastVisWidth = tl->lastVisHeight = -1;

  tl->xleft = tl->xspan = tl->lastXspan = -1;
  tl->xview = -1;
  tl->ytop = tl->yspan = tl->lastYspan = -1;
  tl->yview = -1;

  tl->startTime = Log_StartTime( tl->log );
  tl->endTime = Log_EndTime( tl->log );
  tl->np = Log_Np( tl->log );
  tl->totalTime = tl->endTime - tl->startTime;

    /* create visiblity definitions such that everything is visible */
    /* might want to declare a special case for the visible object for
       the sake of speed in which everything is visible, no checking
       needed */
  tl->procVis = Vis_new();
  for (i=0, n=Log_Np( tl->log ); i < n; i++) {
    Vis_add( tl->procVis, i, -1 );
  }

  tl->eventVis = Vis_new();
  for (i=0, n=Log_NeventDefs( tl->log ); i < n; i++) {
    Vis_add( tl->eventVis, i, -1 );
  }

  tl->stateVis = Vis_new();
  for (i=0, n=Log_NstateDefs( tl->log ); i < n; i++) {
    Vis_add( tl->stateVis, i, -1 );
  }

  tl->msgVis = Vis_new();
  for (i=0, n=Log_NmsgDefs( tl->log ); i < n; i++) {
    Vis_add( tl->msgVis, i, -1 );
  }

    /* get the list of overlap levels, you know, how wide to draw
       a state bar if is is overlapping X other states */
  SetOverlap( &tl->overlap.halfWidths );

  tl->outlineColor = 0;
  tl->msgColor = 0;
  tl->bg = 0;
  tl->bw = 0;

    /* Mark the timelines as not being drawn yet.  Every time new data, like
       visWidth or xspan comes in, Draw() will be called.  If there is not
       enough information to draw, it will just return.  The first time
       that enough data has been provided to draw, it will be drawn.  Every
       time after that it will simply stretch or slide the canvas to
       match changes. */
  tl->isdrawn = 0;;

    /* create list for for state drawing information */
    /* don't forget we still need to add messages and events */
  ListCreate( tl->stateList, tl_stateInfo, 100 );

  tl->msgIds = 0;

    /* lists of state drawing information.  Leave empty until needed */
  tl->fillColors = 0;
  tl->fillStipples = 0;
  tl->stateTags = 0;
  tl->procTags = 0;

    /* allow Tcl to access width, height, visHeight, visWidth, xleft,
       xspan, ytop, yspan,
       bg, outlineColor, msgColor, bw, startTime, totalTime, and endTime */
  LinkVars( interp, tl );

    /* create a command by which the functions int timelines.tcl can
       call me directly (more convenient to put timeLineInfo* in ClientData
       than in some TclPtr) */
  Tcl_CreateCommand( interp, cmd_name, Cmd, (ClientData)tl,
		     (Tcl_CmdDeleteProc*)0 );

    /* like, wow, no errors! */
  return TCL_OK;

 failed_2:
  free( tl->windowName );
  free( tl->canvasName );
  free( tl );
 failed_1:
  return TCL_ERROR;
}



#define LINK_ELEMENT( str_name, name, type ) \
  sprintf( tmp, "%s(%s,%s)", tl->array_name, tl->idx_prefix, str_name ); \
  Tcl_LinkVar( interp, tmp, (char*)&tl->name, type );
/*
  fprintf( stderr, "Linking tl->%s at address %p.\n", \
	   str_name, (char*)&tl->name );
*/


static int LinkVars( interp, tl )
Tcl_Interp *interp;
timeLineInfo *tl;
{
  char *tmp;
  int i = TCL_LINK_INT;
  int f = TCL_LINK_DOUBLE;
  int s = TCL_LINK_STRING;

  /* tmp will get filled with something like "timeline(.win.0,stuff)",
     or ("%s(%s,%s)", array_name, idx_prefix, element_name ), to
     be exact.
  */

  int max_element_len = 12;  /* "windowUnits" */

  tmp = malloc( strlen(tl->array_name) + 1 + strlen(tl->idx_prefix) + 1 +
	        max_element_len + 1 + 1 );

  tl->lineColor = 0;
  tl->outlineColor = 0;
  tl->msgColor = 0;

  LINK_ELEMENT( "width", width, f );
  LINK_ELEMENT( "height", height, f );
  LINK_ELEMENT( "visWidth", visWidth, i );
  LINK_ELEMENT( "visHeight", visHeight, i );
  LINK_ELEMENT( "xleft", xleft, f );
  LINK_ELEMENT( "xspan", xspan, f );
  LINK_ELEMENT( "ytop", ytop, f );
  LINK_ELEMENT( "yspan", yspan, f );
  LINK_ELEMENT( "bg", bg, s );
  LINK_ELEMENT( "lineColor", lineColor, s );
  LINK_ELEMENT( "outlineColor", outlineColor, s );
  LINK_ELEMENT( "msgColor", msgColor, s );
  LINK_ELEMENT( "bw", bw, i );
  LINK_ELEMENT( "startTime", startTime, f );
  LINK_ELEMENT( "totalTime", totalTime, f );
  LINK_ELEMENT( "endTime", endTime, f );
  LINK_ELEMENT( "np", np, i );

  free( tmp );

  return 0;
}


#define UNLINK_ELEMENT( str_name ) \
  sprintf( tmp, "%s(%s,%s)", tl->array_name, tl->idx_prefix, str_name ); \
  Tcl_UnlinkVar( tl->interp, tmp );

static int UnlinkVars( tl )
timeLineInfo *tl;
{
  char *tmp;

  /* tmp will get filled with something like "tl(.win.0,stuff)",
     or ("%s(%s,%s)", tl->array_name, tl->idx_prefix, element_name ), to
     be exact.
  */
  int max_element_len = 12;  /* "outlineColor" */

  tmp = malloc( strlen(tl->array_name) + 1 + strlen(tl->idx_prefix) + 1 +
	        max_element_len + 1 + 1 );

  UNLINK_ELEMENT( "width" );
  UNLINK_ELEMENT( "height" );
  UNLINK_ELEMENT( "visWidth" );
  UNLINK_ELEMENT( "visHeight" );
  UNLINK_ELEMENT( "xleft" );
  UNLINK_ELEMENT( "xspan" );
  UNLINK_ELEMENT( "ytop" );
  UNLINK_ELEMENT( "yspan" );
  UNLINK_ELEMENT( "bg" );
  UNLINK_ELEMENT( "lineColor" );
  UNLINK_ELEMENT( "outlineColor" );
  UNLINK_ELEMENT( "msgColor" );
  UNLINK_ELEMENT( "bw" );
  UNLINK_ELEMENT( "startTime" );
  UNLINK_ELEMENT( "totalTime" );
  UNLINK_ELEMENT( "endTime" );
  UNLINK_ELEMENT( "np" );

  free( tmp );

  return 0;
}



#if 0
static timeLineInfo *TlPtrFromToken( interp, token )
Tcl_Interp *interp;
char *token;
{
  timeLineInfo *tl;

  tl = (timeLineInfo*) GetTclPtr( atoi( token ) );
  if (tl) {
    return tl;
  } else {
    Tcl_AppendResult( interp, token, " -- invalid timeline token",
		     (char *)0 );
    return 0;
  }
}


static timeLineInfo *TlPtrFromWindow( interp, win )
Tcl_Interp *interp;
char *win;
{
  char *tmp;
  char *tl_str;

  tmp = (char*)malloc( strlen(win) + 4 );
  sprintf( tmp, "%s,tl", win );
  tl_str = Tcl_GetVar2( interp, "timeline", tmp, TCL_GLOBAL_ONLY );
  free( tmp );

  if (tl_str) {
    return TlPtrFromToken( interp, tl_str );
  } else {
    Tcl_AppendResult( interp, "timeline(", win, ",tl) doesn't exist.",
		      (char *)0 );
    return 0;
  }
}
#endif



static int Destroy( tl )
timeLineInfo *tl;
{
  ListDestroy( tl->stateList, tl_stateInfo );

  UnlinkVars( tl );

  if (tl->bg) free( tl->bg );
  if (tl->outlineColor) free( tl->outlineColor );
  if (tl->msgColor) free( tl->msgColor );
  if (tl->canvasName) free( tl->canvasName );
  if (tl->array_name) free( tl->array_name );
  if (tl->idx_prefix) free( tl->idx_prefix );

  Vis_close( tl->procVis );
  Vis_close( tl->eventVis );
  Vis_close( tl->stateVis );
  Vis_close( tl->msgVis );
  tl->procVis = tl->eventVis = tl->stateVis = tl->msgVis = 0;

     /* free colors allocated for this display */
  FreeColors( tl );

  free( (char*)tl );			/* malloc()'d */

  return TCL_OK;
}



/*
   Overlap levels are a percentage - 0 to 100, how much of the timeline's
   allocated region it will use up.
*/

static int SetOverlap( list2 )
xpandList *list2;
{
  xpandList /*int*/ list;

  ListCreate( list, int, 4 );
  ListAddItem( list, int, 80 );
  ListAddItem( list, int, 60 );
  ListAddItem( list, int, 40 );

  *list2 = list;

  return 0;
}



static int Cmd( clientData, interp, argc, argv )
ClientData clientData;
Tcl_Interp *interp;
int argc;
char *argv[];
{
  timeLineInfo *tl;

  tl = (timeLineInfo*)clientData;

  if (!strcmp( argv[1], "draw" )) {
    if (argc != 2) {
      Tcl_AppendResult( interp, "wrong # of args: ", argv[0], " draw",
		        (char*)0 );
      return TCL_ERROR;
    }

    return Draw( tl );

  } else if (!strcmp( argv[1], "destroy" )) {
    if (argc != 2) {
      Tcl_AppendResult( interp, "wrong # of args: ", argv[0], " destroy",
		        (char*)0 );
      return TCL_ERROR;
    }

    return Destroy( tl );

  } else if (!strcmp( argv[1], "currentitem" )) {
    if (argc != 2) {
      Tcl_AppendResult( interp, "wrong # of args: ", argv[0], " currentitem",
		        (char*)0 );
      return TCL_ERROR;
    }

    return CurrentItem( tl );

  } else {
    Tcl_AppendResult( interp, argv[1], " -- unknown command for timeline",
		      (char*)0 );
    return TCL_ERROR;
  }
}



/*
   Slide the canvas with xview and yview commands so that it is
   in the correct position.
*/
int Slide( tl )
timeLineInfo *tl;
{
  char cmd[200];
  int xview, yview;

  xview = (tl->xleft - tl->startTime) / tl->totalTime * tl->width;

  if (xview != tl->xview) {
    tl->xview = xview;
    sprintf( cmd, "%s xview %d", tl->canvasName, xview );
    if (Tcl_Eval( tl->interp, cmd ) != TCL_OK) return TCL_ERROR;
  }

  yview = tl->ytop / tl->np * tl->height;

  if (yview != tl->yview) {
    tl->yview = yview;
    sprintf( cmd, "%s yview %d", tl->canvasName, yview );
    if (Tcl_Eval( tl->interp, cmd ) != TCL_OK) return TCL_ERROR;
  }

  return TCL_OK;
}



/*
   Stretch the canvas to fit the display settings requested.
*/
int Stretch( tl )
timeLineInfo *tl;
{
  char cmd[200];
  double width, height;
  
  width =  tl->visWidth  * tl->totalTime / tl->xspan;
  height = tl->visHeight * tl->np        / tl->yspan;

  if (width != tl->width || height != tl->height) {
      /* unsquish calls Feather_ReconfigArrows, which unsquishes any
	 arrows that got squished by the scale */
    sprintf( cmd, "%s scale all 0 0 %.17g %.17g; unsquish %s", tl->canvasName,
	     width / tl->width, height / tl->height, tl->canvasName );
    if (Tcl_Eval( tl->interp, cmd ) != TCL_OK) return TCL_ERROR;
    sprintf( cmd, "%s config -scrollregion {0 0 %.17g %.17g}",
	     tl->canvasName, width, height );
    if (Tcl_Eval( tl->interp, cmd ) != TCL_OK) return TCL_ERROR;
    tl->width = width;
    tl->height = height;
  }

  return Slide( tl );
}






static int Draw( tl )
timeLineInfo *tl;
{
  Tk_Window canvasWin;
  void *canvasPtr;
  char cmd[200];

  if (TCL_OK != Feather_GetCanvasPtr( tl->interp, tl->canvasName, &canvasPtr,
				      &canvasWin )) {
    return TCL_ERROR;
  }

  if (tl->visWidth == -1 || tl->visHeight == -1 ||
      tl->xspan == -1 || tl->yspan == -1) {
      /* Cannot draw--no dimensions yet */
    return TCL_OK;
  }

    /* If the display has already been drawn and hasn't been stretched or
       anything, just slide it a bit. */
  if (tl->isdrawn &&
      tl->visWidth == tl->lastVisWidth &&
      tl->visHeight == tl->lastVisHeight &&
      tl->xspan == tl->lastXspan &&
      tl->yspan == tl->lastYspan) {
    return Slide( tl );
  }

  tl->lastVisWidth = tl->visWidth;
  tl->lastVisHeight = tl->visHeight;
  tl->lastXspan = tl->xspan;
  tl->lastYspan = tl->yspan;

    /* if it's already been drawn, I'll just stretch it to fit. */
  if (tl->isdrawn) {
    return Stretch( tl );
  }

  /* well, looks like we'll actually have to draw it. */

    /* set the canvas scroll region to match what I want */
  tl->width  = tl->visWidth  * tl->totalTime / tl->xspan;
  tl->height = tl->visHeight * tl->np        / tl->yspan;

  sprintf( cmd, "%s config -scrollregion {0 0 %.17g %.17g}",
	   tl->canvasName, tl->width, tl->height );
  if (Tcl_Eval( tl->interp, cmd ) != TCL_OK) return TCL_ERROR;

    /* allocate Feather_Colors and stuff */
  AllocColors( tl );

  if (DrawBgLines( tl ) != TCL_OK ||
      DrawEvents( tl, canvasPtr, canvasWin ) != TCL_OK ||
      DrawStates( tl, canvasPtr, canvasWin ) != TCL_OK ||
      DrawMsgs( tl, canvasPtr, canvasWin ) != TCL_OK) {
    return TCL_ERROR;
  }

  tl->isdrawn = 1;

    /* make sure everyone is in the right position */
  return Slide( tl );
}



/*
   Draw the lines that all the state bars sit on.
*/
static int DrawBgLines( tl )
timeLineInfo *tl;
{
  int i;
  double y;
  char cmd[200];

  for (i=0; i<tl->np; i++) {
    y = Proc2Pix( tl, i+.5 );
    sprintf( cmd, "%s create line 0 %f %f %f -fill %s -tags {%s}",
		  tl->canvasName, y, tl->width, y,
		  tl->lineColor, "color_timeline" );
    if (Tcl_Eval( tl->interp, cmd ) != TCL_OK) return TCL_ERROR;
  }

  return TCL_OK;
}


  



static int DrawEvents( tl, canvasPtr, canvasWin )
timeLineInfo *tl;
void *canvasPtr;
Tk_Window canvasWin;
{

/*
  printf( "%d events to draw\n", Log_Nevents( tl->log ) );

  n = Log_Nevents( tl->log );
  for (i=tl->lastEventDrawn; i<n; i++) {}
  tl->lastEventDrawn = i;
*/
  
  return TCL_OK;
}



/*
   The Feather_CreateRect() command wants allocated colors and bitmaps
   and stuff.  Allocate them once with this procedure.
*/
static int AllocColors( tl )
timeLineInfo *tl;
{
  int nstates, np, i;
  char *name, *color, *bitmap;
  char tag[50];

  nstates = Log_NstateDefs( tl->log );
  np = Log_Np( tl->log );

  tl->fillColors =
    (Feather_Color*) malloc( sizeof(Feather_Color) * nstates );
  tl->fillStipples =
    (Feather_Bitmap*) malloc( sizeof(Feather_Bitmap) * nstates );
  tl->stateTags =
    (Tk_Uid*) malloc( sizeof(Tk_Uid) * nstates );
  tl->colorTags =
    (Tk_Uid*) malloc( sizeof(Tk_Uid) * nstates );
  tl->procTags =
    (Tk_Uid*) malloc( sizeof(Tk_Uid) * np );

  Feather_GetCanvasPtr( tl->interp, tl->canvasName,
		        &tl->canvasPtr, &tl->canvasWin );

  tl->outlineCol = Feather_GetColor( tl->interp, tl->canvasWin, None,
				     Tk_GetUid( tl->outlineColor ) );

  tl->bgCol = Feather_GetColor( tl->interp, tl->canvasWin, None,
				Tk_GetUid( tl->bg ) );

  tl->msgCol = Feather_GetColor( tl->interp, tl->canvasWin, None,
				 Tk_GetUid( tl->msgColor ) );

  tl->colorBgTag =      Tk_GetUid( "color_bg" );
  tl->colorOutlineTag = Tk_GetUid( "color_outline" );
  tl->colorArrowTag =   Tk_GetUid( "color_arrow" );

  for (i=0; i<nstates; i++) {
    Log_GetStateDef( tl->log, i, &name, &color, &bitmap );
    sprintf( tag, "state_%d", i );
    tl->stateTags[i] = Tk_GetUid( tag );
    sprintf( tag, "color_%d", i );
    tl->colorTags[i] = Tk_GetUid( tag );
    tl->fillColors[i] = Feather_GetColor( tl->interp, tl->canvasWin, None,
					  Tk_GetUid( color ) );
    tl->fillStipples[i] = Feather_GetBitmap( tl->interp, tl->canvasWin,
					     Tk_GetUid( bitmap ) );
  }

  for (i=0; i<np; i++) {
    sprintf( tag, "proc_%d", i );
    tl->procTags[i] = Tk_GetUid( tag );
  }

  return TCL_OK;
}


/*
   Free colors and stuff allocated in AllocColor.
*/
static int FreeColors( tl )
timeLineInfo *tl;
{
  int nstates, i;

  nstates = Log_NstateDefs( tl->log );

  Feather_FreeColor( tl->outlineCol );
  Feather_FreeColor( tl->bgCol );
  Feather_FreeColor( tl->msgCol );

  for (i=0; i<nstates; i++) {
    Feather_FreeColor( tl->fillColors[i] );
    Feather_FreeBitmap( tl->fillStipples[i] );
  }
  
  return 0;
}



static int DrawStates( tl, canvasPtr, canvasWin )
timeLineInfo *tl;
void *canvasPtr;
Tk_Window canvasWin;
{
  Feather_RectInfo rectInfo;
  unsigned long rectMask;
  int n, id,  i;
  double x1, y1, x2, y2;
  Tk_Uid tags[3];
  tl_stateInfo state_info;

    /* state info */
  int type, proc, parent, firstChild, overlapLevel;
  double startTime, endTime;
    
/*
  printf( "%d states to draw\n", Log_Nstates( tl->log ) );
*/

  rectInfo.tagList = tags;

  n = Log_Nstates( tl->log );
  for (i=0; i<n; i++) {
    Log_GetState( tl->log, i, &type, &proc, &startTime, &endTime,
		  &parent, &firstChild, &overlapLevel );

      /* set horizontal start and end points */
    x1 = Time2Pix( tl, startTime );
    x2 = Time2Pix( tl, endTime );

      /* set the top and bottom coordinates of the bar*/
      /* the overlap.halfWidth is a number from 0 to 100; how much
	 of the alloted space to fill up */
    y1 = Proc2Pix( tl, proc + .5 * (100 -
      ListItem( tl->overlap.halfWidths, int, overlapLevel ))/100.0 );
    y2 = Proc2Pix( tl, proc + .5 + .5 *
      ListItem( tl->overlap.halfWidths, int, overlapLevel )/100.0 );


      /* give it tags like "proc_%d", "state_%d", and "color_bg" */
    tags[0] = tl->colorBgTag;
/*
    tags[1] = tl->procTags[proc];
    tags[2] = tl->stateTags[type];
*/
    rectInfo.ntags = 1;
      /* process and state tags are not used right now--don't assign them */

      /* fill with background color -- this is the coverup rectangle */
    rectInfo.fillColor = tl->bgCol;

    rectMask = FEATHER_FILLCOLOR | FEATHER_TAGLIST;

    id = Feather_CreateRect( tl->interp, tl->canvasPtr,
			     x1, y1, x2, y2, &rectInfo, rectMask );
    if (id == -1) return TCL_ERROR;
    state_info.canvasId1 = id;

      /* give it tags like "color_%d" and "color_outline" */
    tags[0] = tl->colorTags[type];
    tags[1] = tl->colorOutlineTag;
    rectInfo.ntags = 2;
    rectMask = FEATHER_TAGLIST;
    
    if (tl->bw) {
        /* if in b&w, fill it in a stipple pattern with the outline color */
      rectInfo.fillColor = tl->outlineCol;
      rectInfo.fillStipple = tl->fillStipples[type];
      rectInfo.outlineColor = tl->outlineCol;
      rectMask |= FEATHER_FILLCOLOR | FEATHER_FILLSTIPPLE |
	FEATHER_OUTLINECOLOR;
    } else {
      rectInfo.fillColor = tl->fillColors[type];
      rectInfo.outlineColor = tl->outlineCol;
      rectMask |= FEATHER_FILLCOLOR | FEATHER_OUTLINECOLOR;
    }
    id = Feather_CreateRect( tl->interp, tl->canvasPtr,
			     x1, y1, x2, y2, &rectInfo, rectMask );

    if (id == -1) return TCL_ERROR;
    state_info.canvasId2 = id;

      /* if this state has a firstChild */
    if (firstChild >= 0) {
        /* put the rectangle below this state's first child */
      id = ListItem( tl->stateList, tl_stateInfo, firstChild ).canvasId1;
      Feather_Lower( tl->interp, tl->canvasPtr, state_info.canvasId2, id );
      Feather_Lower( tl->interp, tl->canvasPtr,
		     state_info.canvasId1, state_info.canvasId2 );
    }

      /* keep track of the canvas id's of each state bar */
    ListAddItem( tl->stateList, tl_stateInfo, state_info );
  }
  
  return TCL_OK;
}



static int DrawMsgs( tl, canvasPtr, canvasWin )
timeLineInfo *tl;
void *canvasPtr;
Tk_Window canvasWin;
{
  Feather_LineInfo lineInfo;
  unsigned long lineMask;
  Tk_Uid tags[3];
  int n, i;
  double points[4], sendTime, recvTime;
  int type, sender, receiver, size;

/*
  printf( "%d messages to draw.\n", Log_Nmsgs( tl->log ) );
*/
  n = Log_Nmsgs( tl->log );
  if (!n) return TCL_OK;

  tl->msgIds = (int*)malloc( sizeof(int) * n );

  lineInfo.tagList = tags;

  tags[0] = tl->colorArrowTag;
  lineInfo.ntags = 1;
  
  lineInfo.fg = tl->msgCol;
  lineInfo.arrow = last;
  lineMask = FEATHER_FILLCOLOR | FEATHER_TAGLIST | FEATHER_ARROW;

  for (i=0; i<n; i++) {
    Log_GetMsg( tl->log, i, &type, &sender, &receiver, &sendTime,
	        &recvTime, &size );

    /* This handles any sends/receives from MPI_PROC_NULL that we might
       reach here */
    if (sender < 0 || receiver < 0) continue;

    points[0] = Time2Pix( tl, sendTime );
    points[1] = Proc2Pix( tl, sender + .5 );
    points[2] = Time2Pix( tl, recvTime );
    points[3] = Proc2Pix( tl, receiver + .5 );

/*
    printf( "arrow from (%d,%f) to (%d,%f)\n",
	    sender, sendTime, receiver, recvTime );
*/

    tl->msgIds[i] = Feather_CreateLine( tl->interp, tl->canvasPtr,
				        2, points, 0, &lineInfo, lineMask );
    if (tl->msgIds[i] == -1) {
      return TCL_ERROR;
    }
  }

  return TCL_OK;
}


/*
   Get the current item from the canvas, figure out what kind of
   object it is (state, event, or message) and return the type
   and index of the item, for example, {state 35}
*/
static int CurrentItem( tl )
timeLineInfo *tl;
{
  int i, n;
  int canvas_id;
  char cmd[TMP_CMD_LEN];
  tl_stateInfo *statePtr;

  sprintf( cmd, "%s find withtag current", tl->canvasName );
  if (TCL_OK != Tcl_Eval( tl->interp, cmd )) {
    return TCL_ERROR;
  }

    /* if the result string is empty, there is no current item,
       so return an empty string (leave result alone). */
  if (tl->interp->result[0] == 0) return TCL_OK;

    /* get the canvas id */
  sscanf( tl->interp->result, "%d", &canvas_id );

  n = ListSize( tl->stateList, tl_stateInfo );
  statePtr = ListHeadPtr( tl->stateList, tl_stateInfo );
  for (i=0; i<n; i++,statePtr++) {
    if (canvas_id == statePtr->canvasId2) {
        /* interp->result is supposed to point to about 200 bytes
	   of useable memory, so we can use it */
      sprintf( tl->interp->result, "state %d", i );
      return TCL_OK;
    }
  }

    /* if the id does not match any item, return empty string */
  tl->interp->result[0] = 0;

  return TCL_OK;
}


